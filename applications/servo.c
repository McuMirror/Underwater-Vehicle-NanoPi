/*
 * @Description: 舵机设备
 * @Author: chenxi
 * @Date: 2020-03-18 10:44:44
 * @LastEditTime: 2020-03-18 10:59:26
 * @LastEditors: chenxi
 */

#define LOG_TAG "servo"

#include "../easylogger/inc/elog.h"
#include "../drivers/drv_pca9685.h"

#include "servo.h"
#include "ioDevices.h"
#include "propeller.h"
#include "ret_data.h"

#include <stdio.h>
#include <stdlib.h>



/*---------------------- Constant / Macro Definitions -----------------------*/

#define RoboticArm_MedValue 1500
#define YunTai_MedValue 2000

/*----------------------- Variable Declarations -----------------------------*/

ServoType RoboticArm = {
    .MaxValue = 2000, //机械臂 正向最大值
    .MinValue = 1000, //机械臂 反向
    .MedValue = 1500,
    .Speed = 5 //机械臂当前值
};             //机械臂
ServoType YunTai = {
    .MaxValue = 2500, //机械臂 正向最大值
    .MinValue = 1500, //机械臂 反向
    .MedValue = 2000,
    .Speed = 10 //云台转动速度
};              //云台

uint16 propeller_power = 1500;
short _test_value = 0;

/*----------------------- Function Implement --------------------------------*/

/*******************************************
* 函 数 名：Servo_Output_Limit
* 功    能：舵机输出限制
* 输入参数：输入值：舵机结构体地址 
* 返 回 值：None
* 注    意：
********************************************/
void Servo_Output_Limit(ServoType *Servo)
{
    Servo->CurrentValue = Servo->CurrentValue > Servo->MaxValue ? Servo->MaxValue : Servo->CurrentValue; //正向限幅
    Servo->CurrentValue = Servo->CurrentValue < Servo->MinValue ? Servo->MinValue : Servo->CurrentValue; //反向限幅
}

/**
  * @brief  RoboticArm_Control(机械臂控制)
  * @param  控制指令 0x00：不动作  0x01：张开  0x02：关闭
  * @retval None
  * @notice 
  */
void RoboticArm_Control(uint8 *action)
{
    static uint8 on_off = 0; //自锁开关
    if (0 == on_off)
    {
        on_off = 1;
        RoboticArm.CurrentValue = RoboticArm.MedValue;
    }

    switch (*action)
    {
    case 0x01:
        RoboticArm.CurrentValue += RoboticArm.Speed;
        if (RoboticArm.CurrentValue >= RoboticArm.MaxValue)
        {
            device_hint_flag |= 0x01;
        } //机械臂到头标志
        else
        {
            device_hint_flag &= 0xFE;
        }; //清除机械臂到头标志

        break;
    case 0x02:
        RoboticArm.CurrentValue -= RoboticArm.Speed;
        if (RoboticArm.CurrentValue <= RoboticArm.MinValue)
        {
            device_hint_flag |= 0x01;
        } //机械臂到头标志
        else
        {
            device_hint_flag &= 0xFE;
        }; //清除机械臂到头标志

        break;
    default:
        break;
    }
    Servo_Output_Limit(&RoboticArm); //机械臂舵机限幅
    pca9685PWMWrite(10, 0, RoboticArm.CurrentValue);
    *action = 0x00; //清除控制字
}

/**
  * @brief  YunTai_Control(云台控制)
  * @param  控制指令 0x00：不动作  0x01：向上  0x02：向下
  * @retval None
  * @notice 
  */

void YunTai_Control(uint8 *action)
{
    static uint8 on_off = 0; //自锁开关
    if (0 == on_off)
    {
        on_off = 1;
        YunTai.CurrentValue = YunTai.MedValue;
    }
    switch (*action)
    {
    case 0x01:
        YunTai.CurrentValue -= YunTai.Speed; //向上
        if (YunTai.CurrentValue <= YunTai.MaxValue)
        {
            device_hint_flag |= 0x02;
        } //云台到头标志
        else
        {
            device_hint_flag &= 0xFD;
        }; //清除云台到头标志
        break;

    case 0x02:
        YunTai.CurrentValue += YunTai.Speed; //向下
        if (YunTai.CurrentValue >= YunTai.MinValue)
        {
            device_hint_flag |= 0x02;
        } //云台到头标志
        else
        {
            device_hint_flag &= 0xFD;
        }; //清除云台到头标志
        break;

    case 0x03:
        YunTai.CurrentValue = YunTai.MedValue;
        break; //归中

    default:
        break;
    }
    Servo_Output_Limit(&YunTai);
    // TODO 云台1还是云台2
    pca9685PWMWrite(6, 0, YunTai.CurrentValue);
    *action = 0x00; //清除控制字
}
