#include <Tic.h>
#include <LiquidCrystal.h>
#include <ezButton.h>

#ifdef SERIAL_PORT_HARDWARE_OPEN
#define ticSerial SERIAL_PORT_HARDWARE_OPEN
#else
#include <SoftwareSerial.h>
SoftwareSerial ticSerial(10, 11);
#endif

TicSerial tic(ticSerial);
LiquidCrystal lcd(12, 11, 6, 5, 4, 3);
ezButton kranc_1(2);
ezButton kranc_2(7);

// ISR DECLARATION

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
volatile bool limitSwitch1Touched = false;
volatile bool limitSwitch2Touched = false;
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void limitSwitch1ISR()
{
  limitSwitch1Touched = true;
  detachInterrupt(digitalPinToInterrupt(2));
}

void limitSwitch2ISR()
{
  limitSwitch2Touched = true;
  detachInterrupt(digitalPinToInterrupt(7));
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void setup()
{
  Serial.begin(9600);
  ticSerial.begin(9600);
  delay(20);
  tic.haltAndSetPosition(0);
  delay(300);

  tic.setProduct(TicProduct::Tic36v4);
  tic.setStepMode(TicStepMode::Microstep1);
  tic.setStartingSpeed(1000000);
  tic.setDecayMode(TicDecayMode::Mixed);
  tic.setMaxAccel(10000);
  tic.setMaxSpeed(5000000);

  delay(300);
  tic.energize();
  tic.exitSafeStart();
  lcd.begin(20, 4);
  lcd.setCursor(0, 0);
  lcd.print("Obecna pozcyja: ");
  lcd.setCursor(0, 2);
  lcd.print("Zadana pozycja: ");
  delay(50);
}

void resetCommandTimeout() // Wywoływac 1 raz/s albo będzie error
{
  tic.resetCommandTimeout();
}

void delayWhileResettingCommandTimeout(uint32_t ms) // Ruch nie będzie przerywany przez "bledy"
{
  uint32_t start = millis();
  do
  {
    if (limitSwitch1Touched == true || limitSwitch2Touched == true)
    {
      Serial.println("Zmiana wartości kranc");
      Serial.println("ZADAJ DOBRA POZYCJE");
      tic.haltAndSetPosition(0);
      tic.exitSafeStart();
      break;
    }
    resetCommandTimeout();
  } while ((uint32_t)(millis() - start) <= ms);

  if (digitalRead(2) == 0)
  {
    delay(100);
    attachInterrupt(digitalPinToInterrupt(2), limitSwitch1ISR, RISING);
    limitSwitch1Touched = false;
  }
  if (digitalRead(7) == 0)
  {
    delay(100);
    attachInterrupt(digitalPinToInterrupt(7), limitSwitch2ISR, RISING);
    limitSwitch2Touched = false;
  }
}

// void waitForPosition(int32_t targetPosition)// Czekanie na osiagniecie zamierzonej pozycji
//{
//   do
//   {
//     resetCommandTimeout();
//   } while (tic.getCurrentPosition() != targetPosition);
// }

int previousLimitSwitchState1 = HIGH;
int previousLimitSwitchState2 = HIGH;

// WPISYWANIE POZYCJI
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
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void loop()
{

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  attachInterrupt(digitalPinToInterrupt(2), limitSwitch1ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(7), limitSwitch2ISR, RISING);
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

  // ZAMIANA POZYCJI NA ODLEGŁOŚĆI POKAZANIE NA LCD
  zmiana = zadana_pos - obecna_pos;
  double odl = zmiana * 2 / 3 * 100; //*2 bo mikrokroki
  long int czekanie = (abs(odl)) * 4 + 5000;

  // PŁYTKA LCD
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

  // PRZEJAZD NA DANA POZYCJE
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
      delayWhileResettingCommandTimeout(czekanie);
      delay(100);
      tic.haltAndSetPosition(0);
      tic.exitSafeStart();
      limitSwitch1Touched = false;
    }
  }

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
      delayWhileResettingCommandTimeout(czekanie);
      delay(100);
      tic.haltAndSetPosition(0);
      tic.exitSafeStart();
      limitSwitch2Touched = false;
    }
  }

  limitSwitch1Touched = (limitSwitchState1 == HIGH && previousLimitSwitchState1 == HIGH);
  limitSwitch2Touched = (limitSwitchState2 == HIGH && previousLimitSwitchState2 == HIGH);

  if (limitSwitch1Touched == true)
  {
    Serial.println("Powinien isc w LEWO <---");
    limitSwitch1Touched = false;
    tic.haltAndHold();
    tic.exitSafeStart();
    tic.setTargetPosition(2 * odl);
    delayWhileResettingCommandTimeout(czekanie);
    delay(100);
    tic.haltAndSetPosition(0);
    tic.exitSafeStart();
    limitSwitch1Touched = false;
  }

  if (limitSwitch2Touched == true)
  {
    Serial.println("Powinien isc w PRAWO --->");
    limitSwitch2Touched = false;
    tic.haltAndHold();
    tic.exitSafeStart();
    tic.setTargetPosition(2 * odl);
    delayWhileResettingCommandTimeout(czekanie);
    delay(100);
    tic.haltAndSetPosition(0);
    tic.exitSafeStart();
    limitSwitch2Touched = false;
  }

  limitSwitch1Touched = false;
  limitSwitch2Touched = false;
  tic.exitSafeStart();
  tic.energize();
  tic.resetCommandTimeout();
  delay(100);
}
