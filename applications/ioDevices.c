/*
 * @Description: 
 * @Author: chenxi
 * @Date: 2020-02-02 16:58:32
 * @LastEditTime : 2020-02-03 13:08:03
 * @LastEditors  : chenxi
 */
#include <stdio.h>
#include <stdlib.h>

#include "ioDevices.h"

#include <wiringPi.h>

Buzzer_Type Beep; //蜂鸣器控制器

/* 按键按下产生的任务 */
void key_down(void)
{
    // oled.pagenum++;
    printf("key_down\n");
}

void Buzzer_Init(void)
{
    pinMode(Buzzer_PIN, OUTPUT);
    digitalWrite(Buzzer_PIN, LOW);
}

/*buzzer为蜂鸣器控制器  count为响的次数  length响的时间长度(ms)  */
void Buzzer_Set(Buzzer_Type *buzzer, uint8 count, uint32 time)
{
    buzzer->count = count; // 响的时间长度
    buzzer->time = time;   // 保存时间间隔长度
}

/* 蜂鸣器鸣响任务【可指示系统各个状态】 */
void Buzzer_Process(Buzzer_Type buzzer)
{
    for (int i = buzzer.count; i > 1; i--)
    {
        Buzzer_ON();
        delay(buzzer.time);
        Buzzer_OFF();
        delay(buzzer.time);
    }
    // 最后一次响
    Buzzer_ON();
    delay(buzzer.time);
    Buzzer_OFF();
}
