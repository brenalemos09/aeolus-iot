#include <Arduino.h>

#include "sensores/ldr.h"

namespace {
constexpr int LDR_PIN = 35;
}

void iniciarLDR() {
    pinMode(LDR_PIN, INPUT);
    analogReadResolution(12);

    Serial.println("LDR analogico iniciado no GPIO35.");
}

int lerLDR() {
    return analogRead(LDR_PIN);
}