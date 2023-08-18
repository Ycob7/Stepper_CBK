#include "print.hpp"
#include <Arduino.h>
#include <LiquidCrystal.h>

// WPISYWANIE POZYCJI(OBECNEJ I ZADANEJ) NA SERIAL PORT
float getValidPosition(const char *prompt)
{
  float position;
  do
  {
    while (Serial.available() > 0)
      Serial.read();

    Serial.print("Jaka ");
    Serial.print(prompt);
    Serial.println(" pozycja ???");
    while (!Serial.available())
      position = Serial.parseFloat();

  } while (position < 0 || position > 500);

  return position;
}



//USTAWIENIE POCZĄTKOWEGO STANU LCD
LiquidCrystal lcd(12, 11, 6, 5, 4, 3);

void lcd_attach()
{
    lcd.begin(20, 4);
    lcd.setCursor(0, 0);
    lcd.print("Obecna pozcyja: ");
    lcd.setCursor(0, 2);
    lcd.print("Zadana pozycja: ");
    delay(50);
} 
    

//ZAMIANA WARTOSCI I WYPISANIE NA LCD I SERIAL
  void displayPositionInfo(int obecna_pos, int zadana_pos) 
  {
    int zmiana = zadana_pos - obecna_pos;
    double odl = zmiana * 2 / 3 * 100;          // bierzemy ponziej w pozycji "2 * odl"* bo półkrok
    long int czekanie = (abs(odl)) * 4 + 5000;

    Serial.print("TWOJA OBECNA POZYCJA: ");
    Serial.print(obecna_pos);
    Serial.println(" mm");

    lcd.setCursor(0, 1);
    lcd.print(obecna_pos);

    Serial.print("TWOJA ZADANA POZYCJA: ");
    Serial.print(zadana_pos);
    Serial.println(" mm");

    lcd.setCursor(0, 3);
    lcd.print(zadana_pos);

    Serial.print("ZMIANA POZYCJI O: ");
    Serial.print(zmiana);
    Serial.println(" mm");

    Serial.print("ZMIANA ODL o : ");
    Serial.print(odl);
    Serial.println(" krokow");

    delay(500);
  }
