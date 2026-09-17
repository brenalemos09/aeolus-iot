#include <Arduino.h>

#define CLK_PIN 18  // D18

volatile int contadorPulsos = 0;
volatile unsigned long ultimoPulso = 0;

void IRAM_ATTR contarPulso() {
    unsigned long agora = micros();
    if (agora - ultimoPulso > 2000) {  // ignora pulsos com menos de 2ms de intervalo
        contadorPulsos++;
        ultimoPulso = agora;
    }
}

unsigned long ultimoTempo = 0;
const unsigned long INTERVALO = 1000; // 1 segundo

void setup() {
    Serial.begin(115200);

    pinMode(CLK_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(CLK_PIN), contarPulso, FALLING);

    Serial.println("Teste Encoder 1 - Pulsos por segundo");
}

void loop() {
    unsigned long agora = millis();

    if (agora - ultimoTempo >= INTERVALO) {
        Serial.print("Pulsos no ultimo segundo: ");
        Serial.println(contadorPulsos);

        contadorPulsos = 0;
        ultimoTempo = agora;
    }
}