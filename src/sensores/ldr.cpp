#include <Arduino.h>

#include "sensores/ldr.h"

#define LDR_PIN 35

void iniciarLDR() {
    pinMode(LDR_PIN, INPUT);
}

int lerLDR() {
    return analogRead(LDR_PIN);
}