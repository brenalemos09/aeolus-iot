# 🌬️ Desafio Aeolus — Estação Meteorológica IoT

Projeto desenvolvido durante o Programa de Estágio em IoT do **Vortex Lab — UNIFOR**.

O **Desafio Aeolus** consiste no desenvolvimento de uma estação meteorológica inteligente capaz de coletar, processar, armazenar e exibir dados meteorológicos.

O projeto é inspirado em **Aeolus (Éolo)**, figura da mitologia grega associada aos ventos, e reúne conceitos de IoT, sistemas embarcados, eletrônica, MQTT, banco de dados, desenvolvimento web, fabricação digital e PCB.

> 🚧 Projeto em fase final de montagem física e testes integrados.

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

- ESP32 DevKit V4;
- Sensor DHT22 (temperatura e umidade);
- Sensor de luminosidade LDR (módulo com saída `DO`);
- Display LCD 16x2 com interface I²C (endereço `0x27`);
- LED RGB (common-anode, lógica invertida);
- Encoder rotativo KY-040 do anemômetro (velocidade);
- Encoder rotativo KY-040 da biruta (direção);
- Botão físico de calibração do Norte;
- Anemômetro e biruta fabricados por impressão 3D;
- Caixa "O Odre de Aeolus" impressa em 3D;
- PCB artesanal;
- Protoboard (prototipagem) e componentes eletrônicos.

### Firmware

- C/C++;
- Framework Arduino;
- PlatformIO;
- Wi-Fi;
- MQTT;
- JSON;
- PubSubClient;
- LiquidCrystal_I2C;
- DHT sensor library.

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
- KiCad (esquema e layout da PCB);
- Fabricação e modelagem 3D;
- Desenvolvimento de PCB.

---

## 🔌 Pinagem consolidada

Placa utilizada: **ESP32 DevKit V4**.

| Componente | Função | GPIO | Observação |
|---|---|---:|---|
| DHT22 | DATA | 4 | VCC em 3.3 V |
| LDR (módulo) | DO | 35 | VCC em 3.3 V |
| LED RGB | Vermelho | 26 | common-anode: `LOW` acende |
| LED RGB | Verde | 25 | common-anode: `LOW` acende |
| LED RGB | Azul | 33 | common-anode: `LOW` acende |
| LCD I²C | SDA | 13 | VCC em 5 V, endereço `0x27` |
| LCD I²C | SCL | 14 | `Wire.begin(13, 14)` |
| Encoder do anemômetro | CLK | 21 | `INPUT_PULLUP`; DT e SW não são usados |
| Encoder da biruta | CLK | 16 | VCC em 3.3 V |
| Encoder da biruta | DT | 17 | |
| Encoder da biruta | SW | 19 | ligado, sem função definida |
| Botão de calibração | Sinal | 23 | `INPUT_PULLUP`, outra perna no GND |

> Os GPIOs 13 e 14 (LCD) e 21 (anemômetro) foram escolhidos para facilitar o roteamento da PCB. O GPIO 12 foi evitado de propósito por ser um pino relacionado ao boot do ESP32.

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

> Durante os testes do protótipo, foram utilizados valores reduzidos para facilitar a validação manual do LED RGB. Os limites oficiais devem ser aplicados após a calibração final do anemômetro.

### Templo de Aeolus

O dashboard da estação é denominado **Templo de Aeolus** e apresenta:

- Leituras recebidas em tempo real via MQTT;
- Temperatura;
- Umidade;
- Luminosidade;
- Velocidade do vento;
- Direção e ângulo do vento (com bússola);
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

Os cartões do dashboard recebem os dados diretamente pelo MQTT (tempo real). O gráfico consulta a API, que lê o histórico do PostgreSQL, e se atualiza automaticamente a cada 5 segundos.

---

## 🧠 Como cada medição funciona

- **Temperatura e umidade:** o DHT22 envia os dados por um único fio. A leitura inválida (`NaN`) é tratada e sinalizada no campo `statusDHT`.
- **Luminosidade:** o LDR altera a tensão no pino analógico. O ESP32 converte essa tensão em um valor de 0 a 4095. Na montagem atual, menos luz resulta em valor maior.
- **Velocidade do vento:** o encoder do anemômetro gera pulsos a cada giro. Uma interrupção conta os pulsos (com `INPUT_PULLUP` e filtro de 5 ms contra ruído) e a velocidade é calculada pelo tempo real decorrido entre eles.
- **Direção do vento:** o encoder da biruta tem 20 passos por volta (18° cada). Com a biruta apontada para o Norte real, o botão de calibração salva a posição atual como referência (Norte = 0°). A partir daí, o ângulo é calculado de forma relativa e convertido em uma das 8 direções cardeais.
- **LCD e RGB:** mostram os valores localmente. O RGB indica o nível da Escala Aeolus pela velocidade do vento.

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

Se o Wi-Fi não conectar dentro do tempo limite de tentativas, a estação continua funcionando localmente (sensores, LCD e RGB).

---

## 🗄️ Banco de dados e API

