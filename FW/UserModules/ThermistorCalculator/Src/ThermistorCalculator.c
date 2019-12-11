/*
 * ThermistorCalc.c
 *
 *  Created on: 2018/01/22
 *      Author: Shuji
 */
#include "ThermistorCalculator.h"
#include "math.h"

//! 絶対零度の摂氏温度
#define ZERO_KELVIN (double)(-273.15)

//! R0測定時の温度
#define ROOM_TEMPERATURE (double)25.0

double CalculateTemperature(double thermistorRegistance, const double B, const double Rroom)
{
    return 1.0 / (1.0 / (ROOM_TEMPERATURE - ZERO_KELVIN) + 1.0 / B * (log(thermistorRegistance) - log(Rroom))) + ZERO_KELVIN;
}