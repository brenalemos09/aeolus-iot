#include <Arduino.h>

#include "sensores/dht22.h"
#include "sensores/ldr.h"
#include "sensores/encoder.h"
#include "atuadores/rgb.h"
#include "display/lcd.h"


unsigned long ultimaLeitura = 0;

const unsigned long intervaloLeitura = 2000;


void setup() {

    Serial.begin(115200);

    iniciarDHT22();
    iniciarLDR();
    iniciarEncoder();
    iniciarRGB();
    iniciarLCD();

    Serial.println();
    Serial.println("==============================");
    Serial.println("       AEOLUS INICIADO");
    Serial.println("==============================");
}


void loop() {

    // Essa função precisa rodar o tempo inteiro
    atualizarEncoder();


    // Pega o tempo atual do ESP32
    unsigned long agora = millis();


    // A cada 2 segundos, lê e mostra os outros sensores
    if (agora - ultimaLeitura >= intervaloLeitura) {

        ultimaLeitura = agora;


        // Leitura dos sensores

        float temperatura = lerTemperatura();

        float umidade = lerUmidade();

        int luminosidade = lerLDR();

        int posicao = obterPosicaoEncoder();

        const char* direcao = obterDirecaoEncoder();


        // Monitor Serial

        Serial.println();

        Serial.println("----- DADOS DO AEOLUS -----");


        Serial.print("Temperatura: ");

        Serial.print(temperatura);

        Serial.println(" C");


        Serial.print("Umidade: ");

        Serial.print(umidade);

        Serial.println(" %");


        Serial.print("Luminosidade: ");

        Serial.println(luminosidade);


        Serial.print("Posicao do encoder: ");

        Serial.println(posicao);


        Serial.print("Direcao do vento: ");

        Serial.println(direcao);


        // LCD

        mostrarDadosLCD(
            temperatura,
            umidade
        );


        // RGB temporário baseado na temperatura

        if (temperatura < 21) {

            acenderAzul();

        } else if (temperatura < 24) {

            acenderVerde();

        } else {

            acenderVermelho();
        }


        Serial.println("---------------------------");
    }
}ss