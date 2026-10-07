#include <Arduino.h>
#include <WiFi.h>

#include "comunicacao/wifi.h"
#include "credenciais.h"

void iniciarWiFi() {

    Serial.print("Conectando ao Wi-Fi");

    WiFi.begin(WIFI_SSID, WIFI_SENHA);

    int tentativas = 0;

    while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
        delay(500);
        Serial.print(".");
        tentativas++;
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {

        Serial.println("Wi-Fi conectado!");

        Serial.print("IP: ");
        Serial.println(WiFi.localIP());

    } else {

        Serial.println("Nao foi possivel conectar ao Wi-Fi.");
        Serial.println("Aeolus continuara funcionando localmente.");
    }
}

bool wifiConectado() {

    return WiFi.status() == WL_CONNECTED;
}