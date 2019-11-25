#ifndef SC1602DRIVER_H_
#define SC1602DRIVER_H_

#include "SC1602Typedef.h"

void Initialize_SC1602();
void ClearDisplay_SC1602();
void ReturnHome_SC1602();
void WriteString_SC1602(char* string, int line);

//To be defined by user
void writepin_DATA0(SC1602_PinState_t status);
void writepin_DATA1(SC1602_PinState_t status);
void writepin_DATA2(SC1602_PinState_t status);
void writepin_DATA3(SC1602_PinState_t status);
void writepin_DATA4(SC1602_PinState_t status);
void writepin_DATA5(SC1602_PinState_t status);
void writepin_DATA6(SC1602_PinState_t status);
void writepin_DATA7(SC1602_PinState_t status);
void writepin_RS(SC1602_PinState_t status);
void writepin_RW(SC1602_PinState_t status);
void writepin_E(SC1602_PinState_t status);
void wait_ms(int milisecond);
#endif