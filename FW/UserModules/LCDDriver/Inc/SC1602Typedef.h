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

#endif