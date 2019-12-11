#ifndef SC1602DRIVER_H_
#define SC1602DRIVER_H_

void Initialize_SC1602();
void ClearDisplay_SC1602();
void ReturnHome_SC1602();
void WriteString_SC1602(char* string, int line);

#endif