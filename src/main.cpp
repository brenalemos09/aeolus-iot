#include <Arduino.h>

#include "sensores/dht22.h"
#include "sensores/ldr.h"
#include "sensores/encoderAnemometro.h"
#include "sensores/encoderBiruta.h"

#include "atuadores/rgb.h"
#include "display/lcd.h"

#include "comunicacao/wifi.h"
#include "comunicacao/mqtt.h"

namespace {
constexpr unsigned long INTERVALO_SENSORES = 2500;
constexpr unsigned long INTERVALO_LCD = 250;

unsigned long ultimaLeituraSensores = 0;
unsigned long ultimaAtualizacaoLCD = 0;

float temperaturaAtual = 0;
float umidadeAtual = 0;
int luminosidadeAtual = 0;

float velocidadeAtual = 0;
int anguloAtual = 0;
const char* direcaoAtual = "Norte";

void imprimirLeituras() {
    Serial.println();
    Serial.println("----- LEITURA DOS SENSORES -----");

    Serial.printf(
        "Temperatura: %.1f C | Umidade: %.1f %%\n",
        temperaturaAtual,
        umidadeAtual
    );

    Serial.printf(
        "Luminosidade estimada: %d lux\n",
        luminosidadeAtual
    );

    Serial.printf(
        "Velocidade do vento: %.3f km/h\n",
        velocidadeAtual
    );

    Serial.printf(
        "Direcao: %s | Angulo: %d graus\n",
        direcaoAtual,
        anguloAtual
    );

    Serial.println("--------------------------------");
}
} // namespace

void setup() {
    Serial.begin(115200);

    Serial.println();
    Serial.println("AEOLUS INICIANDO...");

    iniciarDHT22();
    iniciarLDR();
    iniciarRGB();

    // Executa a abertura antes de iniciar os encoders.
    iniciarLCD();

    iniciarEncoderAnemometro();
    iniciarEncoderBiruta();

    iniciarWiFi();
    iniciarMQTT();

    ultimaLeituraSensores = millis();
    ultimaAtualizacaoLCD = millis();

    Serial.println();
    Serial.println("AEOLUS INICIALIZADO.");
    Serial.println("SW19: calibracao do anemometro.");
    Serial.println("SW22: referencia Norte da biruta.");
    Serial.println("Direita GPIO32: liga/desliga LCD.");
    Serial.println("Esquerda GPIO23: passa pagina LCD.");
}

void loop() {
    atualizarEncoderAnemometro();
    atualizarEncoderBiruta();
    atualizarBotoesLCD();

    manterMQTT();

    velocidadeAtual = obterVelocidadeAnemometro();
    anguloAtual = obterAnguloBiruta();
    direcaoAtual = obterDirecaoBiruta();

    atualizarCorPelaVelocidade(velocidadeAtual);

    unsigned long agora = millis();

    if (agora - ultimaLeituraSensores >= INTERVALO_SENSORES) {
        temperaturaAtual = lerTemperatura();
        umidadeAtual = lerUmidade();
        luminosidadeAtual = lerLDR();

        imprimirLeituras();

        // Luminosidade ja convertida pelo ldr.cpp.
        publicarDadosMQTT(
            temperaturaAtual,
            umidadeAtual,
            luminosidadeAtual,
            velocidadeAtual,
            anguloAtual,
            direcaoAtual
        );

        ultimaLeituraSensores = millis();
    }

    agora = millis();

    if (agora - ultimaAtualizacaoLCD >= INTERVALO_LCD) {
        mostrarDadosLCD(
            temperaturaAtual,
            umidadeAtual,
            luminosidadeAtual,
            velocidadeAtual,
            direcaoAtual
        );

        ultimaAtualizacaoLCD = millis();
    }

    delay(1);
}