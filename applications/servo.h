/*
 * @Description: 舵机设备
 * @Author: chenxi
 * @Date: 2020-03-18 10:44:52
 * @LastEditTime: 2020-03-18 10:47:44
 * @LastEditors: chenxi
 */

#ifndef __SERVO_H
#define __SERVO_H

#include "../user/DataType.h"

typedef struct
{
    short MaxValue;     //舵机 正向最大值
    short MinValue;     //舵机 反向
    short MedValue;     //舵机 中值
    short CurrentValue; //舵机当前值
    uint8 Speed;        //舵机速度值
} ServoType;

void RoboticArm_Control(uint8 *action);
void YunTai_Control(uint8 *action);
void DirectionProportion(int Mode);

extern ServoType RoboticArm; //机械臂
extern ServoType YunTai; //云台
extern int DirectionMode;

#endif
