#ifndef RGB_H
#define RGB_H

void iniciarRGB();
void desligarRGB();

void acenderVermelho();
void acenderVerde();
void acenderAzul();

void definirCorRGB(
    int vermelho,
    int verde,
    int azul
);

void atualizarCorPelaVelocidade(
    float velocidadeKmh
);

#endif