// #include "mbed.h"
#include "SC1602Driver.h"
#include "ThermistorCalculator.h"
#include "stdlib.h"
#include "stdio.h"
#include "CageDriver.h"
#include "cmsis_os.h"
#include "CageDriver_HAL.h"

//SettingSwitchの切り替え先定義
#define SETTING_SWITCH_SETTING (DigitalPinState_t)DIGITAL_PIN_HIGH
#define SETTING_SWITCH_OPERATING (DigitalPinState_t)DIGITAL_PIN_LOW

//Settingのタクトボタンの負論理定義
#define SETTING_SWITCH_PUSHED (DigitalPinState_t)DIGITAL_PIN_LOW

//UVスイッチの正論理定義
#define UV_SWITCH_ON (DigitalPinState_t)DIGITAL_PIN_HIGH
#define UV_SWITCH_OFF (DigitalPinState_t)DIGITAL_PIN_LOW

//制御のONOFF正論理定義
#define CONTROL_STATUS_ON (DigitalPinState_t)DIGITAL_PIN_HIGH
#define CONTROL_STATUS_OFF (DigitalPinState_t)DIGITAL_PIN_LOW

//バージョン番号
const char versionNumber[] = "Ver 0.0.2";

//システムステータス
typedef enum{
    SYSTEM_SETTING = 0,
    SYSTEM_OPERATING
}SystemStatus_t;

//オペレーティングステータス
typedef enum{
    FAN_COOLING = 0,
    NATURAL_COOLING,
    HEATING
}OperatingStatus_t;

//サーミスタ周り
double calculateThermistorResistance(double adcRatio);
const double R_ROOM = 10000.0;
const double B_CONST = 3380.0;
const double R_SERIES = 10000.0;
ThermistorCalculator thermoCalculator(B_CONST, R_ROOM);
double measureTemperature();

//operatingAction
double targetTemperature = 25.0;
double targetTemperatureLowerLimit = 20;
double targetTemperatureUpperLimit = 35;
double dangerZone = 40.0;
double deadZone = 1.0;
void operatingAction();
// Timer timer;
// float operatingPeriod = 1;//3秒周期制御

//settingAction
void settingAction();
Timer timer4Setting;
float buttonDisableTime = 300;//300msecはボタン無効時間
void indicateSetTemperature();

//action
void systemAction(SystemStatus_t systemStatus);

//Current Status Indicate
void indicateCurrentStatus(double currentTemperature, char* controlStatus);

//起動メッセージ系
char initialString[] = "System Start";
void indicateInitialMessage();

//状態判定系
SystemStatus_t getRequiredSystemStatus();


//外部から覗ける変数
static double currentTemperature = 0;
static char operatingStatusMessage[26];
static int uvControlFlag_Ether = UV_SWITCH_OFF;
static bool isRemoteControlEnabled = false;

