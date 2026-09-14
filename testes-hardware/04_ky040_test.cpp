#include <Arduino.h>

#define CLK_PIN 18
#define DT_PIN 34
#define SW_PIN 32

int ultimoCLK;
int ultimoBotao = HIGH;

void setup() {
  Serial.begin(115200);

  pinMode(CLK_PIN, INPUT);
  pinMode(DT_PIN, INPUT);
  pinMode(SW_PIN, INPUT_PULLUP);

  ultimoCLK = digitalRead(CLK_PIN);

  Serial.println("Teste Encoder");
}

void loop() {
  int estadoCLK = digitalRead(CLK_PIN);

  if (ultimoCLK == HIGH && estadoCLK == LOW) {
    if (digitalRead(DT_PIN) == HIGH) {
      Serial.println("Sentido horario");
    } else {
      Serial.println("Sentido anti-horario");
    }

    delay(20);
  }

  ultimoCLK = estadoCLK;

  int estadoBotao = digitalRead(SW_PIN);

  if (ultimoBotao == HIGH && estadoBotao == LOW) {
    Serial.println("Botao pressionado");
    delay(50);
  }

  ultimoBotao = estadoBotao;
}