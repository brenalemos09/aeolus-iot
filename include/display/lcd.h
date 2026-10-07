#ifndef LCD_H
#define LCD_H

void iniciarLCD();
void limparLCD();

void ligarLCD();
void desligarLCD();
void alternarEstadoLCD();

void mudarPaginaLCD();
void atualizarBotoesLCD();
void atualizarTelaLCD();

void mostrarAberturaAeolus();

void mostrarDadosLCD(
    float temperatura,
    float umidade,
    int luminosidade,
    float velocidade,
    const char* direcao
);

void mostrarDadosLCD(
    float temperatura,
    float umidade
);

#endif