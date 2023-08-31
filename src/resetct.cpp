#include <Arduino.h>
#include <Tic.h>
#include "isr.hpp"
#include "resetct.hpp"

// #ifdef SERIAL_PORT_HARDWARE_OPEN
// #define ticSerial SERIAL_PORT_HARDWARE_OPEN
// #else
// #include <SoftwareSerial.h>
// SoftwareSerial ticSerial(9, 10);
// #endif

void delayWhileResettingCommandTimeout(uint32_t ms, TicSerial tic) // Ruch nie będzie przerywany przez "bledy"
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
        tic.resetCommandTimeout();
    } while ((uint32_t)(millis() - start) <= ms);

    if (digitalRead(2) == 0)
    {
        delay(100);
        attachInterrupt(digitalPinToInterrupt(2), limitSwitch1ISR, RISING);
        limitSwitch1Touched = false;
    }
    if (digitalRead(3) == 0)
    {
        delay(100);
        attachInterrupt(digitalPinToInterrupt(3), limitSwitch2ISR, RISING);
        limitSwitch2Touched = false;
    }
}