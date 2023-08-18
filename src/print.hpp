#ifndef PRINT_H
#define PRINT_H

/// @brief Add  LCD and sets where should be print ("Obecna pozycja / Zadan pozycja")
void lcd_attach();

/// @brief Serial asks about 2 positions, current and target
/// @param prompt put here value in milimeters
/// @return position where is and where sholud go the stepper motor
float getValidPosition(const char *prompt);

/// @brief printing on LCD screen and Serial monitor parameters as : distance, steps, change
/// @param obecna_pos - current position
/// @param zadana_pos - target position
void displayPositionInfo(int obecna_pos, int zadana_pos);


#endif /* PRINT_H */