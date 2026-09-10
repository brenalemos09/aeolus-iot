# 🌬️ Desafio Aeolus — Estação Meteorológica IoT

Projeto desenvolvido durante o Programa de Estágio em IoT do **Vortex Lab — UNIFOR**.

O **Desafio Aeolus** consiste no desenvolvimento de uma estação meteorológica inteligente capaz de coletar, processar, exibir e transmitir dados meteorológicos em tempo real.

O projeto é inspirado em **Aeolus (Éolo)**, figura da mitologia grega associada aos ventos, e integra conceitos de IoT, sistemas embarcados, eletrônica, comunicação MQTT, fabricação digital e desenvolvimento de PCB.

> 🚧 Projeto em desenvolvimento.

---

## 🎯 Objetivo

Desenvolver uma estação meteorológica utilizando **ESP32** capaz de monitorar:

- 🌡️ Temperatura;
- 💧 Umidade relativa do ar;
- 💨 Velocidade do vento;
- 🧭 Direção do vento.

Os dados serão exibidos localmente em um **display OLED** e enviados via **Wi-Fi e MQTT** para um dashboard de monitoramento em tempo real.

---

## ⚙️ Tecnologias e componentes

### Hardware

- ESP32 DevKit V1
- DHT22
- Display OLED 0.96" I2C (SSD1306)
- Reed switch + ímã
- Cata-vento com sensor de direção
- LED RGB endereçável WS2812/NeoPixel
- Anemômetro fabricado por impressão 3D
- PCB artesanal
- Protoboard e componentes eletrônicos

### Software

- C/C++ (Arduino)
- Arduino IDE / PlatformIO
- MQTT
- JSON
- Git e GitHub
- KiCad ou EasyEDA
- Plataforma de dashboard a definir

---

## 🏛️ Identidade do projeto

Além dos requisitos técnicos, o projeto utiliza uma identidade visual e conceitual inspirada na mitologia grega.

### O Odre de Aeolus

O case responsável por abrigar a eletrônica da estação foi denominado **O Odre de Aeolus**, inspirado no recipiente mitológico no qual os ventos eram mantidos.

### Escala dos Ventos

A velocidade do vento será representada por quatro níveis:

| Velocidade | Classificação | RGB |
|---|---|---|
| 0–10 km/h | Aura | Verde |
| 11–25 km/h | Zéfiro | Amarelo |
| 26–40 km/h | Noto | Laranja |
| Acima de 40 km/h | Fúria de Bóreas | Vermelho |

A classificação será integrada ao **LED RGB, display OLED e dashboard**.

### Templo de Aeolus

O dashboard da estação será denominado **Templo de Aeolus** e será responsável pela visualização remota dos dados meteorológicos.

---

## 📡 Fluxo do sistema

```text
Sensores
   ↓
 ESP32
   ├──→ Display OLED
   ├──→ LED RGB
   │
   ↓
 Wi-Fi
   ↓
 MQTT
   ↓
 Dashboard
