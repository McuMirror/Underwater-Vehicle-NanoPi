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

#define RETURN_DATA_LEN 26

void convert_return_computer_data(uint8 *buff); // 转换需要返回上位机数据
uint8 calculate_check_sum(uint8 *buff, uint8 len);

extern uint8 device_hint_flag; //设备提示字符

#endif
