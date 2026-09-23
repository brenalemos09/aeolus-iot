# 🌬️ Desafio Aeolus — Estação Meteorológica IoT

Projeto desenvolvido durante o Programa de Estágio em IoT do **Vortex Lab — UNIFOR**.

O **Desafio Aeolus** consiste no desenvolvimento de uma estação meteorológica inteligente capaz de coletar, processar, armazenar e exibir dados meteorológicos.

O projeto é inspirado em **Aeolus (Éolo)**, figura da mitologia grega associada aos ventos, e reúne conceitos de IoT, sistemas embarcados, eletrônica, MQTT, banco de dados, desenvolvimento web, fabricação digital e PCB.

> 🚧 Projeto em desenvolvimento.

---

## 🎯 Objetivo

Desenvolver uma estação meteorológica utilizando **ESP32**, capaz de monitorar:

- 🌡️ Temperatura;
- 💧 Umidade relativa do ar;
- ☀️ Luminosidade;
- 💨 Velocidade do vento;
- 🧭 Direção do vento.

Os dados são exibidos localmente em um **display LCD 16x2**, representados por um **LED RGB** e enviados por Wi-Fi e MQTT para um sistema de monitoramento.

As leituras também são armazenadas em um banco de dados PostgreSQL e apresentadas no dashboard em tempo real e por meio de gráfico histórico.

---

## ⚙️ Tecnologias e componentes

### Hardware

- ESP32 DevKit V1;
- Sensor DHT22;
- Sensor de luminosidade LDR;
- Display LCD 16x2 com interface I²C;
- LED RGB;
- Encoder do anemômetro;
- Encoder da biruta;
- Anemômetro fabricado por impressão 3D;
- Biruta para direção do vento;
- PCB artesanal;
- Protoboard e componentes eletrônicos.

### Firmware

- C/C++;
- Framework Arduino;
- PlatformIO;
- Wi-Fi;
- MQTT;
- JSON;
- PubSubClient;
- LiquidCrystal_I2C.

### Backend e dashboard

- Node.js;
- Express;
- PostgreSQL;
- MQTT.js;
- HTML;
- CSS;
- JavaScript;
- API REST;
- Chart.js.

### Desenvolvimento

- Git e GitHub;
- PlatformIO;
- VS Code;
- Fabricação e modelagem 3D;
- Desenvolvimento de PCB.

---

## 🔌 Pinagem consolidada

| Componente | Função | GPIO |
|---|---|---:|
| LCD I²C | SDA | 13 |
| LCD I²C | SCL | 14 |
| Encoder do anemômetro | CLK | 21 |
| LED RGB | Vermelho | 26 |
| LED RGB | Verde | 25 |
| LED RGB | Azul | 33 |

> A pinagem dos demais componentes deve ser consultada diretamente nos arquivos do firmware enquanto o protótipo estiver em desenvolvimento.

---

## 🏛️ Identidade do projeto

Além dos requisitos técnicos, o projeto possui uma identidade visual e conceitual inspirada na mitologia grega.

### O Odre de Aeolus

O case responsável por abrigar a eletrônica da estação foi denominado **O Odre de Aeolus**, inspirado no recipiente mitológico no qual os ventos eram mantidos.

### Escala Aeolus

A intensidade do vento é representada por quatro níveis:

| Velocidade | Classificação | Cor do RGB |
|---|---|---|
| 0–10 km/h | Aura | Verde |
| Acima de 10 até 25 km/h | Zéfiro | Amarelo |
| Acima de 25 até 40 km/h | Noto | Laranja |
| Acima de 40 km/h | Fúria de Bóreas | Vermelho |

> Durante os testes do protótipo, podem ser utilizados valores reduzidos para facilitar a validação manual do LED RGB. Os limites oficiais devem ser aplicados após a calibração final do anemômetro.

### Templo de Aeolus

O dashboard da estação é denominado **Templo de Aeolus** e apresenta:

- Leituras recebidas em tempo real via MQTT;
- Temperatura;
- Umidade;
- Luminosidade;
- Velocidade do vento;
- Direção e ângulo do vento;
- Estado dos componentes e conexões;
- Classificação da Escala Aeolus;
- Gráfico com o histórico das últimas leituras.

