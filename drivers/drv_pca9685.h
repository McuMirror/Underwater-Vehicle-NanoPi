/*
 * @Description: 通过 I2C 控制 PCA9685 来使用 PWM 功能
 * @Author: chenxi
 * @Date: 2020-02-10 12:15:34
 * @LastEditTime: 2020-03-15 12:52:46
 * @LastEditors: chenxi
 */

#ifndef __PWM_H__
#define __PWM_H__

#include "../user/DataType.h"

#define PCA9685_I2C    "/dev/i2c-0" // PCA9685 使用的 I2C设备
#define PCA9685_ADDR   0x40         // 默认 PCA9685 地址
#define PCA9685_OE_PIN 7            // GPIO11引脚序号为7

#define PCA9685_SUBADR1 0x2
#define PCA9685_SUBADR2 0x3
#define PCA9685_SUBADR3 0x4

#define PCA9685_MODE1 0x0
#define PCA9685_PRESCALE 0xFE

#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9
#define PIN_ALL    16
#define LEDALL_ON_L 0xFA

#define ALLLED_ON_L 0xFA
#define ALLLED_ON_H 0xFB
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD

int pca9685Init(void);
void pca9685PWMReset(void);
void pca9685PWMFreq(float freq);
void pca9685PWMWrite(uint16 pin, uint32 on, uint32 off);

#endif
