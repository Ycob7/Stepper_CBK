#ifndef RESET_H
#define RESET_H

extern volatile bool limitSwitch1Touched;
extern volatile bool limitSwitch2Touched;

void delayWhileResettingCommandTimeout(uint32_t ms, TicSerial tic);

#endif /* RESET_H */