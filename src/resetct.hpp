#ifndef RESET_H
#define RESET_H

extern volatile bool limitSwitch1Touched;
extern volatile bool limitSwitch2Touched;

/// @brief wait for movement to finish, if limit switch is touched the operation will instantly stop.
/// @param ms delay in miliseconds
/// @param tic serial tic object
void delayWhileResettingCommandTimeout(uint32_t ms, TicSerial tic);

#endif /* RESET_H */