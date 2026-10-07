#include <Arduino.h>
#include <math.h>

#include "atuadores/rgb.h"

namespace {
constexpr int RED_PIN = 26;
constexpr int GREEN_PIN = 25;
constexpr int BLUE_PIN = 33;

struct CorRGB {
    int vermelho;
    int verde;
    int azul;
};

const CorRGB CORES[] = {
    {  0,   0,   0},  // Desligado
    {  0,   0, 255},  // Azul
    {  0, 255, 255},  // Ciano
    {  0, 255,   0},  // Verde
    {255,   0,   0}   // Vermelho
};

// Limites em km/h.
const float LIMITES[] = {
    0.1f,
    0.5f,
    1.0f,
    3.0f
};

// Folga de 10% para evitar oscilar entre cores.
constexpr float HISTERESE = 0.10f;

constexpr int TOTAL_CORES =
    sizeof(CORES) / sizeof(CORES[0]);

int faixaAtual = 0;

void escreverBrilho(int pino, int brilho) {
    // Teste fisico confirmou: HIGH acende e LOW apaga.
    analogWrite(pino, constrain(brilho, 0, 255));
}

void aplicarCor(const CorRGB& cor) {
    definirCorRGB(
        cor.vermelho,
        cor.verde,
        cor.azul
    );
}
} // namespace

void definirCorRGB(int vermelho, int verde, int azul) {
    escreverBrilho(RED_PIN, vermelho);
    escreverBrilho(GREEN_PIN, verde);
    escreverBrilho(BLUE_PIN, azul);
}

void iniciarRGB() {
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);

    desligarRGB();
}

void desligarRGB() {
    faixaAtual = 0;
    definirCorRGB(0, 0, 0);
}

void acenderVermelho() {
    definirCorRGB(255, 0, 0);
}

void acenderVerde() {
    definirCorRGB(0, 255, 0);
}

void acenderAzul() {
    definirCorRGB(0, 0, 255);
}

void atualizarCorPelaVelocidade(float velocidadeKmh) {
    if (!isfinite(velocidadeKmh) || velocidadeKmh < 0) {
        desligarRGB();
        return;
    }

    int faixa = faixaAtual;

    // Aumenta a faixa quando ultrapassa o limite com folga.
    while (
        faixa < TOTAL_CORES - 1 &&
        velocidadeKmh > LIMITES[faixa] * (1.0f + HISTERESE)
    ) {
        faixa++;
    }

    // Diminui a faixa quando cai abaixo do limite com folga.
    while (
        faixa > 0 &&
        velocidadeKmh < LIMITES[faixa - 1] * (1.0f - HISTERESE)
    ) {
        faixa--;
    }

    faixaAtual = faixa;
    aplicarCor(CORES[faixaAtual]);
}