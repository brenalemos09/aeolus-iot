#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <time.h>

#include "display/lcd.h"

namespace {
constexpr int SDA_LCD = 13;
constexpr int SCL_LCD = 14;

constexpr int BOTAO_TELA = 32;
constexpr int BOTAO_PAGINA = 23;

constexpr unsigned long DURACAO_RAJADA = 1100;
constexpr unsigned long DURACAO_NOME = 1200;
constexpr unsigned long DEBOUNCE_MS = 50;

constexpr int ESPACO_LETRAS = 1;
constexpr int QUANTIDADE_PAGINAS = 3;

LiquidCrystal_I2C lcd(0x27, 16, 2);

byte VENTO_1[8] = {
    0b10001, 0b00000, 0b00100, 0b00000,
    0b10001, 0b00000, 0b00100, 0b00000
};

byte VENTO_2[8] = {
    0b10101, 0b01010, 0b10101, 0b01010,
    0b10101, 0b01010, 0b10101, 0b01010
};

byte VENTO_3[8] = {
    0b01110, 0b11111, 0b11011, 0b11111,
    0b01110, 0b11111, 0b11011, 0b11111
};

byte VENTO_4[8] = {
    0b11111, 0b11111, 0b11111, 0b11111,
    0b11111, 0b11111, 0b11111, 0b11111
};

byte O_TOPO[8] = {
    0b01110, 0b11111, 0b11011, 0b11011,
    0b11011, 0b11011, 0b11011, 0b11011
};

byte O_BASE[8] = {
    0b11011, 0b11011, 0b11011, 0b11011,
    0b11011, 0b11011, 0b11111, 0b01110
};

byte U_TOPO[8] = {
    0b11011, 0b11011, 0b11011, 0b11011,
    0b11011, 0b11011, 0b11011, 0b11011
};

byte L_TOPO[8] = {
    0b11000, 0b11000, 0b11000, 0b11000,
    0b11000, 0b11000, 0b11000, 0b11000
};

byte L_BASE[8] = {
    0b11000, 0b11000, 0b11000, 0b11000,
    0b11000, 0b11000, 0b11111, 0b11111
};

byte A_BASE[8] = {
    0b11111, 0b11111, 0b11011, 0b11011,
    0b11011, 0b11011, 0b11011, 0b11011
};

byte E_TOPO[8] = {
    0b11111, 0b11111, 0b11000, 0b11000,
    0b11000, 0b11000, 0b11111, 0b11111
};

byte S_BASE[8] = {
    0b00011, 0b00011, 0b00011, 0b00011,
    0b00011, 0b00011, 0b11111, 0b11111
};

enum {
    ID_O_TOPO,
    ID_O_BASE,
    ID_U_TOPO,
    ID_L_TOPO,
    ID_L_BASE,
    ID_A_BASE,
    ID_E_TOPO,
    ID_S_BASE
};

const uint8_t LETRAS_AEOLUS[6][2] = {
    {ID_O_TOPO, ID_A_BASE},
    {ID_E_TOPO, ID_L_BASE},
    {ID_O_TOPO, ID_O_BASE},
    {ID_L_TOPO, ID_L_BASE},
    {ID_U_TOPO, ID_O_BASE},
    {ID_E_TOPO, ID_S_BASE}
};

uint8_t telaAtual[2][16];
uint8_t quadro[2][16];
uint8_t nomeAeolus[2][16];

bool lcdLigado = true;
int paginaAtual = 0;

float temperaturaLCD = 0;
float umidadeLCD = 0;
int luminosidadeLCD = 0;
float velocidadeLCD = 0;
char direcaoLCD[17] = "Norte";

struct BotaoLCD {
    int pino;
    int ultimaLeitura;
    int estadoEstavel;
    unsigned long ultimaMudanca;
};

BotaoLCD botaoTela = {
    BOTAO_TELA, HIGH, HIGH, 0
};

BotaoLCD botaoPagina = {
    BOTAO_PAGINA, HIGH, HIGH, 0
};

bool foiPressionado(BotaoLCD& botao) {
    unsigned long agora = millis();
    int leitura = digitalRead(botao.pino);

    if (leitura != botao.ultimaLeitura) {
        botao.ultimaLeitura = leitura;
        botao.ultimaMudanca = agora;
    }

    if (
        agora - botao.ultimaMudanca >= DEBOUNCE_MS &&
        leitura != botao.estadoEstavel
    ) {
        botao.estadoEstavel = leitura;
        return leitura == LOW;
    }

    return false;
}

void limparQuadro() {
    memset(quadro, ' ', sizeof(quadro));
}

void exibirQuadro() {
    int ultimaLinha = -1;
    int ultimaColuna = -2;

    for (int linha = 0; linha < 2; linha++) {
        for (int coluna = 0; coluna < 16; coluna++) {
            if (
                quadro[linha][coluna] ==
                telaAtual[linha][coluna]
            ) {
                continue;
            }

            if (
                ultimaLinha != linha ||
                ultimaColuna != coluna - 1
            ) {
                lcd.setCursor(coluna, linha);
            }

            lcd.write(quadro[linha][coluna]);

            telaAtual[linha][coluna] =
                quadro[linha][coluna];

            ultimaLinha = linha;
            ultimaColuna = coluna;
        }
    }
}

void escreverCentralizado(
    int linha,
    const char* texto
) {
    int tamanho = strlen(texto);
    int inicio = (16 - tamanho) / 2;

    if (inicio < 0) {
        inicio = 0;
    }

    for (
        int i = 0;
        i < tamanho && inicio + i < 16;
        i++
    ) {
        quadro[linha][inicio + i] = texto[i];
    }
}

void carregarLetrasAeolus() {
    byte* letras[8] = {
        O_TOPO,
        O_BASE,
        U_TOPO,
        L_TOPO,
        L_BASE,
        A_BASE,
        E_TOPO,
        S_BASE
    };

    for (int i = 0; i < 8; i++) {
        lcd.createChar(i, letras[i]);
    }
}

void montarNomeAeolus() {
    memset(nomeAeolus, ' ', sizeof(nomeAeolus));

    int largura = 6 + 5 * ESPACO_LETRAS;
    int colunaInicial = (16 - largura) / 2;

    for (int i = 0; i < 6; i++) {
        int coluna =
            colunaInicial + i * (1 + ESPACO_LETRAS);

        if (coluna >= 0 && coluna < 16) {
            nomeAeolus[0][coluna] =
                LETRAS_AEOLUS[i][0];

            nomeAeolus[1][coluna] =
                LETRAS_AEOLUS[i][1];
        }
    }
}

void efeitoRelampago() {
    lcd.noBacklight();
    delay(70);

    lcd.backlight();
    delay(50);

    lcd.noBacklight();
    delay(70);

    lcd.backlight();
}

void desenharPaginaVento() {
    limparQuadro();

    char texto[17];

    snprintf(
        texto,
        sizeof(texto),
        "Vento: %.1fkm/h",
        velocidadeLCD
    );

    escreverCentralizado(0, texto);
    escreverCentralizado(1, direcaoLCD);

    exibirQuadro();
}

void desenharPaginaAmbiente() {
    limparQuadro();

    char primeiraLinha[17];
    char segundaLinha[17];

    snprintf(
        primeiraLinha,
        sizeof(primeiraLinha),
        "T:%.1fC U:%.0f%%",
        temperaturaLCD,
        umidadeLCD
    );

    // Leitura bruta do LDR, sem conversao para lux.
    snprintf(
        segundaLinha,
        sizeof(segundaLinha),
        "LDR: %d",
        luminosidadeLCD
    );

    escreverCentralizado(0, primeiraLinha);
    escreverCentralizado(1, segundaLinha);

    exibirQuadro();
}

void desenharPaginaRelogio() {
    limparQuadro();

    // O wifi.cpp configura a sincronizacao NTP
    // e o fuso de Fortaleza (UTC-3).
    time_t agora = time(nullptr);
    struct tm horario = {};

    // Consulta imediata, sem esperar pela rede.
    if (
        localtime_r(&agora, &horario) == nullptr ||
        horario.tm_year < (2024 - 1900)
    ) {
        escreverCentralizado(
            0,
            "Hora indisponivel"
        );

        escreverCentralizado(
            1,
            "Aguardando NTP"
        );

        exibirQuadro();
        return;
    }

    char hora[17];
    char data[17];

    strftime(
        hora,
        sizeof(hora),
        "%H:%M:%S",
        &horario
    );

    strftime(
        data,
        sizeof(data),
        "%d/%m/%Y",
        &horario
    );

    escreverCentralizado(0, hora);
    escreverCentralizado(1, data);

    exibirQuadro();
}

} // namespace

