#include <Arduino.h>

#define CLK_PIN 16   // RX2
#define DT_PIN  17   // TX2
#define BTN_CALIBRACAO 23  // D23

#define PASSOS_POR_VOLTA 20

int ultimoEstadoCLK;
int posicao = 0;
int referenciaNorte = 0;

String obterDirecaoCardinal(int angulo) {
    if (angulo < 23)  return "Norte";
    if (angulo < 68)  return "Nordeste";
    if (angulo < 113) return "Leste";
    if (angulo < 158) return "Sudeste";
    if (angulo < 203) return "Sul";
    if (angulo < 248) return "Sudoeste";
    if (angulo < 293) return "Oeste";
    if (angulo < 338) return "Noroeste";
    return "Norte";
}

void setup() {
    Serial.begin(115200);

    pinMode(CLK_PIN, INPUT);
    pinMode(DT_PIN, INPUT);
    pinMode(BTN_CALIBRACAO, INPUT_PULLUP);

    ultimoEstadoCLK = digitalRead(CLK_PIN);

    Serial.println("Teste Direcao Cardinal");
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
    }

    ultimoEstadoCLK = estadoAtualCLK;

    if (digitalRead(BTN_CALIBRACAO) == LOW) {
        referenciaNorte = posicao;
        Serial.print("Calibrado! Referencia Norte = posicao ");
        Serial.println(referenciaNorte);
        delay(300);
    }

    int posicaoRelativa = posicao - referenciaNorte;
    if (posicaoRelativa < 0) {
        posicaoRelativa += PASSOS_POR_VOLTA;
    }

    int angulo = posicaoRelativa * 18;

    Serial.print("Posicao bruta: ");
    Serial.print(posicao);
    Serial.print(" | Angulo: ");
    Serial.print(angulo);
    Serial.print(" | Direcao: ");
    Serial.println(obterDirecaoCardinal(angulo));
}