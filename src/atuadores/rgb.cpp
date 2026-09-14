#include <Arduino.h>

#include "atuadores/rgb.h"

#define RED_PIN 25
#define GREEN_PIN 26
#define BLUE_PIN 33

void iniciarRGB() {
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);

    desligarRGB();
}

void desligarRGB() {
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, HIGH);
}

void acenderVermelho() {
    desligarRGB();
    digitalWrite(RED_PIN, LOW);
}

void acenderVerde() {
    desligarRGB();
    digitalWrite(GREEN_PIN, LOW);
}

void acenderAzul() {
    desligarRGB();
    digitalWrite(BLUE_PIN, LOW);
}