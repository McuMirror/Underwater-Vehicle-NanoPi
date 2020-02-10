/*
 * @Description: 
 * @Author: chenxi
 * @Date: 2020-02-09 17:25:35
 * @LastEditTime : 2020-02-10 10:50:39
 * @LastEditors  : chenxi
 */
#ifndef __ADC_CONVERT_H_
#define __ADC_CONVERT_H_

#include "../user/DataType.h"

//初始化ADC
int adc_init(void);

uint16 get_adc0(void);         //get adcin0通道值
uint16 get_adc1(void);         //get adcin1通道值
float get_voltage_value(void); // 获取电压值
float get_current_value(void); // 获取电流值

#endif
