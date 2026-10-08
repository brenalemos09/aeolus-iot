#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

#include "comunicacao/wifi.h"
#include "credenciais.h"

void iniciarWiFi() {
    Serial.print("Conectando ao Wi-Fi");

    WiFi.begin(WIFI_SSID, WIFI_SENHA);

    int tentativas = 0;

    while (
        WiFi.status() != WL_CONNECTED &&
        tentativas < 20
    ) {
        delay(500);
        Serial.print(".");
        tentativas++;
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Wi-Fi conectado!");

        Serial.print("IP: ");
        Serial.println(WiFi.localIP());

        // Fortaleza: UTC-3, sem ajuste de horario de verao.
        // A sincronizacao acontece em segundo plano.
        configTime(
            -3 * 60 * 60,
            0,
            "pool.ntp.org",
            "time.google.com",
            "time.cloudflare.com"
        );

        Serial.println(
            "Relogio: sincronizacao NTP iniciada (UTC-3)."
        );
    } else {
        Serial.println("Nao foi possivel conectar ao Wi-Fi.");
        Serial.println("Aeolus continuara funcionando localmente.");
        Serial.println(
            "Relogio aguardando conexao para sincronizar."
        );
    }
}

bool wifiConectado() {
    return WiFi.status() == WL_CONNECTED;
}