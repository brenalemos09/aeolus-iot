#include <Arduino.h>

#define SW_PIN 19  // D19

void setup() {
    Serial.begin(115200);

    pinMode(SW_PIN, INPUT_PULLUP);

    Serial.println("Teste Encoder 2 - Biruta (SW)");
}

void loop() {
    if (digitalRead(SW_PIN) == LOW) {
        Serial.println("Botao pressionado");
        delay(300);
    }
}