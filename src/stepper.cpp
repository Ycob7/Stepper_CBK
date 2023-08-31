#include <Tic.h>
#include <ezButton.h>

#include "isr.hpp"     // ISR - Przerwania
#include "print.hpp"   // SERIAL AND LCD - Wypisywanie
#include "resetct.hpp" // Reset Command Timeout

#ifdef SERIAL_PORT_HARDWARE_OPEN
#define ticSerial SERIAL_PORT_HARDWARE_OPEN
#else
#include <SoftwareSerial.h>
SoftwareSerial ticSerial(9, 10);
#endif

TicSerial tic(ticSerial);
ezButton kranc_1(2);
ezButton kranc_2(3);

void setup()
{
  Serial.begin(9600);
  ticSerial.begin(9600);
  delay(20);
  tic.haltAndSetPosition(0);
  delay(300);

  // Stepper declaration
  tic.setProduct(TicProduct::Tic36v4);
  tic.setStepMode(TicStepMode::Microstep1);
  tic.setStartingSpeed(1000000); // 100 steps/second
  tic.setDecayMode(TicDecayMode::Mixed);
  tic.setMaxAccel(10000);   // 100 steps/second mozna 50 dac ale idzie wolniej
  tic.setMaxSpeed(5000000); // 500 steps/second

  delay(300);
  tic.energize();
  tic.exitSafeStart();
  lcd_attach();
}

int previousLimitSwitchState1 = HIGH;
int previousLimitSwitchState2 = HIGH;

void loop()
{

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  attachInterrupt(digitalPinToInterrupt(2), limitSwitch1ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(3), limitSwitch2ISR, RISING);
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  kranc_1.loop();
  kranc_2.loop();

  int limitSwitchState1 = kranc_1.getState();
  int limitSwitchState2 = kranc_2.getState();

  double obecna_pos = 0;
  double zadana_pos = 0;
  double zmiana = 0;

  // WYPISANIE NA SERIAL MONITOR
  obecna_pos = getValidPosition("Obecna");
  Serial.println(obecna_pos);
  zadana_pos = getValidPosition("Zadana");
  Serial.println(zadana_pos);

  // ZAMIANA POZYCJI NA ODLEGŁOŚĆI
  zmiana = zadana_pos - obecna_pos;
  double odl = zmiana * 2 / 3 * 100; //*2 bo mikrokroki
  long int czekanie = (abs(odl)) * 4 + 5000; // 5000 *2 jesli zmniejszymy przyspieszenie

  // WYPISANIIE NA LCD I SERIAL MONITOR
  displayPositionInfo(obecna_pos, zadana_pos);

  // PRZEJAZD W LEWO
  if (odl < 0)
  {
    if (limitSwitchState1 == LOW && previousLimitSwitchState1 == HIGH)
    {
      Serial.println("LEWO <---");
      limitSwitch1Touched = false;
      limitSwitch2Touched = false;
      tic.haltAndHold();
      tic.exitSafeStart();
      tic.setTargetPosition(2 * odl);
      delayWhileResettingCommandTimeout(czekanie, tic);
      delay(300);
      tic.haltAndSetPosition(0);
      tic.exitSafeStart();
      limitSwitch1Touched = false;
    }
  }

  // PRZEJAZD W PRAWO
  else if (odl > 0)
  {
    if (limitSwitchState2 == LOW && previousLimitSwitchState2 == HIGH)
    {
      Serial.println("PRAWO --->");
      limitSwitch1Touched = false;
      limitSwitch2Touched = false;
      tic.haltAndHold();
      tic.exitSafeStart();
      tic.setTargetPosition(2 * odl);
      delayWhileResettingCommandTimeout(czekanie, tic);
      delay(300);
      tic.haltAndSetPosition(0);
      tic.exitSafeStart();
      limitSwitch2Touched = false;
    }
  }

  limitSwitch1Touched = (limitSwitchState1 == HIGH && previousLimitSwitchState1 == HIGH);
  limitSwitch2Touched = (limitSwitchState2 == HIGH && previousLimitSwitchState2 == HIGH);

  // JEŚLI DOTYKA KRANC_1 TO PRZEJAZD W LEWO
  if (limitSwitch1Touched == true)
  {
    Serial.println("Powinien isc w LEWO <---");
    limitSwitch1Touched = false;
    tic.haltAndHold();
    tic.exitSafeStart();
    tic.setTargetPosition(2 * odl);
    delayWhileResettingCommandTimeout(czekanie, tic);
    delay(300);
    tic.haltAndSetPosition(0);
    tic.exitSafeStart();
    limitSwitch1Touched = false;
  }

  // JEŚLI DOTYKA KRANC_2 TO PRZEJAZD W PRAWO
  if (limitSwitch2Touched == true)
  {
    Serial.println("Powinien isc w PRAWO --->");
    limitSwitch2Touched = false;
    tic.haltAndHold();
    tic.exitSafeStart();
    tic.setTargetPosition(2 * odl);
    delayWhileResettingCommandTimeout(czekanie, tic);
    delay(300);
    tic.haltAndSetPosition(0);
    tic.exitSafeStart();
    limitSwitch2Touched = false;
  }

  limitSwitch1Touched = false;
  limitSwitch2Touched = false;
  tic.exitSafeStart();
  tic.energize();
  tic.resetCommandTimeout();
  delay(300);
}