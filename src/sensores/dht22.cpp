#include <Arduino.h> 
#include <DHT.h>
#include "sensores/dht22.h" // liga ao nosso arq header do include/sensores/dht22.h

#define DHT_PIN 4
#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE); // criando o obj dht da classe DHT

void iniciarDHT22() {
  dht.begin(); // inicializa o sensor
}

float lerTemperatura(){
    float temperatura = dht.readTemperature(); //guarda o valor lido dentro de temperatura
    if (isnan(temperatura)) { // se n for um valor valido ele retorna -1 e printa a msg de erro
        Serial.println("Erro ao ler temperatura");
        return -1;
    }
    return temperatura;
}

float lerUmidade(){
    float umidade = dht.readHumidity(); // guarda o valor da umidade lido dentro da var umidade
    if (isnan(umidade)) { // se n for um valor valido ele retorna -1 e printa a msg de erro
        Serial.println("Erro ao ler umidade");
        return -1;
    }
    return umidade;
}