O backend assina o tópico MQTT, valida os campos recebidos e grava cada leitura no PostgreSQL, na tabela `leituras_aeolus`:

| Campo | Tipo |
|---|---|
| `id` | BIGINT (chave primária, automático) |
| `temperatura` | NUMERIC(5,2) |
| `umidade` | NUMERIC(5,2) |
| `luminosidade` | INTEGER |
| `velocidade` | NUMERIC(6,2) |
| `angulo` | INTEGER |
| `direcao` | VARCHAR(20) |
| `status_dht` | BOOLEAN |
| `criado_em` | TIMESTAMPTZ (automático) |

Os campos `statusWiFi` e `statusMQTT` não são gravados: uma mensagem só chega ao backend se essas conexões estiverem funcionando.

### Endpoints

#### Estado do backend e banco

```http
GET /api/status
```

#### Histórico das leituras

```http
GET /api/leituras
```

O endpoint de histórico retorna as últimas leituras registradas (no máximo 100), organizadas em ordem cronológica para utilização no gráfico do dashboard.

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

Cada módulo do firmware segue o padrão `.h` (declaração) + `.cpp` (implementação). O `main.cpp` apenas coordena os módulos.

---

## 🔐 Arquivos privados (não versionados)

Dois arquivos de configuração ficam fora do GitHub e precisam ser criados em cada computador:

**`include/credenciais.h`** — rede Wi-Fi do ESP32:

```cpp
#ifndef CREDENCIAIS_H
#define CREDENCIAIS_H

#define WIFI_SSID "nome_da_rede"
#define WIFI_SENHA "senha_da_rede"

#endif
```

**`backend/.env`** — configurações do PostgreSQL, porta da API, broker e tópico MQTT.

Ambos estão no `.gitignore`, assim como a pasta `backend/node_modules/`.

---

## ▶️ Execução

### Firmware do ESP32

Crie o `include/credenciais.h`, abra o projeto no PlatformIO e execute:

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

Crie o arquivo `.env` e inicie o servidor:

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

## 🧪 Testes de hardware

Cada componente foi validado individualmente antes de entrar no firmware principal. Os testes ficam na pasta `testes-hardware/`, numerados na ordem em que foram feitos:

1. comunicação serial do ESP32;
2. DHT22, LDR, LED RGB e LCD I²C;
3. encoder da biruta: giro, botão, posição, calibração do Norte e direção cardinal;
4. encoder do anemômetro: contagem de pulsos com filtro de ruído;
5. integração dos dois encoders;
6. Wi-Fi e integração dos componentes com Wi-Fi e MQTT.

Metodologia: componente → código simples de teste → Serial Monitor → teste físico → registro do resultado → só então integração ao firmware.

---

## 🌿 Fluxo de versionamento

- `main`: versão estável;
- `develop`: integração do desenvolvimento;
- `feature/...`: uma branch por funcionalidade, integrada à `develop` após validação.

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
- [x] Gráfico com dados históricos;
- [x] PCB artesanal fabricada e componentes soldados.

---

## 📌 Pendências

- [ ] Realizar a calibração final da velocidade do anemômetro;
- [ ] Validar a referência Norte e a calibração da biruta;
- [ ] Definir o intervalo definitivo de armazenamento das leituras;
- [ ] Realizar teste integrado prolongado;
- [ ] Melhorar o tratamento de reconexão do Wi-Fi e MQTT;
- [ ] Testar todos os componentes na PCB já instalada dentro da caixa;
- [ ] Finalizar a impressão da base da caixa, do anemômetro e da biruta;
- [ ] Integrar a branch de desenvolvimento à `develop` após a revisão final.

---

## 📚 Desafios e aprendizados

- **Brownout no Wi-Fi:** três placas ESP32 DevKit V1 reiniciavam (`Brownout detector was triggered`) ao iniciar o Wi-Fi, mesmo trocando cabo, porta USB e fonte. O regulador de tensão dessas placas não suportava o pico de corrente do rádio. A troca para a DevKit V4 resolveu. Os GPIOs usados são os mesmos nas duas placas.
- **Ruído mecânico nos encoders (bounce):** pulsos falsos inflavam a velocidade calculada. A solução foi um filtro por tempo mínimo entre pulsos, calibrado na prática, junto com `INPUT_PULLUP`.
- **Escala de teste × vento real:** com giro manual a velocidade ficou baixa demais para ver todas as cores do RGB, então os limites foram reduzidos temporariamente nos testes.
- **PCB artesanal:** foram necessárias quatro tentativas, com problemas em etapas diferentes (desenho das trilhas, corte, corrosão, furação e solda). A quarta placa funcionou.
- **Impressão 3D:** mais de dez impressões falharam, e uma das impressoras entupiu durante o processo.
- **Segurança de credenciais:** senha do Wi-Fi e configurações do banco ficam em arquivos fora do Git (`credenciais.h` e `.env`).

---

## 👩‍💻 Desenvolvimento

Projeto desenvolvido no **Vortex Lab — Universidade de Fortaleza (UNIFOR)** como parte do Programa de Estágio em IoT.
