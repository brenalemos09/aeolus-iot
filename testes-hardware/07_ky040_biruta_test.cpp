#include <Arduino.h>

#define CLK_PIN 16
#define DT_PIN 17

int ultimoEstadoCLK;

void setup(){
    Serial.begin(115200);

    pinMode(CLK_PIN, INPUT);
    pinMode(DT_PIN, INPUT);

    ultimoEstadoCLK = digitalRead(CLK_PIN);

    Serial.println("Teste encouder 2 - biruta (CLK/DT  )");
}

void loop(){
    int estadoAtualCLK = digitalRead(CLK_PIN);
    if (estadoAtualCLK != ultimoEstadoCLK) {
        if (digitalRead(DT_PIN) != estadoAtualCLK) {
            Serial.println("Sentido horário");
        } else {
            Serial.println("Sentido anti-horário");
        }
    }
    ultimoEstadoCLK = estadoAtualCLK;
}