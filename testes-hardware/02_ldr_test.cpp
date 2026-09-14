#include <Arduino.h>
#define LDR_PIN 35

void setup(){
  Serial.begin(115200);
  Serial.println("Teste LDR");
}

void loop(){
  int valorLDR = analogRead(LDR_PIN);
  Serial.print("Valor LDR: ");
  Serial.println(valorLDR);
  delay(1000);
}