#include "CageDriver_HAL.h"

__attribute__ ((weak)) DigitalPinState_t readSettingEntrySwitch(){ return DIGITAL_PIN_LOW; }
__attribute__ ((weak)) DigitalPinState_t readUVControlSwitch(){ return DIGITAL_PIN_LOW; }
__attribute__ ((weak)) DigitalPinState_t readPushButton_SettingUp(){ return DIGITAL_PIN_LOW; }
__attribute__ ((weak)) DigitalPinState_t readPushButton_SettingDown(){ return DIGITAL_PIN_LOW; }
__attribute__ ((weak)) void writeUVControl(DigitalPinState_t state){}
__attribute__ ((weak)) void writeHeaterControl(DigitalPinState_t state){}
__attribute__ ((weak)) void writeFanControl(DigitalPinState_t state){}
__attribute__ ((weak)) DigitalPinState_t readUVControl(){ return DIGITAL_PIN_LOW; }
__attribute__ ((weak)) DigitalPinState_t readHeaterControl(){ return DIGITAL_PIN_LOW; }
__attribute__ ((weak)) DigitalPinState_t readFanControl(){ return DIGITAL_PIN_LOW; }
__attribute__ ((weak)) double readThermistorPinAnalogRatio(){ return 0; }
__attribute__ ((weak)) void resetTimer4Setting(){}
__attribute__ ((weak)) int readTime4Setting_ms(){ return 0; }
__attribute__ ((weak)) void resetTimer4Period(){}
__attribute__ ((weak)) int readTime4Period_ms(){ return 0; }

