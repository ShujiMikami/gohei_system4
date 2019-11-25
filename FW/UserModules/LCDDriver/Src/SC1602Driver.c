#include "SC1602Driver.h"
#include "SC1602Typedef.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

static void writeCommand(uint8_t data);
static void writeData(uint8_t data);
static void writeCharToPos(uint8_t pos, char data);
static void ePinControl();
static void setDDRAMaddress(uint8_t address);
static void functionSet(InterfaceDataLength_t dataTransferMode, NumberOfLine_t indicateLineNum, FontSize_t fontSize);
static void displayONOFFcontrol(DisplayONOFF_t displayON, CursorONOFF_t cursorON, BlinkONOFF_t blinkingOn);
static void entryModeSet(CursorMoveDirection_t direction, Shift_t shift);
static bool isAddressInRange(uint8_t address);
static void writepin_DataBus(uint8_t data);

void Initialize_SC1602()
{
    //起動待ち
    wait_ms(BOOT_TIME);

    //ファンクションセット
    functionSet(INTERFACE_DATA_8BIT, LINE_2, FONT_SIZE_5_8);

    //DisplayONOFFセット
    displayONOFFcontrol(DISPLAY_ON, CURSOR_OFF, BLINK_OFF);

    //Displayクリア
    ClearDisplay_SC1602();

    //Entry modeセット
    entryModeSet(CURSOR_INCREASE, Shift_OFF);    
}
void functionSet(InterfaceDataLength_t dataTransferMode, NumberOfLine_t indicateLineNum, FontSize_t fontSize)
{
    writeCommand(FUNCTION_SET_COMMAND | (int)dataTransferMode | (int) indicateLineNum | (int)fontSize);

    wait_ms(DESCRIPTION_TIME_FUNCTION_SET);
}
void displayONOFFcontrol(DisplayONOFF_t displayON, CursorONOFF_t cursorON, BlinkONOFF_t blinkingOn)
{
    writeCommand(DISPLAY_ON_OFF_COMMAND | (int)displayON | (int)cursorON | (int) blinkingOn);

    wait_ms(DESCRIPTION_TIME_DISPLAY_ONOFF);
}
void entryModeSet(CursorMoveDirection_t direction, Shift_t shift)
{
    writeCommand(ENTRY_MODE_SET_COMMAND | (int)direction | (int)shift);

    wait_ms(DESCRIPTION_TIME_ENTRYMODE_SET);
}
void ClearDisplay_SC1602()
{
    writeCommand(CLEAR_DISPLAY_COMMAND);

    wait_ms(DESCRIPTION_TIME_CLEAR_DISPLAY);
}
void ReturnHome_SC1602()
{
    writeCommand(RETURN_HOME_COMMAND);
    
    wait_ms(DESCRIPTION_TIME_RETURN_HOME);
}
void WriteString_SC1602(char* string, int line)
{
    int dataLength = strlen(string);

    if(dataLength > DDRAM_LENGTH_PER_LINE){
        dataLength = DDRAM_LENGTH_PER_LINE;
    }

    int startPos = DDRAM_ADDRESS_LINE1_MIN;
    if(line == 1){
        startPos = DDRAM_ADDRESS_LINE1_MIN;
    }
    else{
        startPos = DDRAM_ADDRESS_LINE2_MIN;
    }

    for(int i = 0; i < dataLength; i++){
        writeCharToPos(i + startPos, string[i]);
    }
}
void writeCommand(uint8_t data)
{
    //RSPinをコントロール
    writepin_RS(RSPINSTATE_CONTROL);

    //RWPinを書き込み
    writepin_RW(RWPINSTATE_W);

    //データバスセット
    writepin_DataBus(data);

    //データ送信
    ePinControl();
}
void ePinControl()
{
    //T_PWの正パルスを与える
    writepin_E(SC1602_PIN_HIGH);
    wait_ms(T_PW);
    writepin_E(SC1602_PIN_LOW);
}
void writeData(uint8_t data)
{
    //RSPinをコントロール
    writepin_RS(RSPINSTATE_DATA);

    //RWPinを書き込み
    writepin_RW(RWPINSTATE_W);

    //データバスセット
    writepin_DataBus(data);

    //データ送信
    ePinControl();
}
void writeCharToPos(uint8_t pos, char data)
{
    setDDRAMaddress(pos);

    writeData((uint8_t)data);

    wait_ms(DESCRIPTION_TIME_WRITE_DATA_TO_RAM);
}
void setDDRAMaddress(uint8_t address)
{
    if(isAddressInRange(address)){
        writeCommand(SET_DDRAM_COMMAND | address);
        wait_ms(DESCRIPTION_TIME_SET_DDRAM_ADDRESS);
    }
}
bool isAddressInRange(uint8_t address)
{
    return ((address <= DDRAM_ADDRESS_LINE1_MAX)
            || (DDRAM_ADDRESS_LINE2_MIN <= address && address <= DDRAM_ADDRESS_LINE2_MAX));
}
void writepin_DataBus(uint8_t data)
{
    SC1602_PinState_t statesToWrite[8];

    for(int i = 0; i < 8; i++){
        if((data >> i & 0x01) == 0x01){
            statesToWrite[i] = SC1602_PIN_HIGH;
        }else{
            statesToWrite[i] = SC1602_PIN_LOW;
        }
    }
    writepin_DATA0(statesToWrite[0]);
    writepin_DATA1(statesToWrite[1]);
    writepin_DATA2(statesToWrite[2]);
    writepin_DATA3(statesToWrite[3]);
    writepin_DATA4(statesToWrite[4]);
    writepin_DATA5(statesToWrite[5]);
    writepin_DATA6(statesToWrite[6]);
    writepin_DATA7(statesToWrite[7]);
}
__attribute__ ((weak)) void writepin_DATA0(SC1602_PinState_t status)
{
    assert(1);
}
__attribute__ ((weak)) void writepin_DATA1(SC1602_PinState_t status)
{
    assert(1);
}
__attribute__ ((weak)) void writepin_DATA2(SC1602_PinState_t status)
{
    assert(1);
}
__attribute__ ((weak)) void writepin_DATA3(SC1602_PinState_t status)
{
    assert(1);
}
__attribute__ ((weak)) void writepin_DATA4(SC1602_PinState_t status)
{
    assert(1);
}
__attribute__ ((weak)) void writepin_DATA5(SC1602_PinState_t status)
{
    assert(1);
}
__attribute__ ((weak)) void writepin_DATA6(SC1602_PinState_t status)
{
    assert(1);
}
__attribute__ ((weak)) void writepin_DATA7(SC1602_PinState_t status)
{
    assert(1);
}
__attribute__ ((weak)) void writepin_RS(SC1602_PinState_t status)
{
    assert(1);
}
__attribute__ ((weak)) void writepin_RW(SC1602_PinState_t status)
{
    assert(1);
}
__attribute__ ((weak)) void writepin_E(SC1602_PinState_t status)
{
    assert(1);
}
__attribute__ ((weak)) void wait_ms(int milisecond)
{
    assert(1);
}