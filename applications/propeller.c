/*
 * @Description: 推进器设备
 * @Author: chenxi
 * @Date: 2020-03-01 22:36:16
 * @LastEditTime: 2020-03-18 19:22:50
 * @LastEditors: chenxi
 */

#define LOG_TAG "propeller"

#include "../easylogger/inc/elog.h"
#include "propeller.h"

#include <stdlib.h>
#include <string.h>

#include "rc_data.h"
#include "I2C_PWM.h"

#include <wiringPi.h>

uint8 Propeller_Init_Flag = 0;

PropellerParameter_Type PropellerParameter = {
	//初始化推进器参数值【限幅】
	.PowerMax = 429, //正向最大值
	.PowerMed = 287, //中值
	.PowerMin = 172, //反向最小值【反向推力最大】

	.PowerDeadband = 10 //死区值
};

PropellerDir_Type PropellerDir = {1, 1, 1, 1, 1, 1};		//推进器方向，默认为1
PropellerPower_Type PropellerPower = {0, 0, 0, 0, 0, 0, 0}; //推进器推力控制器
PropellerError_Type PropellerError = {0, 0, 0, 0, 0, 0};	//推进器偏差值

PropellerPower_Type power_test; //调试用的变量

PropellerError_Type Forward = {0, 0, 0, 0, 0, 0};
PropellerError_Type Retreat = {0, 0, 0, 0, 0, 0};
PropellerError_Type TurnLeft = {0, 0, 0, 0, 0, 0};
PropellerError_Type TurnRight = {0, 0, 0, 0, 0, 0};
Adjust_Parameter AdjustParameter = {1, 1, 1, 1};

int16 PowerPercent;
int Extractor_Value = 0; //吸取器推进器的值

/*******************************************
* 函 数 名：Propeller_Init
* 功    能：推进器的初始化
* 输入参数：none
* 返 回 值：none
* 注    意：初始化流程：
*           1,接线,上电，哔-哔-哔三声,表示开机正常
*           2,给电调2ms或1ms最高转速信号,哔一声
*           3,给电调1.5ms停转信号,哔一声
*           4,初始化完成，可以开始控制
********************************************/
void Propeller_Init(void) //这边都需要经过限幅在给定  原先为2000->1500
{
	// 初始化推进器
	I2C_PWM_SetPWM(0, 0, PropellerPower_Max); //最高转速信号  水平推进器1号  右上
	I2C_PWM_SetPWM(5, 0, PropellerPower_Max); //最高转速信号  水平推进器2号  左下
	I2C_PWM_SetPWM(3, 0, PropellerPower_Max); //最高转速信号  水平推进器3号  左上
	I2C_PWM_SetPWM(2, 0, PropellerPower_Max); //最高转速信号  水平推进器4号  右下

	I2C_PWM_SetPWM(4, 0, PropellerPower_Max); //最高转速信号  垂直推进器1号  左中
	I2C_PWM_SetPWM(1, 0, PropellerPower_Max); //最高转速信号  垂直推进器2号  右中

	I2C_PWM_SetPWM(10, 0, PropellerPower_Max); //机械臂

	delay(2000); //2s

	I2C_PWM_SetPWM(0, 0, PropellerPower_Med); //停转信号
	I2C_PWM_SetPWM(5, 0, PropellerPower_Med); //停转信号
	I2C_PWM_SetPWM(3, 0, PropellerPower_Med); //停转信号
	I2C_PWM_SetPWM(2, 0, PropellerPower_Med); //停转信号

	I2C_PWM_SetPWM(4, 0, PropellerPower_Med); //停转信号
	I2C_PWM_SetPWM(1, 0, PropellerPower_Med); //停转信号

	I2C_PWM_SetPWM(10, 0, PropellerPower_Med); //机械臂

	// TODO 云台怎么初始化
	// TIM4_PWM_CH3_D14(1500); //机械臂中值 1000~2000
	// TIM4_PWM_CH4_D15(2000); //云台中值

	delay(1000);

	Propeller_Init_Flag = 1;
	log_i("Propeller_init()");
}

void PWM_Update(PropellerPower_Type *propeller)
{
	power_test.rightUp = PropellerPower_Med + propeller->rightUp;
	power_test.leftDown = PropellerPower_Med + propeller->leftDown;
	power_test.leftUp = PropellerPower_Med + propeller->leftUp;
	power_test.rightDown = PropellerPower_Med + propeller->rightDown;

	power_test.leftMiddle = PropellerPower_Med + propeller->leftMiddle;
	power_test.rightMiddle = PropellerPower_Med + propeller->rightMiddle;

	if (1 == Propeller_Init_Flag)
	{

		I2C_PWM_SetPWM(0, 0, power_test.rightUp);   // 水平推进器1号  右上
		I2C_PWM_SetPWM(5, 0, power_test.leftDown);  // 水平推进器2号  左下
		I2C_PWM_SetPWM(3, 0, power_test.leftUp);	// 水平推进器3号  左上
		I2C_PWM_SetPWM(2, 0, power_test.rightDown); // 水平推进器4号  右下

		I2C_PWM_SetPWM(4, 0, power_test.leftMiddle);  // 垂直推进器1号  左中
		I2C_PWM_SetPWM(1, 0, power_test.rightMiddle); // 垂直推进器2号  右中
	}
}

int power_value = 1500;
/**
  * @brief  Extractor_Control(吸取器控制)
  * @param  控制指令 0x00：不动作  0x01：吸取  0x02：关闭
  * @retval None
  * @notice 
  */
void Extractor_Control(uint8 *action)
{
	switch (*action)
	{
	case 0x01:
		power_value = Extractor_Value; //设定吸力值
		break;
	case 0x02:
		power_value = PropellerPower_Med; //推进器中值 停转
		break;
	default:
		break;
	}
	//TIM4_PWM_CH3_D14(power_value);
}
