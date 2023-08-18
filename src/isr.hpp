#ifndef ISR_H
#define ISR_H

extern volatile bool limitSwitch1Touched;
extern volatile bool limitSwitch2Touched;

/// @brief Include Stopping motor - interrupt
void limitSwitch1ISR();
void limitSwitch2ISR();

#endif /* ISR_H */