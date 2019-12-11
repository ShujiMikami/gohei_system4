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


#endif
