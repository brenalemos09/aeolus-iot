#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "comunicacao/mqtt.h"


WiFiClient clienteWiFi;
PubSubClient clienteMQTT(clienteWiFi);


const char* brokerMQTT = "broker.hivemq.com";
const int portaMQTT = 1883;

const char* topicoDados = "aeolus/sensor/dados";


// ==========================================
// CONECTA AO MQTT
// ==========================================

void conectarMQTT() {

    while (!clienteMQTT.connected()) {

        Serial.print("Conectando ao MQTT...");

        String idCliente = "aeolus-";

        idCliente +=
            String(
                (uint32_t)ESP.getEfuseMac(),
                HEX
            );


        if (
            clienteMQTT.connect(
                idCliente.c_str()
            )
        ) {

            Serial.println(" conectado!");

        } else {

            Serial.print(
                " falhou. Estado: "
            );

            Serial.println(
                clienteMQTT.state()
            );

            delay(2000);
        }
    }
}


// ==========================================
// INICIA MQTT
// ==========================================

void iniciarMQTT() {

    clienteMQTT.setServer(
        brokerMQTT,
        portaMQTT
    );

    conectarMQTT();
}


// ==========================================
// MANTÉM MQTT CONECTADO
// ==========================================

void manterMQTT() {

    if (!clienteMQTT.connected()) {

        conectarMQTT();
    }

    clienteMQTT.loop();
}


// ==========================================
// PUBLICA OS DADOS DO AEOLUS
// ==========================================

void publicarDadosMQTT(
    float temperatura,
    float umidade,
    int luminosidade,
    float velocidade,
    int angulo,
    const char* direcao
) {

    // DHT22
    // O próprio módulo retorna -1
    // quando ocorre erro de leitura.

    bool statusDHT =
        temperatura != -1 &&
        umidade != -1;


    // Wi-Fi

    bool statusWiFi =
        WiFi.status() == WL_CONNECTED;


    // MQTT

    bool statusMQTT =
        clienteMQTT.connected();


    // ======================================
    // MONTA O JSON
    // ======================================

    String json = "{";


    // TEMPERATURA

    json += "\"temperatura\":";
    json += String(temperatura, 2);


    // UMIDADE

    json += ",\"umidade\":";
    json += String(umidade, 2);


    // LUMINOSIDADE

    json += ",\"luminosidade\":";
    json += String(luminosidade);


    // VELOCIDADE DO VENTO

    json += ",\"velocidade\":";
    json += String(velocidade, 2);


    // ÂNGULO DA BIRUTA

    json += ",\"angulo\":";
    json += String(angulo);


    // DIREÇÃO DO VENTO

    json += ",\"direcao\":\"";
    json += direcao;
    json += "\"";


    // STATUS DHT22

    json += ",\"statusDHT\":";

    if (statusDHT) {

        json += "true";

    } else {

        json += "false";
    }


    // STATUS WI-FI

    json += ",\"statusWiFi\":";

    if (statusWiFi) {

        json += "true";

    } else {

        json += "false";
    }


    // STATUS MQTT

    json += ",\"statusMQTT\":";

    if (statusMQTT) {

        json += "true";

    } else {

        json += "false";
    }


    json += "}";


    // ======================================
    // PUBLICA
    // ======================================

    clienteMQTT.publish(
        topicoDados,
        json.c_str()
    );


    Serial.print(
        "MQTT enviado: "
    );

    Serial.println(json);
}