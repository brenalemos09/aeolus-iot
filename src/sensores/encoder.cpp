#include <Arduino.h>

#include "sensores/encoder.h"


const int pinCLK = 18;
const int pinDT = 34;
const int pinSW = 32;


// constantes físicas

const float raio_encoder = 0.05;

const float arco_pulso = (PI * raio_encoder) / 10.0;


// variáveis do encoder

int posicaoAtual = 0;
int posicaoAnterior = 0;
int estadoAnteriorClk;

const char* NOMES_DIRECOES[8] = {
    "Norte",
    "Nordeste",
    "Leste",
    "Sudeste",
    "Sul",
    "Sudoeste",
    "Oeste",
    "Noroeste"
};


// variáveis de velocidade

unsigned long tempoUltimoPulsoMs = 0;

bool primeiroPulso = true;

float vel_kmH = 0;


// debounce

unsigned long tempoUltimaLeituraValida = 0;

const unsigned long tempoMinimoEntrePulsosMs = 3;


// acumuladores

float svpd[8] = {
    0, 0, 0, 0, 0, 0, 0, 0
};

int qapd[8] = {
    0, 0, 0, 0, 0, 0, 0, 0
};

float vmpd[8] = {
    0, 0, 0, 0, 0, 0, 0, 0
};


// converte posição de 0-19 para direção

int calcularIndiceDirecao(int posicao) {

    if (posicao < 3)  return 0;
    if (posicao < 5)  return 1;
    if (posicao < 8)  return 2;
    if (posicao < 10) return 3;
    if (posicao < 13) return 4;
    if (posicao < 15) return 5;
    if (posicao < 18) return 6;

    return 7;
}


// verifica passagem entre 19 e 0

bool verificarVoltaCompleta(int anterior, int atual) {

    if (anterior == 19 && atual == 0) {
        return true;
    }

    if (anterior == 0 && atual == 19) {
        return true;
    }

    return false;
}


// calcula médias e zera os acumuladores

void finalizarVolta() {

    for (int i = 0; i < 8; i++) {

        if (qapd[i] > 0) {

            vmpd[i] = svpd[i] / qapd[i];
        }

        svpd[i] = 0;
        qapd[i] = 0;
    }


    Serial.println(
        "--- Volta completa: velocidades atualizadas ---"
    );


    for (int i = 0; i < 8; i++) {

        Serial.print(NOMES_DIRECOES[i]);

        Serial.print(": ");

        Serial.print(vmpd[i]);

        Serial.println(" km/h");
    }
}


// recalibra a posição atual como Norte

void verificarBotaoResetNorte() {

    if (digitalRead(pinSW) == LOW) {

        posicaoAtual = 0;

        posicaoAnterior = 0;

        Serial.println(
            "Botao pressionado: posicao recalibrada para Norte (0)."
        );

        delay(300);
    }
}


// inicia o encoder

void iniciarEncoder() {

    pinMode(pinCLK, INPUT);

    pinMode(pinDT, INPUT);

    pinMode(pinSW, INPUT_PULLUP);


    estadoAnteriorClk = digitalRead(pinCLK);

    tempoUltimoPulsoMs = millis();


    Serial.println(
        "Sistema iniciado. Posicao atual = 0 (Norte)."
    );

    Serial.println(
        "CLK | DT | SW configurados. Aguardando pulsos..."
    );
}


// atualiza o encoder

void atualizarEncoder() {

    int estadoAtualClk = digitalRead(pinCLK);


    // processa somente quando CLK passa de HIGH para LOW

    if (
        estadoAnteriorClk == HIGH &&
        estadoAtualClk == LOW
    ) {

        unsigned long agora = millis();


        // debounce

        if (
            agora - tempoUltimaLeituraValida
            >= tempoMinimoEntrePulsosMs
        ) {

            tempoUltimaLeituraValida = agora;


            // calcula velocidade

            if (!primeiroPulso) {

                float deltaSegundos =
                    (agora - tempoUltimoPulsoMs) / 1000.0;

                float velocidadeMs =
                    arco_pulso / deltaSegundos;

                vel_kmH =
                    velocidadeMs * 3.6;

            } else {

                primeiroPulso = false;
            }


            tempoUltimoPulsoMs = agora;


            // guarda a posição anterior

            posicaoAnterior = posicaoAtual;


            // verifica o sentido do giro

            if (digitalRead(pinDT) != estadoAtualClk) {

                posicaoAtual =
                    (posicaoAtual + 1) % 20;

            } else {

                posicaoAtual =
                    (posicaoAtual - 1 + 20) % 20;
            }


            // descobre a direção

            int idx =
                calcularIndiceDirecao(posicaoAtual);


            // acumula a velocidade

            svpd[idx] += vel_kmH;

            qapd[idx]++;


            // mostra o resultado no Serial

            Serial.print("Pulso -> posicao: ");

            Serial.print(posicaoAtual);

            Serial.print(" | direcao: ");

            Serial.print(NOMES_DIRECOES[idx]);

            Serial.print(" | vel_kmH: ");

            Serial.println(vel_kmH);


            // verifica a passagem entre 19 e 0

            if (
                verificarVoltaCompleta(
                    posicaoAnterior,
                    posicaoAtual
                )
            ) {

                finalizarVolta();
            }
        }
    }


    // guarda o CLK atual para a próxima leitura

    estadoAnteriorClk = estadoAtualClk;


    // verifica o botão

    verificarBotaoResetNorte();
}


// devolve a posição atual

int obterPosicaoEncoder() {

    return posicaoAtual;
}


// devolve a direção atual

const char* obterDirecaoEncoder() {

    int idx =
        calcularIndiceDirecao(posicaoAtual);

    return NOMES_DIRECOES[idx];
}


// devolve a velocidade calculada

float obterVelocidadeEncoder() {

    return vel_kmH;
}