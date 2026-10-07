#include <Arduino.h>
#include <DHT.h>

namespace {
DHT testeDhtIsolado(4, DHT22);
}

void setup() {
    Serial.begin(115200);
    testeDhtIsolado.begin();

    Serial.println("Teste DHT22 - GPIO4");
    delay(2500);
}

void loop() {
    float temperatura = testeDhtIsolado.readTemperature();
    float umidade = testeDhtIsolado.readHumidity();

    if (isnan(temperatura) || isnan(umidade)) {
        Serial.println("Falha na leitura do DHT22.");
    } else {
        Serial.print("Temperatura: ");
        Serial.print(temperatura);
        Serial.print(" C | Umidade: ");
        Serial.print(umidade);
        Serial.println(" %");
    }

    delay(2500);
}