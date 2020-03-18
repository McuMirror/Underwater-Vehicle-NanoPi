/*
 * @Description: ����ģ��
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

#define JY901_PACKET_LENGTH 11 //���ݰ�����

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

short Compass_Offset_Angle = 0;//ָ���벹���Ƕ�   �����ܵ����Ӵų����ţ���Ҫ��һ�������Ƕ�  -360 ~ +360

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

// CopeSerialDataΪ����2�жϵ��ú���������ÿ�յ�һ�����ݣ�����һ�����������
void CopeSerial2Data(uint8 Data)
{
	static uint8 rxBuffer[20] = {0}; // ���ݰ�
	static uint8 rxCheck = 0;		 // βУ����
	static uint8 rxCount = 0;		 // ���ռ���
	static uint8 i = 0;				 // ���ռ���

	rxBuffer[rxCount++] = Data; // ���յ������ݴ��뻺������

	if (rxBuffer[0] != 0x55)
	{
		// ����ͷ���ԣ������¿�ʼѰ��0x55����ͷ
		rxCount = 0; // ��ջ�����
		return;
	}
	if (rxCount < JY901_PACKET_LENGTH)
	{
		return;
	} // ���ݲ���11�����򷵻�

	/*********** ֻ�н�����11���ֽ����� �Ż�������³��� ************/
	for (i = 0; i < 10; i++)
	{
		rxCheck += rxBuffer[i]; //У��λ�ۼ�
	}

	if (rxCheck == rxBuffer[JY901_PACKET_LENGTH - 1]) // �ж����ݰ�У���Ƿ���ȷ
	{
		// �ж��������������ݣ�Ȼ���俽������Ӧ�Ľṹ���У���Щ���ݰ���Ҫͨ����λ���򿪶�Ӧ������󣬲��ܽ��յ�������ݰ�������
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
		rxCount = 0; // ��ջ�����
		rxCheck = 0; // У��λ����

		jy901_convert(&Sensor.JY901); // JY901����ת��
	}
	else // ��������
	{
		rxCount = 0; // ��ջ�����
		rxCheck = 0; // У��λ����
		return;
	}
}

// Sensor.JY901 ����ת��
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

	// ƫ�ƽǶ� ���ڵ�ָ�� ����ʱ�ĽǶ�(-360 ~ +360 )
	if (Compass_Offset_Angle != 0) // ���δ���ò����Ƕȣ��򲻽��нǶȲ���������Ϊ ���Ƕȡ�
	{
		pArr->Euler.Yaw -= Compass_Offset_Angle; // ��ȥ�����Ƕ�
		if (pArr->Euler.Yaw < -180){
			pArr->Euler.Yaw += 360; // �Ƕȷ��򲹳�
		}
		if (pArr->Euler.Yaw > 180){
			pArr->Euler.Yaw -= 360; // �Ƕȷ��򲹳�
		}
	}

	pArr->Mag.x = stcMag.h[0];
	pArr->Mag.y = stcMag.h[1];
	pArr->Mag.z = stcMag.h[2];

	pArr->Temperature = (float)stcAcc.T / 100;
}

