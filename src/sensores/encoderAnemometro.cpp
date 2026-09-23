#include <Arduino.h>

#include "sensores/encoderAnemometro.h"

const int pinClkAnemometro = 21;

// Constante física do anemômetro.
// Considera 20 pulsos por volta.
const float raioAnemometro = 0.05;
const float arcoPorPulso =
    (2.0 * PI * raioAnemometro) / 20.0;

volatile unsigned long contadorPulsosAnemometro = 0;
volatile unsigned long ultimoPulsoMicros = 0;

unsigned long ultimoCalculoMs = 0;
const unsigned long intervaloMinimoCalculoMs = 1000;

float velocidadeAtualKmh = 0;

void IRAM_ATTR contarPulsoAnemometro() {
    unsigned long agora = micros();

    // Ignora pulsos ocorridos em menos de 5 ms.
    if (agora - ultimoPulsoMicros > 5000) {
        contadorPulsosAnemometro++;
        ultimoPulsoMicros = agora;
    }
}

void iniciarEncoderAnemometro() {
    pinMode(pinClkAnemometro, INPUT_PULLUP);

    attachInterrupt(
        digitalPinToInterrupt(pinClkAnemometro),
        contarPulsoAnemometro,
        FALLING
    );

    ultimoCalculoMs = millis();

    Serial.println(
        "Encoder do anemometro (velocidade) iniciado."
    );
}

void calcularVelocidadeAnemometro() {
    unsigned long agora = millis();
    unsigned long tempoDecorridoMs =
        agora - ultimoCalculoMs;

    if (
        tempoDecorridoMs >=
        intervaloMinimoCalculoMs
    ) {
        noInterrupts();

        unsigned long pulsos =
            contadorPulsosAnemometro;

        contadorPulsosAnemometro = 0;

        interrupts();

        float tempoDecorridoSegundos =
            tempoDecorridoMs / 1000.0;

        float pulsosPorSegundo =
            pulsos / tempoDecorridoSegundos;

        float velocidadeMs =
            pulsosPorSegundo * arcoPorPulso;

        velocidadeAtualKmh =
            velocidadeMs * 3.6;

        ultimoCalculoMs = agora;
    }
}

float obterVelocidadeAnemometro() {
    calcularVelocidadeAnemometro();

    return velocidadeAtualKmh;
}