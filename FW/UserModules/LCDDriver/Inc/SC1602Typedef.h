#ifndef SC1602TYPEDEF_H_
#define SC1602TYPEDEF_H_

typedef enum{
    INTERFACE_DATA_8BIT = (1 << 4),
    INTERFACE_DATA_4BIT = (0 << 4)
}InterfaceDataLength_t;

typedef enum{
    LINE_2 = (1 << 3),
    LINE_1 = (0 << 3)
}NumberOfLine_t;
typedef enum{
    FONT_SIZE_5_11 = (1 << 2),
    FONT_SIZE_5_8 = (0 << 2)
}FontSize_t;
typedef enum{
    DISPLAY_ON = (1 << 2),
    DISPLAY_OFF = (0 << 2)
}DisplayONOFF_t;
typedef enum{
    CURSOR_ON = (1 << 1),
    CURSOR_OFF = (0 << 1)
} CursorONOFF_t;

typedef enum{
    BLINK_ON = 1,
    BLINK_OFF = 0
}BlinkONOFF_t;
typedef enum{
    CURSOR_INCREASE = (1 << 1),
    CURSOR_DECREASE = (0 << 1)
}CursorMoveDirection_t;
typedef enum{
    Shift_ON = 1,
    Shift_OFF = 0
}Shift_t;
typedef enum{
    SC1602_PIN_HIGH,
    SC1602_PIN_LOW
}SC1602_PinState_t;

//ピンの状態名
#define RSPINSTATE_CONTROL SC1602_PIN_LOW
#define RSPINSTATE_DATA SC1602_PIN_HIGH
#define RWPINSTATE_W SC1602_PIN_LOW
#define RWPINSTATE_R SC1602_PIN_HIGH

//SC1602固有の時間[ms]
#define T_PW 1 //仕様は140nsec
#define DESCRIPTION_TIME_RETURN_HOME 2 //仕様は1.52ms
#define DESCRIPTION_TIME_CLEAR_DISPLAY 2 //仕様は1.52ms
#define BOOT_TIME 40 //起動後40msec待つらしい
#define DESCRIPTION_TIME_FUNCTION_SET 1 //仕様は37us
#define DESCRIPTION_TIME_DISPLAY_ONOFF 1 //仕様は37us
#define DESCRIPTION_TIME_ENTRYMODE_SET 1 //仕様は37us
#define DESCRIPTION_TIME_SET_DDRAM_ADDRESS 1//仕様は37us
#define DESCRIPTION_TIME_WRITE_DATA_TO_RAM 1//仕様は37us

//コマンドフラグ
#define SET_DDRAM_COMMAND  0x80
#define SET_CGRAM_COMMAND  0x40
#define FUNCTION_SET_COMMAND  0x20
#define CURSOR_OR_DISPLAY_SHIFT_COMMAND  0x10
#define DISPLAY_ON_OFF_COMMAND  0x08
#define ENTRY_MODE_SET_COMMAND  0x04
#define RETURN_HOME_COMMAND  0x02
#define CLEAR_DISPLAY_COMMAND  0x01

//パラメータ定義域
#define DDRAM_ADDRESS_LINE1_MIN  0x00
#define DDRAM_ADDRESS_LINE1_MAX  0x0F
#define DDRAM_ADDRESS_LINE2_MIN  0x40
#define DDRAM_ADDRESS_LINE2_MAX  0x4F
#define DDRAM_LENGTH_PER_LINE (DDRAM_ADDRESS_LINE1_MAX - DDRAM_ADDRESS_LINE1_MIN + 1)

#endif