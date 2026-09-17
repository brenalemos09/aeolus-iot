#include <Arduino.h>

#define BTN_CALIBRACAO 23  // D23

void setup() {
    Serial.begin(115200);

    pinMode(BTN_CALIBRACAO, INPUT_PULLUP);

    Serial.println("Teste Botao de Calibracao (D23)");
}

void loop() {
    if (digitalRead(BTN_CALIBRACAO) == LOW) {
        Serial.println("Calibracao acionada");
        delay(300);
    }
}