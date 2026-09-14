#include <Arduino.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

void setup(){
  Serial.begin(115200);
  dht.begin();
  Serial.println("Teste DHT22");
}

void loop(){
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.print("C");
  Serial.print(" Umidade: ");
  Serial.print(umidade);
  Serial.println("%");
  delay(2000);
}