void CageDriveThread() {
    //起動メッセージ表示
    indicateInitialMessage();

    SystemStatus_t operatingStatus = SYSTEM_OPERATING;

    while(1) {
        //スイッチ状態監視と状態遷移
        operatingStatus = getRequiredSystemStatus();

        //状態に応じたアクション
        systemAction(operatingStatus);
    }
}
double calculateThermistorResistance(double adcRatio)
{
    return adcRatio / (1.0 - adcRatio) * R_SERIES;
}
double measureTemperature()
{
    double thermistorResistance = calculateThermistorResistance(thermistorPin.read());

    return thermoCalculator.CalculateTemperature(thermistorResistance);
}
void operatingAction()
{
    //温度を測定
    currentTemperature = measureTemperature();

    //動作モード確定
    static OperatingStatus_t operatingStatus = NATURAL_COOLING;
    if(currentTemperature > dangerZone){
        operatingStatus = FAN_COOLING;
    }else if(targetTemperature + deadZone < currentTemperature && currentTemperature <= dangerZone){
        operatingStatus = NATURAL_COOLING;
    }else if(targetTemperature - deadZone < currentTemperature && currentTemperature <= targetTemperature + deadZone){
        //deadZone内では, 前のセッティングを保持
    }else if(currentTemperature <= targetTemperature - deadZone){
        operatingStatus = HEATING;
    }

    //表示する状態文字を指定
    if(operatingStatus == FAN_COOLING){
        sprintf(operatingStatusMessage, "%s", "Fan Cooling");
    }else if(operatingStatus == NATURAL_COOLING){
        sprintf(operatingStatusMessage, "%s", "Natural Cooling");
    }else if(operatingStatus == HEATING){
        sprintf(operatingStatusMessage, "%s", "HEATING");
    }else{
        sprintf(operatingStatusMessage, "%s", "NOT DEFINED");
    }

    //ファン, ヒータ制御
    if(operatingStatus == FAN_COOLING){
        heaterControl = CONTROL_STATUS_OFF;
        fanControl = CONTROL_STATUS_ON;
    }else if(operatingStatus == NATURAL_COOLING){
        heaterControl = CONTROL_STATUS_OFF;
        fanControl = CONTROL_STATUS_OFF;
    }else{
        heaterControl = CONTROL_STATUS_ON;
        fanControl = CONTROL_STATUS_OFF;
    }

    //LCDに反映
    indicateCurrentStatus(currentTemperature, operatingStatusMessage);
}
void settingAction()
{
    static bool buttonEnabled = true;

    if(buttonEnabled){
        if(settingUpSwitch == SETTING_SWITCH_PUSHED){
            buttonEnabled = false;
            timer4Setting.start();
            if(targetTemperature < targetTemperatureUpperLimit){
                targetTemperature++;
            }
            indicateSetTemperature();
        }else if(settingDownSwitch == SETTING_SWITCH_PUSHED){
            buttonEnabled = false;
            timer4Setting.start();
            if(targetTemperature > targetTemperatureLowerLimit){
                targetTemperature--;
            }
            indicateSetTemperature();
        }
    }else{
        if(timer4Setting.read_ms() > buttonDisableTime){
            buttonEnabled = true;
            timer4Setting.stop();
            timer4Setting.reset();
        }
    }
}
void systemAction(SystemStatus_t systemStatus)
{
    //UVスイッチ監視, 制御
    if(!isRemoteControlEnabled){//リモート制御無効時
        if(uvControlSwitch == UV_SWITCH_ON){
            uvControl = CONTROL_STATUS_ON;
        }else{
            uvControl = CONTROL_STATUS_OFF;
        }
    }else{//リモート制御が有効時
        if(uvControlFlag_Ether == UV_SWITCH_ON){
            uvControl = CONTROL_STATUS_ON;
        }else{
            uvControl = CONTROL_STATUS_OFF;
        }
    }

    static bool isTimerStatrted = false;

    static bool outOfSettingModeFlag = true;

    if(systemStatus == SYSTEM_OPERATING){
        outOfSettingModeFlag = true;

        if(!isTimerStatrted){
            timer.start();
            isTimerStatrted = true;
        }

        if(timer.read() > operatingPeriod){
            operatingAction();
            timer.reset();
            DEBUG_PRINT("[CageDrive Thread]Operating Period\r\n");
        }
    }else{
        //SettingModeに入ると, Ether制御を無効にする
        isRemoteControlEnabled = false;

        timer.stop();
        timer.reset();
        isTimerStatrted = false;

        //セッティングモードに入った初回の表示
        if(outOfSettingModeFlag){
            indicateSetTemperature();
            outOfSettingModeFlag = false;
            DEBUG_PRINT("[CageDrive Thread]Setting Mode\r\n");
        }

        settingAction();
    }
}
void indicateSetTemperature()
{
    char buffer[16];

    sprintf(buffer, "%2.1f%c%c", targetTemperature, (char)0xDF, 'C');

    LCD.ClearDisplay();

    LCD.WriteString(buffer, 1);
}
void indicateCurrentStatus(double currentTemperature, char* controlStatus)
{
    LCD.ClearDisplay();
    char line1Buf[16];
    sprintf(line1Buf, "%2.1f%c%c", currentTemperature, (char)0xDF, 'C');

    LCD.WriteString(line1Buf, 1);
    LCD.WriteString(controlStatus, 2);
}
void indicateInitialMessage()
{
    Initialize_SC1602();
    WriteString_SC1602(initialString, 1);
    WriteString_SC1602((char*)versionNumber, 2);
    osDelay(3000);
}
SystemStatus_t getRequiredSystemStatus()
{
    SystemStatus_t result = SYSTEM_OPERATING;

    if(readSettingEntrySwitch() == SETTING_SWITCH_SETTING){
        result = SYSTEM_SETTING;
    }else{
        result = SYSTEM_OPERATING;
    }

    return result;
}
CageStatus_t GetCageStatus()
{
    CageStatus_t result;

    result.temperature = currentTemperature;
    sprintf(result.statusMessage, "%s", operatingStatusMessage);

    if(uvControl == UV_SWITCH_ON){
        strcpy(result.uvStatusMessage, "ON");
    }else{
        strcpy(result.uvStatusMessage, "OFF");
    }

    return result;
}
void UVOnFromEther()
{
    isRemoteControlEnabled = true;
    uvControlFlag_Ether = UV_SWITCH_ON;

    while(uvControl != uvControlFlag_Ether);
}
void UVOffFromEther(){
    isRemoteControlEnabled = true;
    uvControlFlag_Ether = UV_SWITCH_OFF;
    
    while(uvControl != uvControlFlag_Ether);
}
void UVToggleFromEther()
{
    if(uvControlFlag_Ether == UV_SWITCH_ON){
        UVOffFromEther();
    }else{
        UVOnFromEther();
    }
}
