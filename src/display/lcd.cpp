#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "display/lcd.h"

LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço , (16 colunas, 2 linhas)

void iniciarLCD(){
    lcd.init(); // inicia
    lcd.backlight(); // ativa a luz de fundo do LCD
    lcd.clear(); // limpa o display
}

void mostrarDadosLCD(float temperatura, float umidade){
    lcd.setCursor(0, 0); // define o cursor na primeira linha
    lcd.print("Temp: ");
    lcd.print(temperatura);
    lcd.print("C");

    lcd.setCursor(0, 1); // define o cursor na segunda linha
    lcd.print("Umid: ");
    lcd.print(umidade);
    lcd.print("%");
}

void limparLCD(){
    lcd.clear(); 
}