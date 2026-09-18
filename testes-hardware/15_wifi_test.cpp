#include <Arduino.h>
#include <WiFi.h>

#include "credenciais.h"

void setup() {
    Serial.begin(115200);
    Serial.println("Conectando ao Wi-Fi...");
    WiFi.begin(WIFI_SSID, WIFI_SENHA);

    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Wi-Fi conectado!!");
    Serial.print("Endereco IP: ");
    Serial.println(WiFi.localIP());
}

void loop(){
    //nd
}