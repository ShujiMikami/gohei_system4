#ifndef CAGEDRIVER_HAL_H_
#define CAGEDRIVER_HAL_H_

//To be defined by user
typedef enum{
    DIGITAL_PIN_HIGH,
    DIGITAL_PIN_LOW
}DigitalPinState_t;

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

#endif
