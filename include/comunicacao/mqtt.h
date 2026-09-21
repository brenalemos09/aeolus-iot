#ifndef MQTT_H
#define MQTT_H

void iniciarMQTT();

void manterMQTT();

void publicarDadosMQTT(
    float temperatura,
    float umidade,
    int luminosidade,
    float velocidade,
    int angulo,
    const char* direcao
);

#endif