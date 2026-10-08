#include <Arduino.h>
#include "sensores/ldr.h"

#define LDR_PIN 35

void iniciarLDR() {
    pinMode(LDR_PIN, INPUT);
    analogReadResolution(12);

    Serial.println("LDR analogico iniciado.");
}

int lerLDR() {
    int leitura = analogRead(LDR_PIN);

    leitura = constrain(leitura, 0, 3358);

    int lux = (3358 - leitura) * 120000L / 3358;

    return lux;
}