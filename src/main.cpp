#include <Arduino.h>

#include "comunicacao/wifi.h"

void setup() {

    Serial.begin(115200);

    Serial.println("TESTE WIFI ISOLADO");

    iniciarWiFi();
}

void loop() {

}