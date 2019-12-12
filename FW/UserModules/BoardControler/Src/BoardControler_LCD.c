#include "SC1602Driver_HAL.h"

#include "gpio.h"
#include "cmsis_os.h"

//SC1602DriverのHAL実装
static GPIO_PinState stateConvert_SC1602(SC1602_PinState_t status);


void writepin_DATA0(SC1602_PinState_t status)
{
    HAL_GPIO_WritePin(LCD_DATA0_GPIO_Port, LCD_DATA0_Pin, stateConvert_SC1602(status));
}
void writepin_DATA1(SC1602_PinState_t status)
{
    HAL_GPIO_WritePin(LCD_DATA1_GPIO_Port, LCD_DATA1_Pin, stateConvert_SC1602(status));
}
void writepin_DATA2(SC1602_PinState_t status)
{
    HAL_GPIO_WritePin(LCD_DATA2_GPIO_Port, LCD_DATA2_Pin, stateConvert_SC1602(status));
}
void writepin_DATA3(SC1602_PinState_t status)
{
    HAL_GPIO_WritePin(LCD_DATA3_GPIO_Port, LCD_DATA3_Pin, stateConvert_SC1602(status));
}
void writepin_DATA4(SC1602_PinState_t status)
{
    HAL_GPIO_WritePin(LCD_DATA4_GPIO_Port, LCD_DATA4_Pin, stateConvert_SC1602(status));
}
void writepin_DATA5(SC1602_PinState_t status)
{
    HAL_GPIO_WritePin(LCD_DATA5_GPIO_Port, LCD_DATA5_Pin, stateConvert_SC1602(status));
}
void writepin_DATA6(SC1602_PinState_t status)
{
    HAL_GPIO_WritePin(LCD_DATA6_GPIO_Port, LCD_DATA6_Pin, stateConvert_SC1602(status));
}
void writepin_DATA7(SC1602_PinState_t status)
{
    HAL_GPIO_WritePin(LCD_DATA7_GPIO_Port, LCD_DATA7_Pin, stateConvert_SC1602(status));
}
void writepin_RS(SC1602_PinState_t status)
{
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, stateConvert_SC1602(status));
}
void writepin_RW(SC1602_PinState_t status)
{
    HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, stateConvert_SC1602(status));
}
void writepin_E(SC1602_PinState_t status)
{
    HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, stateConvert_SC1602(status));
}
void wait_ms(int milisecond)
{
    osDelay(milisecond);
}
GPIO_PinState stateConvert_SC1602(SC1602_PinState_t status)
{
    GPIO_PinState result;

    if(status == SC1602_PIN_HIGH){
        result = GPIO_PIN_SET;
    }else{
        result = GPIO_PIN_RESET;
    }

    return result;
}