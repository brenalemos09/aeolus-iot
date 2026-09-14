#include <Arduino.h>

#include "sensores/dht22.h"
#include "sensores/ldr.h"
#include "atuadores/rgb.h"
#include "display/lcd.h"

void setup() {
    Serial.begin(115200);

    iniciarDHT22();
    iniciarLDR();
    iniciarRGB();
    iniciarLCD();

    Serial.println("==============================");
    Serial.println("       AEOLUS INICIADO");
    Serial.println("==============================");
}

void loop() {
    float temperatura = lerTemperatura();
    float umidade = lerUmidade();
    int luminosidade = lerLDR();

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

    mostrarDadosLCD(temperatura, umidade);

    Serial.print("RGB: ");

    if (temperatura < 21) {
        acenderAzul();
        Serial.println("AZUL - temperatura baixa");
    }
    else if (temperatura < 24) {
        acenderVerde();
        Serial.println("VERDE - temperatura normal");
    }
    else {
        acenderVermelho();
        Serial.println("VERMELHO - temperatura alta");
    }

    Serial.println("-------------------------------");

    delay(2000);
}