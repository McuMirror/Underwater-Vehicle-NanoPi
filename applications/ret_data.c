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

uint8 device_hint_flag = 0x0; //设备提示字符


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

/**
  * @brief  calculate_check_sum(计算校验和)
  * @param  起始包头*begin_buff、数据包*buff、数据包长度len
  * @retval None
  * @notice 
  */
uint8 calculate_check_sum(uint8 *buff, uint8 len)
{
    uint8 sum = 0;
    for (int i = 0; i < len; i++)
    {
        sum += buff[i];
    }
    return sum;
}

/**********************   发送到PC端   ***********************/
void convert_rov_data(uint8 *buff) // 转换需要返回上位机数据
{
    short troll; //暂存数据
    short tpitch;
    short tyaw;

    troll = (short)((Sensor.JY901.Euler.Roll + 180) * 100); //数据转换:将角度数据转为正值并放大100倍
    tpitch = (short)((Sensor.JY901.Euler.Pitch + 180) * 100);
    tyaw = (short)((Sensor.JY901.Euler.Yaw + 180) * 100);

    buff[3] = (int)Sensor.PowerSource.Voltage;         //整数倍
    buff[4] = get_decimal(Sensor.PowerSource.Voltage); //小数的100倍

    buff[5] = (int)Sensor.CPU.Temperature;         //整数倍
    buff[6] = get_decimal(Sensor.CPU.Temperature); //小数的100倍

    buff[7] = (int)Sensor.DepthSensor.Temperature;         //整数倍
    buff[8] = get_decimal(Sensor.DepthSensor.Temperature); //小数的100倍

    buff[9] = (int)(Sensor.DepthSensor.Depth) >> 16; //高8位
    buff[10] = (int)(Sensor.DepthSensor.Depth) >> 8;  //中8位
    buff[11] = (int)(Sensor.DepthSensor.Depth);       //低8位

    buff[12] = tyaw >> 8;    // Yaw 高8位
    buff[13] = (uint8)tyaw; //低8位

    buff[14] = tpitch >> 8;   // Pitch 高8位
    buff[15] = (uint8)tpitch; //低8位

    buff[16] = troll >> 8;   // Roll 高8位
    buff[17] = (uint8)troll; //低8位


    buff[18] = (uint8)0;        //x轴航速
    buff[19] = device_hint_flag; //设备提示字符

    buff[20] = 0x01; // ControlCmd.All_Lock;

    buff[21] = (int)Sensor.PowerSource.Current;
    buff[22] = get_decimal(Sensor.PowerSource.Current); //小数的100倍;

    buff[23] = 0x0; // 保留
    buff[24] = 0x0; // 保留

    buff[25] = calculate_check_sum(buff, RETURN_DATA_LEN - 1);//获取校验和
}

