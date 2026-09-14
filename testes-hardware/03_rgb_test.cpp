#include <Arduino.h>
#define RED 25
#define GREEN 26
#define BLUE 33

void setup(){
  pinMode(RED, OUTPUT);
  pinMode(GREEN,OUTPUT);
  pinMode(BLUE,OUTPUT);

  Serial.begin(115200);
  Serial.println("Teste RGB");
}

void loop(){
  Serial.println("Vermelho");
  digitalWrite(RED,HIGH);
  digitalWrite(GREEN,LOW);
  digitalWrite(BLUE,LOW);
  delay(1000);

  Serial.println("Verde");
  digitalWrite(RED,LOW);  
  digitalWrite(GREEN,HIGH);
  digitalWrite(BLUE,LOW);

  Serial.println("Azul");
  digitalWrite(RED,LOW);
  digitalWrite(GREEN,LOW);
  digitalWrite(BLUE,HIGH);
  delay(1000);
}