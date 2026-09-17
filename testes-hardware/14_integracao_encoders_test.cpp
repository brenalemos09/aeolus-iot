#include <Arduino.h>

#include "sensores/encoderAnemometro.h"
#include "sensores/encoderBiruta.h"

unsigned long ultimaImpressao = 0;
const unsigned long intervaloImpressao = 1000;

void setup() {
    Serial.begin(115200);

    iniciarEncoderAnemometro();
    iniciarEncoderBiruta();

    Serial.println("Teste de integracao: Encoder Anemometro + Encoder Biruta");
}

void loop() {
    atualizarEncoderBiruta();

    unsigned long agora = millis();

    if (agora - ultimaImpressao >= intervaloImpressao) {
        float velocidade = obterVelocidadeAnemometro();
        int angulo = obterAnguloBiruta();
        const char* direcao = obterDirecaoBiruta();

        Serial.print("Velocidade: ");
        Serial.print(velocidade);
        Serial.print(" km/h | Angulo: ");
        Serial.print(angulo);
        Serial.print(" | Direcao: ");
        Serial.println(direcao);

        ultimaImpressao = agora;
    }
}