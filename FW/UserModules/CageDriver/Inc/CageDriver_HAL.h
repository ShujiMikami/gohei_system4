#ifndef CAGEDRIVER_HAL_H_
#define CAGEDRIVER_HAL_H_

typedef enum{
    DIGITAL_PIN_HIGH,
    DIGITAL_PIN_LOW
}DigitalPinState_t;

//To be defined by user
DigitalPinState_t readSettingEntrySwitch();
DigitalPinState_t readUVControlSwitch();
DigitalPinState_t readPushButton_SettingUp();
DigitalPinState_t readPushButton_SettingDown();

void writeUVControl(DigitalPinState_t state);
void writeHeaterControl(DigitalPinState_t state);
void writeFanControl(DigitalPinState_t state);

DigitalPinState_t readUVControl();
DigitalPinState_t readHeaterControl();
DigitalPinState_t readFanControl();

double readThermistorPinAnalogRatio();

void resetTimer4Setting();
int readTime4Setting_ms();

void resetTimer4Period();
int readTime4Period_ms();
#endif
