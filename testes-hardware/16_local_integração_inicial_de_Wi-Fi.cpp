#include <Arduino.h>

#include "sensores/dht22.h"
#include "sensores/ldr.h"
#include "sensores/encoderAnemometro.h"
#include "sensores/encoderBiruta.h"
#include "atuadores/rgb.h"
#include "display/lcd.h"
#include "comunicacao/wifi.h"

unsigned long ultimaLeituraSensores = 0;
const unsigned long intervaloSensores = 2000;

void setup() {

    Serial.begin(115200);

    iniciarDHT22();
    iniciarLDR();
    iniciarEncoderAnemometro();
    iniciarEncoderBiruta();
    iniciarRGB();
    iniciarLCD();

    Serial.println("AEOLUS INICIANDO...");

    iniciarWiFi();
    
}

void loop() {

    // Atualiza constantemente a posição da biruta
    atualizarEncoderBiruta();

    unsigned long agora = millis();

    // Faz a leitura dos sensores a cada 2 segundos
    if (agora - ultimaLeituraSensores >= intervaloSensores) {

        float temperatura = lerTemperatura();
        float umidade = lerUmidade();
        int luminosidade = lerLDR();

        float velocidade = obterVelocidadeAnemometro();

        int angulo = obterAnguloBiruta();
        const char* direcao = obterDirecaoBiruta();

        // Mostra os dados no Serial Monitor
        Serial.println();
        Serial.println("----- LEITURA DOS SENSORES -----");

        Serial.print("Temperatura: ");
        Serial.print(temperatura);
        Serial.println(" C");

        Serial.print("Umidade: ");
        Serial.print(umidade);
        Serial.println(" %");

        Serial.print("Luminosidade: ");
        Serial.println(luminosidade);

        Serial.print("Velocidade do vento: ");
        Serial.print(velocidade);
        Serial.println(" km/h");

        Serial.print("Direcao do vento: ");
        Serial.print(direcao);
        Serial.print(" (");
        Serial.print(angulo);
        Serial.println(" graus)");

        Serial.println("--------------------------------");

        // Atualiza o LCD
        mostrarDadosLCD(temperatura, umidade);

        // Atualiza o RGB de acordo com a velocidade do vento
        atualizarCorPelaVelocidade(velocidade);

        ultimaLeituraSensores = agora;
    }
}