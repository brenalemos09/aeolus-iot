#include <Arduino.h>

#include "sensores/encoderAnemometro.h"

const int pinClkAnemometro = 18;  // D18

// constante física do anemômetro (arco percorrido a cada pulso)
const float raioAnemometro = 0.05;
const float arcoPorPulso = (PI * raioAnemometro) / 10.0;

volatile int contadorPulsosAnemometro = 0;
volatile unsigned long ultimoPulsoMicros = 0;

unsigned long ultimoCalculoMs = 0;
const unsigned long intervaloCalculoMs = 1000;

float velocidadeAtualKmh = 0;

void IRAM_ATTR contarPulsoAnemometro() {
    unsigned long agora = micros();

    if (agora - ultimoPulsoMicros > 6000) {
        contadorPulsosAnemometro++;
        ultimoPulsoMicros = agora;
    }
}

void iniciarEncoderAnemometro() {
    pinMode(pinClkAnemometro, INPUT);

    attachInterrupt(
        digitalPinToInterrupt(pinClkAnemometro),
        contarPulsoAnemometro,
        FALLING
    );

    Serial.println("Encoder do anemometro (velocidade) iniciado.");
}

void calcularVelocidadeAnemometro() {
    unsigned long agora = millis();

    if (agora - ultimoCalculoMs >= intervaloCalculoMs) {
        float pulsosPorSegundo =
            contadorPulsosAnemometro / (intervaloCalculoMs / 1000.0);

        float velocidadeMs = pulsosPorSegundo * arcoPorPulso;

        velocidadeAtualKmh = velocidadeMs * 3.6;

        contadorPulsosAnemometro = 0;
        ultimoCalculoMs = agora;
    }
}

float obterVelocidadeAnemometro() {
    calcularVelocidadeAnemometro();
    return velocidadeAtualKmh;
}