---

## 📡 Fluxo do sistema

```text
Sensores
   ↓
ESP32
   ├──→ LCD 16x2
   ├──→ LED RGB
   │
   ↓
Wi-Fi
   ↓
MQTT
   ├──→ Dashboard em tempo real
   │
   ↓
Backend Node.js
   ↓
PostgreSQL
   ↓
API REST
   ↓
Gráfico histórico do dashboard
```

---

## 📦 Formato dos dados MQTT

O ESP32 publica as leituras no tópico:

```text
aeolus/sensor/dados
```

Exemplo de mensagem:

```json
{
  "temperatura": 24.5,
  "umidade": 57.1,
  "luminosidade": 96,
  "velocidade": 0.96,
  "angulo": 0,
  "direcao": "Norte",
  "statusDHT": true,
  "statusWiFi": true,
  "statusMQTT": true
}
```

O broker utilizado durante o desenvolvimento é:

```text
broker.hivemq.com
```

---

## 🗄️ Banco de dados e API

O backend recebe as mensagens MQTT e armazena as leituras no PostgreSQL.

### Endpoints

#### Estado do backend e banco

```http
GET /api/status
```

#### Histórico das leituras

```http
GET /api/leituras
```

O endpoint de histórico retorna as últimas leituras registradas, organizadas em ordem cronológica para utilização no gráfico do dashboard.

---

## 📁 Estrutura do projeto

```text
aeolus-iot/
├── backend/
│   ├── consumidor-mqtt.js
│   ├── db.js
│   ├── server.js
│   ├── teste-banco.js
│   └── teste-mqtt.js
├── dashboard/
│   ├── index.html
│   └── script.js
├── include/
│   ├── atuadores/
│   ├── comunicacao/
│   ├── display/
│   └── sensores/
├── src/
│   ├── atuadores/
│   ├── comunicacao/
│   ├── display/
│   ├── sensores/
│   └── main.cpp
├── testes-hardware/
├── platformio.ini
└── README.md
```

---

## ▶️ Execução

### Firmware do ESP32

Abra o projeto no PlatformIO e execute:

```powershell
platformio.exe run --target upload
```

Também é possível utilizar a opção **Upload** do PlatformIO no VS Code.

### Backend

Entre na pasta do backend:

```powershell
cd backend
```

Instale as dependências:

```powershell
npm install
```

Inicie o servidor:

```powershell
node server.js
```

O backend ficará disponível em:

```text
http://localhost:3000
```

### Dashboard

Abra o arquivo:

```text
dashboard/index.html
```

O dashboard pode ser executado utilizando uma extensão de servidor local, como o Live Server do VS Code.

> O backend precisa estar em execução para que o gráfico consiga consultar o histórico armazenado no PostgreSQL.

---

## ✅ Funcionalidades validadas

- [x] Leitura de temperatura e umidade;
- [x] Leitura de luminosidade;
- [x] Detecção dos pulsos do anemômetro;
- [x] Leitura da direção do vento;
- [x] Exibição de dados no LCD;
- [x] Classificação da velocidade pelo LED RGB;
- [x] Conexão do ESP32 ao Wi-Fi;
- [x] Publicação das leituras via MQTT;
- [x] Recebimento das mensagens pelo backend;
- [x] Armazenamento das leituras no PostgreSQL;
- [x] API para consulta do histórico;
- [x] Dashboard em tempo real;
- [x] Gráfico com dados históricos.

---

## 📌 Pendências

- [ ] Realizar a calibração final da velocidade do anemômetro;
- [ ] Validar a referência Norte e a calibração da biruta;
- [ ] Definir o intervalo definitivo de armazenamento das leituras;
- [ ] Realizar teste integrado prolongado;
- [ ] Melhorar o tratamento de reconexão do Wi-Fi e MQTT;
- [ ] Finalizar o case e a montagem física;
- [ ] Finalizar e validar a PCB;
- [ ] Integrar a branch de desenvolvimento após a revisão final.

---

## 👩‍💻 Desenvolvimento

Projeto desenvolvido no **Vortex Lab — Universidade de Fortaleza (UNIFOR)** como parte do Programa de Estágio em IoT.
