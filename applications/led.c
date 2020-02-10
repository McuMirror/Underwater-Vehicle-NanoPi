/*
 * @Description: 
 * @Author: chenxi
 * @Date: 2020-02-02 15:32:14
 * @LastEditTime : 2020-02-02 16:49:59
 * @LastEditors  : chenxi
 */
#include <stdio.h>
#include <stdlib.h>

#include "led.h"

#include <wiringPi.h>

void LED_Init(void)
{
    pinMode(LED_Red, OUTPUT);
    digitalWrite(LED_Red, HIGH);
    pinMode(LED_Green, OUTPUT);
    digitalWrite(LED_Green, HIGH);
    pinMode(LED_Blue, OUTPUT);
    digitalWrite(LED_Blue, HIGH);
}

void ErrorStatus_LED(void)
{
    LED_ON(LED_Red);
    LED_OFF(LED_Green);
    LED_OFF(LED_Blue);
}

void ALL_LED_OFF(void)
{
    digitalWrite(LED_Red, HIGH);
    digitalWrite(LED_Green, HIGH);
    digitalWrite(LED_Blue, HIGH);
}