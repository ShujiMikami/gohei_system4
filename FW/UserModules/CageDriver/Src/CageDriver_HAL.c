#include "CageDriver_HAL.h"

__attribute__ ((weak)) DigitalPinState_t readSettingEntrySwitch()
{
    return DIGITAL_IN_LOW;
}
__attribute__ ((weak)) DigitalPinState_t readUVControlSwitch()
{
    return DIGITAL_IN_LOW;
}
__attribute__ ((weak)) DigitalPinState_t readPushButton_SettingUp()
{
    return DIGITAL_IN_LOW;
}
__attribute__ ((weak)) DigitalPinState_t readPushButton_SettingDown()
{
    return DIGITAL_IN_LOW;
}
__attribute__ ((weak)) void writeUVControl(DigitalPinState_t state)
{

}
