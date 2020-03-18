/*
 * @Description: 九轴模块
 * @Author: chenxi
 * @Date: 2020-02-10 12:15:34
 * @LastEditTime: 2020-03-17 09:23:19
 * @LastEditors: chenxi
 */

#define LOG_TAG "gyro"

#include "../easylogger/inc/elog.h"
#include "gyroscope.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

#include <wiringPi.h>
#include <wiringSerial.h>

extern Sensor_Type Sensor;
static int jy901_fd = 0;

/*---------------------- Constant / Macro Definitions -----------------------*/

#define JY901_PACKET_LENGTH 11 //数据包长度

/*----------------------- Variable Declarations -----------------------------*/
struct STime stcTime;
struct SAcc stcAcc;
struct SGyro stcGyro;
struct SAngle stcAngle;
struct SMag stcMag;
struct SDStatus stcDStatus;
struct SPress stcPress;
struct SLonLat stcLonLat;
struct SGPSV stcGPSV;
struct SQ stcQ;

short Compass_Offset_Angle = 0;//指南针补偿角度   由于受到板子磁场干扰，需要加一个补偿角度  -360 ~ +360

/*----------------------- Function Implement --------------------------------*/

int jy901_init(void)
{
	if ((jy901_fd = serialOpen("/dev/ttyS2", 115200)) < 0)
	{
		log_e("Unable to open serial device: %s", strerror(errno));
		return -1;
	}
	log_i("jy901_init\n");
	log_d("jy901_fd:%d", jy901_fd);

	if (jy901_fd < 0)
	{
		log_e("JY901 init failed");
	}
	return jy901_fd;
}

// CopeSerialData为串口2中断调用函数，串口每收到一个数据，调用一次这个函数。
void CopeSerial2Data(uint8 Data)
{
	static uint8 rxBuffer[20] = {0}; // 数据包
	static uint8 rxCheck = 0;		 // 尾校验字
	static uint8 rxCount = 0;		 // 接收计数
	static uint8 i = 0;				 // 接收计数

	rxBuffer[rxCount++] = Data; // 将收到的数据存入缓冲区中

	if (rxBuffer[0] != 0x55)
	{
		// 数据头不对，则重新开始寻找0x55数据头
		rxCount = 0; // 清空缓存区
		return;
	}
	if (rxCount < JY901_PACKET_LENGTH)
	{
		return;
	} // 数据不满11个，则返回

	/*********** 只有接收满11个字节数据 才会进入以下程序 ************/
	for (i = 0; i < 10; i++)
	{
		rxCheck += rxBuffer[i]; //校验位累加
	}

	if (rxCheck == rxBuffer[JY901_PACKET_LENGTH - 1]) // 判断数据包校验是否正确
	{
		// 判断数据是哪种数据，然后将其拷贝到对应的结构体中，有些数据包需要通过上位机打开对应的输出后，才能接收到这个数据包的数据
		switch (rxBuffer[1])
		{
		case 0x50:
			memcpy(&stcTime, &rxBuffer[2], 8);
			break;
		case 0x51:
			memcpy(&stcAcc, &rxBuffer[2], 8);
			break;
		case 0x52:
			memcpy(&stcGyro, &rxBuffer[2], 8);
			break;
		case 0x53:
			memcpy(&stcAngle, &rxBuffer[2], 8);
			break;
		case 0x54:
			memcpy(&stcMag, &rxBuffer[2], 8);
			break;
		case 0x55:
			memcpy(&stcDStatus, &rxBuffer[2], 8);
			break;
		case 0x56:
			memcpy(&stcPress, &rxBuffer[2], 8);
			break;
		case 0x57:
			memcpy(&stcLonLat, &rxBuffer[2], 8);
			break;
		case 0x58:
			memcpy(&stcGPSV, &rxBuffer[2], 8);
			break;
		case 0x59:
			memcpy(&stcQ, &rxBuffer[2], 8);
			break;
		}
		rxCount = 0; // 清空缓存区
		rxCheck = 0; // 校验位清零

		jy901_convert(&Sensor.JY901); // JY901数据转换
	}
	else // 错误清零
	{
		rxCount = 0; // 清空缓存区
		rxCheck = 0; // 校验位清零
		return;
	}
}

// Sensor.JY901 数据转换
void jy901_convert(JY901_Type *pArr)
{
	pArr->Acc.x = (float)stcAcc.a[0] / 2048; // 32768*16
	pArr->Acc.y = (float)stcAcc.a[1] / 2048;
	pArr->Acc.z = (float)stcAcc.a[2] / 2048;

	pArr->Gyro.x = (float)stcGyro.w[0] / 2048 * 125; // 32768*2000
	pArr->Gyro.y = (float)stcGyro.w[1] / 2048 * 125;
	pArr->Gyro.z = (float)stcGyro.w[2] / 2048 * 125;

	pArr->Euler.Roll = (float)stcAngle.angle[0] / 8192 * 45; // 32768*180;
	pArr->Euler.Pitch = (float)stcAngle.angle[1] / 8192 * 45;
	pArr->Euler.Yaw = (float)stcAngle.angle[2] / 8192 * 45;

	// 偏移角度 等于当指向 正北时的角度(-360 ~ +360 )
	if (Compass_Offset_Angle != 0) // 如果未设置补偿角度，则不进行角度补偿【补偿为 正角度】
	{
		pArr->Euler.Yaw -= Compass_Offset_Angle; // 减去补偿角度
		if (pArr->Euler.Yaw < -180){
			pArr->Euler.Yaw += 360; // 角度反向补偿
		}
		if (pArr->Euler.Yaw > 180){
			pArr->Euler.Yaw -= 360; // 角度反向补偿
		}
	}

	pArr->Mag.x = stcMag.h[0];
	pArr->Mag.y = stcMag.h[1];
	pArr->Mag.z = stcMag.h[2];

	pArr->Temperature = (float)stcAcc.T / 100;
}