void limparLCD() {
    lcd.clear();
    memset(telaAtual, ' ', sizeof(telaAtual));
}

void mostrarAberturaAeolus() {
    montarNomeAeolus();
    limparLCD();

    lcd.createChar(0, VENTO_1);
    lcd.createChar(1, VENTO_2);
    lcd.createChar(2, VENTO_3);
    lcd.createChar(3, VENTO_4);

    const uint8_t perfilVento[8] = {
        0, 1, 2, 3, 3, 2, 1, 0
    };

    unsigned long inicio = millis();

    while (millis() - inicio < DURACAO_RAJADA) {
        int cabeca =
            (millis() - inicio) * 28UL /
            DURACAO_RAJADA;

        limparQuadro();

        for (int linha = 0; linha < 2; linha++) {
            for (int coluna = 0; coluna < 16; coluna++) {
                int distancia =
                    cabeca - 3 * linha - coluna;

                if (
                    distancia >= 0 &&
                    distancia < 8
                ) {
                    quadro[linha][coluna] =
                        perfilVento[distancia];
                }
            }
        }

        exibirQuadro();
        delay(1);
    }

    limparLCD();
    lcd.noBacklight();

    carregarLetrasAeolus();

    memcpy(
        quadro,
        nomeAeolus,
        sizeof(quadro)
    );

    exibirQuadro();

    delay(150);
    lcd.backlight();
    delay(120);

    efeitoRelampago();
    delay(DURACAO_NOME);

    limparLCD();
}

