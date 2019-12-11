#include "CageDriver_HAL.h"
#include "SC1602Driver_HAL.h"

#include "gpio.h"


//SC1602DriverのHAL実装
static GPIO_PinState stateConvert_SC1602(SC1602_PinState_t status);

static GPIO_PinState stateConvert_SC1602(SC1602_PinState_t status)
{
    GPIO_PinState result;

    if(status == SC1602_PIN_HIGH){
        result = GPIO_PIN_SET;
    }else{
        result = GPIO_PIN_RESET;
    }

    return result;
}
void writepin_DATA0(SC1602_PinState_t status)
{

}
void writepin_DATA1(SC1602_PinState_t status)
{

}
void writepin_DATA2(SC1602_PinState_t status)
{

}
void writepin_DATA3(SC1602_PinState_t status)
{

}
void writepin_DATA4(SC1602_PinState_t status)
{

}
void writepin_DATA5(SC1602_PinState_t status)
{

}
void writepin_DATA6(SC1602_PinState_t status)
{

}
void writepin_DATA7(SC1602_PinState_t status)
{

}
void writepin_RS(SC1602_PinState_t status)
{

}
void writepin_RW(SC1602_PinState_t status)
{

}
void writepin_E(SC1602_PinState_t status)
{

}
void wait_ms(int milisecond)
{

}