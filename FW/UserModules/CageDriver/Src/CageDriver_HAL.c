#include "CageDriver_HAL.h"

__attribute__ ((weak)) DigitalPinState_t readSettingEntrySwitch()
{
    return DIGITAL_PIN_LOW;
}
__attribute__ ((weak)) DigitalPinState_t readUVControlSwitch()
{
    return DIGITAL_PIN_LOW;
}
__attribute__ ((weak)) DigitalPinState_t readPushButton_SettingUp()
{
    return DIGITAL_PIN_LOW;
}
__attribute__ ((weak)) DigitalPinState_t readPushButton_SettingDown()
{
    return DIGITAL_PIN_LOW;
}
__attribute__ ((weak)) void writeUVControl(DigitalPinState_t state)
{

}
__attribute__ ((weak)) void writeHeaterControl(DigitalPinState_t state)
{

}
__attribute__ ((weak)) void writeFanControl(DigitalPinState_t state)
{

}
__attribute__ ((weak)) DigitalPinState_t readUVControl()
{
    return DIGITAL_PIN_LOW;
}
__attribute__ ((weak)) DigitalPinState_t readHeaterControl()
{
    return DIGITAL_PIN_LOW;
}
__attribute__ ((weak)) DigitalPinState_t readFanControl()
{
    return DIGITAL_PIN_LOW;
}
