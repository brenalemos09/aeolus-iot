#include <Arduino.h>

#define CLK_PIN 21

volatile unsigned long contadorPulsos = 0;
volatile unsigned long ultimoPulso = 0;

void IRAM_ATTR contarPulso() {
    unsigned long agora = micros();

    if (agora - ultimoPulso > 5000) {
        contadorPulsos++;
        ultimoPulso = agora;
    }
}

unsigned long ultimaExibicao = 0;

void setup() {
    Serial.begin(115200);

    pinMode(CLK_PIN, INPUT_PULLUP);

    attachInterrupt(
        digitalPinToInterrupt(CLK_PIN),
        contarPulso,
        FALLING
    );

    Serial.println("Teste do anemometro");
    Serial.println("Gire exatamente uma volta completa.");
}

void loop() {
    if (millis() - ultimaExibicao >= 500) {

        noInterrupts();
        unsigned long pulsos = contadorPulsos;
        interrupts();

        Serial.print("Total de pulsos: ");
        Serial.println(pulsos);

        ultimaExibicao = millis();
    }
}