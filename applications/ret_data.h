/*
 * @Description: 处理需要发送的数据
 * @Author: chenxi
 * @Date: 2020-02-17 11:43:59
 * @LastEditTime: 2020-02-17 11:44:00
 * @LastEditors: chenxi
 */

#ifndef __RETURN_DATA_
#define __RETURN_DATA_

#include "../user/DataType.h"

#define Send_Date_Len 22 //数据包长度，不包括包头、校验字

void Convert_Return_Computer_Data(uint8 *Return_Data); // 转换需要返回上位机数据
uint8 Calculate_Check_Byte(uint8 *begin_buff, uint8 *buff, uint8 len);

extern uint8 device_hint_flag; //设备提示字符
extern uint8 begin_buff[3];
extern uint8 Return_Data[Send_Date_Len];

#endif
