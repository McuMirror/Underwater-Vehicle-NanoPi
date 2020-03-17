/*
 * @Description: 处理需要发送的数据
 * @Author: chenxi
 * @Date: 2020-02-17 11:43:59
 * @LastEditTime: 2020-02-17 11:44:00
 * @LastEditors: chenxi
 */

#ifndef __RET_DATA_
#define __RET_DATA_

#include "../user/DataType.h"

#define RETURN_DATA_LEN 26

uint8 calculate_check_sum(uint8 *buff, uint8 len);
void convert_rov_data(uint8 *buff); // 转换需要返回上位机数据

extern uint8 device_hint_flag; //设备提示字符

#endif
