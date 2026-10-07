#include <Arduino.h>

#define CLK_PIN 16  // RX2
#define DT_PIN  17  // TX2

#define PASSOS_POR_VOLTA 20

int ultimoEstadoCLK;
int posicao = 0;

void setup() {
    Serial.begin(115200);

    pinMode(CLK_PIN, INPUT);
    pinMode(DT_PIN, INPUT);

    ultimoEstadoCLK = digitalRead(CLK_PIN);

    Serial.println("Teste Encoder 2 - Posicao");
}

void loop() {
    int estadoAtualCLK = digitalRead(CLK_PIN);

    if (estadoAtualCLK != ultimoEstadoCLK) {
        if (digitalRead(DT_PIN) != estadoAtualCLK) {
            posicao++;
        } else {
            posicao--;
        }

        if (posicao < 0) {
            posicao = PASSOS_POR_VOLTA - 1;
        }
        if (posicao >= PASSOS_POR_VOLTA) {
            posicao = 0;
        }

        Serial.print("Posicao: ");
        Serial.println(posicao);
    }

    ultimoEstadoCLK = estadoAtualCLK;
}