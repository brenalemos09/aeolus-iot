#include <Arduino.h>

#include "atuadores/rgb.h"

#define RED_PIN 26
#define GREEN_PIN 25
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

// escreve um brilho (0 = apagado, 255 = totalmente aceso)
// invertido porque o LED e common-anode
void escreverBrilho(int pino, int brilho) {
    analogWrite(pino, 255 - brilho);
}

void definirCorRGB(int r, int g, int b) {
    escreverBrilho(RED_PIN, r);
    escreverBrilho(GREEN_PIN, g);
    escreverBrilho(BLUE_PIN, b);
}

// Limiares da escala Aeolus, em km/h.
// Calibrados para teste manual/ventilador fraco no protótipo.
// Reajustar para valores reais de vento (10/25/40) apos a
// calibracao final do anemometro com vento real (Fase 3).
const float LIMIAR_AURA = 1.0;
const float LIMIAR_ZEFIRO = 3.0;
const float LIMIAR_NOTO = 6.0;

void atualizarCorPelaVelocidade(float velocidadeKmh) {
    if (velocidadeKmh <= LIMIAR_AURA) {
        definirCorRGB(0, 255, 0);      // Aura - verde
    } else if (velocidadeKmh <= LIMIAR_ZEFIRO) {
        definirCorRGB(255, 180, 0);    // Zefiro - amarelo
    } else if (velocidadeKmh <= LIMIAR_NOTO) {
        definirCorRGB(255, 80, 0);     // Noto - laranja
    } else {
        definirCorRGB(255, 0, 0);      // Furia de Boreas - vermelho
    }
}