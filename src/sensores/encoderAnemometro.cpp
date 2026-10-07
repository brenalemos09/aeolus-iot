#include <Arduino.h>
#include <soc/gpio_reg.h>

#include "sensores/encoderAnemometro.h"

namespace {
constexpr int PIN_CLK = 16;
constexpr int PIN_DT = 17;
constexpr int PIN_SW = 19;

constexpr int PASSOS_VOLTA = 80;
constexpr int SENTIDO = 1;

const float PERIMETRO_M = 2.0f * PI * 0.042f;
constexpr float K_CALIB = 1.0f;
constexpr unsigned long VALIDADE_MS = 3000;

const int8_t TAB[16] = {
     0, -1,  1,  0,
     1,  0,  0, -1,
    -1,  0,  0,  1,
     0,  1, -1,  0
};

portMUX_TYPE encoderMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint8_t estadoAnt = 0;
volatile int8_t ultSent = 1;
volatile long pos = 0;

long zero = 0;
bool calibrado = false;

int setorAnt = -1;
long posEntrada = 0;
unsigned long tEntrada = 0;

float ultimaVelocidade[8] = {};
unsigned long instanteAmostra[8] = {};
bool temAmostra[8] = {};
float velocidadeAtualKmh = 0;

int swUltimaLeitura = HIGH;
int swEstavel = HIGH;
unsigned long swUltimaMudanca = 0;

void IRAM_ATTR lerEncoder() {
    portENTER_CRITICAL_ISR(&encoderMux);

    uint32_t entrada = REG_READ(GPIO_IN_REG);
    uint8_t atual =
        (((entrada >> PIN_CLK) & 1) << 1) |
        ((entrada >> PIN_DT) & 1);

    if (atual != estadoAnt) {
        int8_t d = TAB[(estadoAnt << 2) | atual];

        // Mesma compensação do código da Marianna.
        if (d == 0) {
            d = 2 * ultSent;
        } else {
            ultSent = d;
        }

        pos += d;
        estadoAnt = atual;
    }

    portEXIT_CRITICAL_ISR(&encoderMux);
}

long lerPosicao() {
    portENTER_CRITICAL(&encoderMux);
    long copia = pos;
    portEXIT_CRITICAL(&encoderMux);
    return copia;
}

int setorDe(long p) {
    long q = ((p - zero) * SENTIDO) % PASSOS_VOLTA;

    if (q < 0) {
        q += PASSOS_VOLTA;
    }

    return ((q + 5) / 10) % 8;
}

void limparLeituras() {
    for (int i = 0; i < 8; i++) {
        ultimaVelocidade[i] = 0;
        instanteAmostra[i] = 0;
        temAmostra[i] = false;
    }

    velocidadeAtualKmh = 0;
    setorAnt = -1;
}
} // namespace

void iniciarEncoderAnemometro() {
    pinMode(PIN_CLK, INPUT_PULLUP);
    pinMode(PIN_DT, INPUT_PULLUP);
    pinMode(PIN_SW, INPUT_PULLUP);

    uint32_t entrada = REG_READ(GPIO_IN_REG);
    estadoAnt =
        (((entrada >> PIN_CLK) & 1) << 1) |
        ((entrada >> PIN_DT) & 1);

    zero = lerPosicao();
    calibrado = false;
    limparLeituras();

    swUltimaLeitura = digitalRead(PIN_SW);
    swEstavel = HIGH;
    swUltimaMudanca = millis();

    attachInterrupt(
        digitalPinToInterrupt(PIN_CLK),
        lerEncoder,
        CHANGE
    );

    attachInterrupt(
        digitalPinToInterrupt(PIN_DT),
        lerEncoder,
        CHANGE
    );

    Serial.println("Anemometro: CLK16 | DT17 | SW19.");
    Serial.println("Posicione a pa de referencia e pressione SW19.");
}

void atualizarEncoderAnemometro() {
    unsigned long ms = millis();
    int swAtual = digitalRead(PIN_SW);

    // Debounce somente do botão de calibração.
    if (swAtual != swUltimaLeitura) {
        swUltimaLeitura = swAtual;
        swUltimaMudanca = ms;
    }

    if (ms - swUltimaMudanca >= 50 && swAtual != swEstavel) {
        swEstavel = swAtual;

        if (swEstavel == LOW) {
            zero = lerPosicao();
            calibrado = true;
            limparLeituras();

            Serial.println("Anemometro calibrado pelo SW19.");
        }
    }

    if (!calibrado) {
        velocidadeAtualKmh = 0;
        return;
    }

    long p = lerPosicao();
    int setor = setorDe(p);
    unsigned long agora = micros();

    if (setorAnt < 0) {
        setorAnt = setor;
        posEntrada = p;
        tEntrada = agora;
    } else if (setor != setorAnt) {
        long passos = labs(p - posEntrada);
        unsigned long dt = agora - tEntrada;

        // Mesmos critérios de travessia do main da Marianna.
        if (
            passos >= 7 &&
            passos <= 13 &&
            dt > 0 &&
            dt < VALIDADE_MS * 1000UL
        ) {
            float distancia =
                (passos / static_cast<float>(PASSOS_VOLTA)) *
                PERIMETRO_M;

            float velocidade =
                (distancia / (dt / 1e6f)) * 3.6f * K_CALIB;

            ultimaVelocidade[setorAnt] = velocidade;
            instanteAmostra[setorAnt] = ms;
            temAmostra[setorAnt] = true;
        }

        setorAnt = setor;
        posEntrada = p;
        tEntrada = agora;
    }

    // Média das últimas velocidades dos setores ainda válidos.
    float soma = 0;
    int quantidade = 0;

    for (int i = 0; i < 8; i++) {
        if (
            temAmostra[i] &&
            ms - instanteAmostra[i] <= VALIDADE_MS
        ) {
            soma += ultimaVelocidade[i];
            quantidade++;
        }
    }

    velocidadeAtualKmh =
        quantidade > 0 ? soma / quantidade : 0;
}

float obterVelocidadeAnemometro() {
    return velocidadeAtualKmh;
}