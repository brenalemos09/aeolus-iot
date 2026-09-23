#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "display/lcd.h"

#define SDA_LCD 13
#define SCL_LCD 14

LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço, 16 colunas e 2 linhas

void iniciarLCD(){

    Wire.begin(SDA_LCD, SCL_LCD); // SDA = 13 e SCL = 14

    lcd.init(); // inicia o LCD
    lcd.backlight(); // ativa a luz de fundo
    lcd.clear(); // limpa o display
}

void mostrarDadosLCD(float temperatura, float umidade){

    lcd.setCursor(0, 0); // primeira linha
    lcd.print("Temp: ");
    lcd.print(temperatura);
    lcd.print("C");

    lcd.setCursor(0, 1); // segunda linha
    lcd.print("Umid: ");
    lcd.print(umidade);
    lcd.print("%");
}

void limparLCD(){

    lcd.clear();

} 