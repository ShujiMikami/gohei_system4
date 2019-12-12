#include "CageDriver_HAL.h"

#include "gpio.h"
#include "adc.h"
#include "cmsis_os.h"

static GPIO_PinState stateConvertFromCage(DigitalPinState_t status);
static DigitalPinState_t stateConvertToCage(GPIO_PinState status);

DigitalPinState_t readSettingEntrySwitch()
{
    stateConvertToCage(HAL_GPIO_ReadPin(SETTING_ENTRY_SW_GPIO_Port, SETTING_ENTRY_SW_Pin));
}
DigitalPinState_t readUVControlSwitch()
{
    stateConvertToCage(HAL_GPIO_ReadPin(UVSETTING_SW_GPIO_Port, UVSETTING_SW_Pin));
}
DigitalPinState_t readPushButton_SettingUp()
{
    stateConvertToCage(HAL_GPIO_ReadPin(SETTING_BUTTON_UP_GPIO_Port, SETTING_BUTTON_UP_Pin));
}
DigitalPinState_t readPushButton_SettingDown()
{
    stateConvertToCage(HAL_GPIO_ReadPin(SETTING_BUTTON_DOWN_GPIO_Port, SETTING_BUTTON_DOWN_Pin));
}

void writeUVControl(DigitalPinState_t state)
{
    HAL_GPIO_WritePin(UV_CONTROL_GPIO_Port, UV_CONTROL_Pin, stateConvertFromCage(state));
}
void writeHeaterControl(DigitalPinState_t state)
{
    HAL_GPIO_WritePin(HEATER_CONTROL_GPIO_Port, HEATER_CONTROL_Pin, stateConvertFromCage(state));
}
void writeFanControl(DigitalPinState_t state)
{
    HAL_GPIO_WritePin(FAN_CONTROL_GPIO_Port, FAN_CONTROL_Pin, stateConvertFromCage(state));
}

DigitalPinState_t readUVControl()
{
    stateConvertToCage(HAL_GPIO_ReadPin(UV_CONTROL_GPIO_Port, UV_CONTROL_Pin));
}
DigitalPinState_t readHeaterControl()
{
    stateConvertToCage(HAL_GPIO_ReadPin(HEATER_CONTROL_GPIO_Port, HEATER_CONTROL_Pin));
}
DigitalPinState_t readFanControl()
{
    stateConvertToCage(HAL_GPIO_ReadPin(FAN_CONTROL_GPIO_Port, FAN_CONTROL_Pin));
}

double readThermistorPinAnalogRatio()
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 1000);

    uint32_t adcVal = HAL_ADC_GetValue(&hadc1);

    return (double)adcVal / (double)0xFFF;
}

static int timer4SettingCount = 0;
void resetTimer4Setting()
{
    timer4SettingCount = osKernelSysTick();
}
int readTime4Setting_ms()
{
    return osKernelSysTick() - timer4SettingCount;
}

static int timer4PeriodCount = 0;
void resetTimer4Period()
{
    timer4PeriodCount = osKernelSysTick();
}
int readTime4Period_ms()
{
    return osKernelSysTick() - timer4PeriodCount;
}
GPIO_PinState stateConvertFromCage(DigitalPinState_t status)
{
    GPIO_PinState result;

    if(status == DIGITAL_PIN_HIGH){
        result = GPIO_PIN_SET;
    }else{
        result = GPIO_PIN_RESET;
    }

    return result;
}
DigitalPinState_t stateConvertToCage(GPIO_PinState status)
{
    DigitalPinState_t result;

    if(status == GPIO_PIN_SET){
        result = DIGITAL_PIN_HIGH;
    }else{
        result = DIGITAL_PIN_LOW;
    }

    return result;
}
