#ifndef PRINT_H
#define PRINT_H


void lcd_attach();
float getValidPosition(const char *prompt);
void displayPositionInfo(int obecna_pos, int zadana_pos);


#endif /* PRINT_H */