#include <Arduino.h>

#include "sensores/encoderBiruta.h"

const int pinClkBiruta = 16;         // RX2
const int pinDtBiruta = 17;          // TX2
const int pinBtnCalibracao = 23;     // D23

const int passosPorVolta = 20;
const int grausPorPasso = 18;

int ultimoEstadoClkBiruta;
int posicaoBiruta = 0;
int referenciaNorte = 0;

const char* obterNomeDirecao(int angulo) {
    if (angulo < 23)  return "Norte";
    if (angulo < 68)  return "Nordeste";
    if (angulo < 113) return "Leste";
    if (angulo < 158) return "Sudeste";
    if (angulo < 203) return "Sul";
    if (angulo < 248) return "Sudoeste";
    if (angulo < 293) return "Oeste";
    if (angulo < 338) return "Noroeste";
    return "Norte";
}

void iniciarEncoderBiruta() {
    pinMode(pinClkBiruta, INPUT);
    pinMode(pinDtBiruta, INPUT);
    pinMode(pinBtnCalibracao, INPUT_PULLUP);

    ultimoEstadoClkBiruta = digitalRead(pinClkBiruta);

    Serial.println("Encoder da biruta (direcao) iniciado.");
}

void atualizarEncoderBiruta() {
    int estadoAtualClk = digitalRead(pinClkBiruta);

    if (estadoAtualClk != ultimoEstadoClkBiruta) {
        if (digitalRead(pinDtBiruta) != estadoAtualClk) {
            posicaoBiruta++;
        } else {
            posicaoBiruta--;
        }

        if (posicaoBiruta < 0) {
            posicaoBiruta = passosPorVolta - 1;
        }
        if (posicaoBiruta >= passosPorVolta) {
            posicaoBiruta = 0;
        }
    }

    ultimoEstadoClkBiruta = estadoAtualClk;

    if (digitalRead(pinBtnCalibracao) == LOW) {
        referenciaNorte = posicaoBiruta;
        Serial.print("Biruta calibrada! Referencia Norte = posicao ");
        Serial.println(referenciaNorte);
        delay(300);
    }
}

int obterPosicaoBiruta() {
    return posicaoBiruta;
}

int obterAnguloBiruta() {
    int posicaoRelativa = posicaoBiruta - referenciaNorte;

    if (posicaoRelativa < 0) {
        posicaoRelativa += passosPorVolta;
    }

    return posicaoRelativa * grausPorPasso;
}

const char* obterDirecaoBiruta() {
    return obterNomeDirecao(obterAnguloBiruta());
}