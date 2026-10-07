#include <Arduino.h>
#include <soc/gpio_reg.h>

#include "sensores/encoderBiruta.h"

namespace {
constexpr int CLK_BIRUTA = 21;
constexpr int DT_BIRUTA = 27;
constexpr int BOTAO_CALIBRACAO_BIRUTA = 22;

constexpr int PASSOS_POR_VOLTA_BIRUTA = 80;
constexpr int SENTIDO_BIRUTA = 1;

const int8_t TABELA_BIRUTA[16] = {
     0, -1,  1,  0,
     1,  0,  0, -1,
    -1,  0,  0,  1,
     0,  1, -1,  0
};

const char* DIRECOES_BIRUTA[8] = {
    "Norte",
    "Nordeste",
    "Leste",
    "Sudeste",
    "Sul",
    "Sudoeste",
    "Oeste",
    "Noroeste"
};

portMUX_TYPE birutaMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint8_t estadoAnteriorBiruta = 0;
volatile int8_t ultimoSentidoBiruta = 1;
volatile long posicaoBiruta = 0;

long referenciaNorteBiruta = 0;

int ultimaLeituraBotaoBiruta = HIGH;
int estadoEstavelBotaoBiruta = HIGH;
unsigned long ultimaMudancaBotaoBiruta = 0;

void IRAM_ATTR lerEncoderBiruta() {
    portENTER_CRITICAL_ISR(&birutaMux);

    // Leitura simultanea dos dois canais.
    uint32_t entradas = REG_READ(GPIO_IN_REG);

    uint8_t estadoAtual =
        (((entradas >> CLK_BIRUTA) & 1) << 1) |
        ((entradas >> DT_BIRUTA) & 1);

    if (estadoAtual != estadoAnteriorBiruta) {
        int8_t deslocamento = TABELA_BIRUTA[
            (estadoAnteriorBiruta << 2) | estadoAtual
        ];

        // Preserva a compensacao da Marianna.
        if (deslocamento == 0) {
            deslocamento = 2 * ultimoSentidoBiruta;
        } else {
            ultimoSentidoBiruta = deslocamento;
        }

        posicaoBiruta += deslocamento;
        estadoAnteriorBiruta = estadoAtual;
    }

    portEXIT_CRITICAL_ISR(&birutaMux);
}

long lerPosicaoAtualBiruta() {
    portENTER_CRITICAL(&birutaMux);
    long copia = posicaoBiruta;
    portEXIT_CRITICAL(&birutaMux);

    return copia;
}

long obterPosicaoRelativaBiruta() {
    long posicaoAtual = lerPosicaoAtualBiruta();

    long relativa =
        ((posicaoAtual - referenciaNorteBiruta) *
         SENTIDO_BIRUTA) % PASSOS_POR_VOLTA_BIRUTA;

    if (relativa < 0) {
        relativa += PASSOS_POR_VOLTA_BIRUTA;
    }

    return relativa;
}
} // namespace

void calibrarBiruta() {
    referenciaNorteBiruta = lerPosicaoAtualBiruta();

    Serial.println("Biruta: posicao atual definida como Norte.");
}

void iniciarEncoderBiruta() {
    pinMode(CLK_BIRUTA, INPUT_PULLUP);
    pinMode(DT_BIRUTA, INPUT_PULLUP);
    pinMode(BOTAO_CALIBRACAO_BIRUTA, INPUT_PULLUP);

    uint32_t entradas = REG_READ(GPIO_IN_REG);

    estadoAnteriorBiruta =
        (((entradas >> CLK_BIRUTA) & 1) << 1) |
        ((entradas >> DT_BIRUTA) & 1);

    ultimaLeituraBotaoBiruta =
        digitalRead(BOTAO_CALIBRACAO_BIRUTA);

    estadoEstavelBotaoBiruta = HIGH;
    ultimaMudancaBotaoBiruta = millis();

    attachInterrupt(
        digitalPinToInterrupt(CLK_BIRUTA),
        lerEncoderBiruta,
        CHANGE
    );

    attachInterrupt(
        digitalPinToInterrupt(DT_BIRUTA),
        lerEncoderBiruta,
        CHANGE
    );

    // Mantem a referencia inicial do modulo anterior.
    calibrarBiruta();

    Serial.println("Biruta iniciada: CLK21 | DT27 | SW22.");
    Serial.println("Aponte para o Norte e pressione/solte SW22.");
}

void atualizarEncoderBiruta() {
    unsigned long agora = millis();

    int leituraAtual =
        digitalRead(BOTAO_CALIBRACAO_BIRUTA);

    // Debounce somente do botao.
    if (leituraAtual != ultimaLeituraBotaoBiruta) {
        ultimaLeituraBotaoBiruta = leituraAtual;
        ultimaMudancaBotaoBiruta = agora;
    }

    if (
        agora - ultimaMudancaBotaoBiruta >= 50 &&
        leituraAtual != estadoEstavelBotaoBiruta
    ) {
        estadoEstavelBotaoBiruta = leituraAtual;

        if (estadoEstavelBotaoBiruta == LOW) {
            calibrarBiruta();
        }
    }
}

int obterPosicaoBiruta() {
    return static_cast<int>(obterPosicaoRelativaBiruta());
}

int obterAnguloBiruta() {
    long relativa = obterPosicaoRelativaBiruta();

    return (relativa * 360) / PASSOS_POR_VOLTA_BIRUTA;
}

int obterSetorBiruta() {
    long relativa = obterPosicaoRelativaBiruta();

    return ((relativa + 5) / 10) % 8;
}

const char* obterDirecaoBiruta() {
    return DIRECOES_BIRUTA[obterSetorBiruta()];
}