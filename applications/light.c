/*
 * @Description: 探照灯
 * @Author: chenxi
 * @Date: 2020-03-18 10:31:23
 * @LastEditTime: 2020-03-18 20:27:23
 * @LastEditors: chenxi
 */

#define LOG_TAG "light"

#include "../easylogger/inc/elog.h"
#include "light.h"

#include <stdlib.h>
#include "pwm.h"

short light_value = 0;

/*******************************************
* 函 数 名：Light_Output_Limit
* 功    能：灯光亮度输出限制
* 输入参数：灯光亮度值 0~90%
* 返 回 值：None
* 注    意：
********************************************/
int Light_Output_Limit(short *value)
{
    *value = *value >= 90 ? 90 : *value; //限幅
    *value = *value <= 0 ? 0 : *value;   //限幅
    return *value;
}

/**
  * @brief  Light_Control(探照灯控制)
  * @param  控制指令 0x00：不动作  0x01：向上  0x02：向下
  * @retval None
  * @notice 
  */
void Search_Light_Control(uint8 *action)
{
    switch (*action)
    {
    case 0x01:
        light_value += 5; //变亮
        break;
    case 0x02:
        light_value -= 5; //变暗
        break;
    default:
        break;
    }
    Light_Output_Limit(&light_value);
    I2C_PWM_SetPWM(8, 0, light_value);
    I2C_PWM_SetPWM(9, 0, light_value);
    *action = 0x00; //清除控制字
}
