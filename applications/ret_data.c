/*
 * @Description: 处理需要发送的数据
 * @Author: chenxi
 * @Date: 2020-02-17 11:43:52
 * @LastEditTime: 2020-02-17 11:43:53
 * @LastEditors: chenxi
 */

#define LOG_TAG "ret_data"

#include "../easylogger/inc/elog.h"
#include "ret_data.h"
#include "sensor.h"

extern Sensor_Type Sensor;

uint8 device_hint_flag = 0x0; //设备提示字符
uint8 begin_buff[3] = {0xAA, 0x55, 0x16};
uint8 Return_Data[Send_Date_Len];

/**
  * @brief  get_decimal(得到浮点型数据头两位小数的100倍)
  * @param  浮点型数据 data
  * @retval 头两位小数的100倍
  * @notice 
  */
uint8 get_decimal(float data)
{
    return (uint8)((float)(data - (int)data) * 100);
}

/**********************   发送到PC端   ***********************/
void Convert_Return_Computer_Data(uint8 *Return_Data) // 转换需要返回上位机数据
{
    static short temp_Roll = 0; //暂存数据
    static short temp_Pitch = 0;
    static short temp_Yaw = 0;

    temp_Roll = (short)((Sensor.JY901.Euler.Roll + 180) * 100); //数据转换:将角度数据转为正值并放大100倍
    temp_Pitch = (short)((Sensor.JY901.Euler.Pitch + 180) * 100);
    temp_Yaw = (short)((Sensor.JY901.Euler.Yaw + 180) * 100);

    Return_Data[0] = (int)Sensor.PowerSource.Voltage;         //整数倍
    Return_Data[1] = get_decimal(Sensor.PowerSource.Voltage); //小数的100倍

    Return_Data[2] = (int)Sensor.CPU.Temperature;         //整数倍
    Return_Data[3] = get_decimal(Sensor.CPU.Temperature); //小数的100倍

    Return_Data[4] = (int)Sensor.DepthSensor.Temperature;         //整数倍
    Return_Data[5] = get_decimal(Sensor.DepthSensor.Temperature); //小数的100倍

    Return_Data[6] = (int)(Sensor.DepthSensor.Depth) >> 16; //高8位
    Return_Data[7] = (int)(Sensor.DepthSensor.Depth) >> 8;  //中8位
    Return_Data[8] = (int)(Sensor.DepthSensor.Depth);       //低8位

    Return_Data[9] = temp_Yaw >> 8;    // Yaw 高8位
    Return_Data[10] = (uint8)temp_Yaw; //低8位

    Return_Data[11] = temp_Pitch >> 8;   // Pitch 高8位
    Return_Data[12] = (uint8)temp_Pitch; //低8位

    Return_Data[13] = temp_Roll >> 8;   // Roll 高8位
    Return_Data[14] = (uint8)temp_Roll; //低8位

    Return_Data[15] = (uint8)10;        //x轴航速
    Return_Data[16] = device_hint_flag; //设备提示字符

    Return_Data[17] = 0x01; // ControlCmd.All_Lock;

    Return_Data[18] = (int)Sensor.PowerSource.Current;
    Return_Data[19] = get_decimal(Sensor.PowerSource.Current); //小数的100倍;

    Return_Data[20] = 0x0; // 保留
    Return_Data[21] = 0x0; // 保留
}

/**
  * @brief  Calculate_Check_Byte(计算得到最后校验位)
  * @param  起始包头*begin_buff、数据包*buff、数据包长度len
  * @retval None
  * @notice 
  */
uint8 Calculate_Check_Byte(uint8 *begin_buff, uint8 *buff, uint8 len)
{
    uint8 Check_Byte = 0;
    uint8 i = 0;
    for (i = 0; i < 3; i++)
    {
        Check_Byte += begin_buff[i];
    }
    for (i = 0; i < len; i++)
    {
        Check_Byte += buff[i];
    }
    return Check_Byte;
}
