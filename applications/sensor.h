/*
 * @Description: 
 * @Author: chenxi
 * @Date: 2020-02-09 12:30:19
 * @LastEditTime : 2020-02-09 16:15:31
 * @LastEditors  : chenxi
 */
#ifndef __SENSOR_H_
#define __SENSOR_H_

#include "../user/DataType.h"

int sensor_thread_init(void);
void print_sensor_info(void);         // 打印传感器信息
void Depth_Sensor_Data_Convert(void); // 深度传感器数据转换

extern char *Depth_Sensor_Name[3];

#endif