void mudarPaginaLCD() {
    if (!lcdLigado) {
        return;
    }

    paginaAtual =
        (paginaAtual + 1) % QUANTIDADE_PAGINAS;

    limparLCD();
    atualizarTelaLCD();

    Serial.printf(
        "LCD: pagina %d de 3.\n",
        paginaAtual + 1
    );
}

void atualizarTelaLCD() {
    if (!lcdLigado) {
        return;
    }

    // A pagina muda somente pelo botao GPIO23.
    if (paginaAtual == 0) {
        desenharPaginaVento();
    } else if (paginaAtual == 1) {
        desenharPaginaAmbiente();
    } else {
        desenharPaginaRelogio();
    }
}

void ligarLCD() {
    lcdLigado = true;
    lcd.backlight();

    mostrarAberturaAeolus();

    paginaAtual = 0;
    atualizarTelaLCD();

    Serial.println("LCD ligado.");
}

void desligarLCD() {
    lcdLigado = false;

    limparLCD();
    lcd.noBacklight();

    Serial.println("LCD desligado.");
}

void alternarEstadoLCD() {
    if (lcdLigado) {
        desligarLCD();
    } else {
        ligarLCD();
    }
}

void atualizarBotoesLCD() {
    // Le ambos antes de executar a animacao.
    bool pressionouTela =
        foiPressionado(botaoTela);

    bool pressionouPagina =
        foiPressionado(botaoPagina);

    if (pressionouTela) {
        alternarEstadoLCD();
    } else if (pressionouPagina) {
        mudarPaginaLCD();
    }
}

void mostrarDadosLCD(
    float temperatura,
    float umidade,
    int luminosidade,
    float velocidade,
    const char* direcao
) {
    temperaturaLCD = temperatura;
    umidadeLCD = umidade;
    luminosidadeLCD = luminosidade;
    velocidadeLCD = velocidade;

    snprintf(
        direcaoLCD,
        sizeof(direcaoLCD),
        "%s",
        direcao != nullptr ? direcao : "--"
    );

    atualizarTelaLCD();
}

void mostrarDadosLCD(
    float temperatura,
    float umidade
) {
    temperaturaLCD = temperatura;
    umidadeLCD = umidade;

    atualizarTelaLCD();
}

void iniciarLCD() {
    Wire.begin(SDA_LCD, SCL_LCD);

    lcd.init();
    lcd.backlight();

    pinMode(BOTAO_TELA, INPUT_PULLUP);
    pinMode(BOTAO_PAGINA, INPUT_PULLUP);

    botaoTela.ultimaLeitura =
        digitalRead(BOTAO_TELA);

    botaoTela.estadoEstavel =
        botaoTela.ultimaLeitura;

    botaoTela.ultimaMudanca = millis();

    botaoPagina.ultimaLeitura =
        digitalRead(BOTAO_PAGINA);

    botaoPagina.estadoEstavel =
        botaoPagina.ultimaLeitura;

    botaoPagina.ultimaMudanca = millis();

    lcdLigado = true;
    paginaAtual = 0;

    mostrarAberturaAeolus();
    atualizarTelaLCD();

    Serial.println(
        "LCD: direita GPIO32 = liga/desliga."
    );

    Serial.println(
        "LCD: esquerda GPIO23 = passa pagina."
    );

    Serial.println(
        "LCD: relogio usa horario do sistema via NTP."
    );
}