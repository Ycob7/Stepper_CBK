#include "isr.hpp"
#include <Arduino.h>

volatile bool limitSwitch1Touched = false;
volatile bool limitSwitch2Touched = false;

void limitSwitch1ISR()
{
    limitSwitch1Touched = true;
    detachInterrupt(digitalPinToInterrupt(2));
}

void limitSwitch2ISR()
{
    limitSwitch2Touched = true;
    detachInterrupt(digitalPinToInterrupt(3));
}
