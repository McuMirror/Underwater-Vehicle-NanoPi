/*
 * @Description: 获取电压、电流
 * @Author: chenxi
 * @Date: 2020-02-09 17:25:35
 * @LastEditTime : 2020-02-10 17:09:28
 * @LastEditors  : chenxi
 */

#define LOG_TAG "drv_adc"

#include "../easylogger/inc/elog.h"
#include "../Tools/filter.h"
#include "drv_ads1118.h"

#include <stdio.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

// #define FSR (6.144f) // 满量程：正负 6.144 V
// #define LSB (187.5f) // 最低有效位： 187.5 uV

#define FSR (4.096f) // 满量程：正负 4.096 V
#define LSB (125.0f) // 最低有效位： 125.0 uV

static int fd = 0;
static ads1118_t *ads1118;
static double vref[] = {6.144, 4.096, 2.048, 1.024, 0.512, 0.256};

uint8 i = 0;
float voltage1 = 0.0f;
uint32 adc1_value[10] = {0};

// 冒泡获取电压
float get_voltage_value(void)
{
    for (i = 0; i < 10; i++)
    {
        adc1_value[i] = get_adc1(); // 采样
    }
    voltage1 = Bubble_Filter(adc1_value) * LSB / 1000 / 1000;

    return voltage1;
}

uint8 j = 0;
uint32 adc0_value[10] = {0};
float voltage0 = 0.0f, current = 0.0f;

// 冒泡获取电流
float get_current_value(void)
{
    for (j = 0; j < 10; j++)
    {
        adc0_value[j] = get_adc0(); // 采样
    }
    voltage0 = Bubble_Filter(adc0_value) * LSB / 1000 / 1000;
    current = voltage0;
    return current;
}

// AINP 为AIN1 且AINN 为GND
// FSR = 正负 6.144 V
// 写入：0xD18A
// FSR = 正负 4.096 V
// 写入：0xD38A
uint16 get_adc1(void)
{
    uint8 data[] = {0xD3, 0x8A, 0, 0};

    wiringPiSPIDataRW(1, data, 4);
    delay(50);
    data[0] = 0xD3;
    data[1] = 0x8A;
    data[2] = 0;
    data[3] = 0;
    wiringPiSPIDataRW(1, data, 4);

    return (data[0] << 8) + data[1];
}

// AINP 为AIN0 且AINN 为GND
// FSR = 正负 6.144 V
// 写入：0xC18A
// FSR = 正负 4.096 V
// 写入：0xC38A
uint16 get_adc0(void)
{
    uint8 data[] = {0xC3, 0x8A, 0, 0};

    wiringPiSPIDataRW(1, data, 4);
    delay(50);
    data[0] = 0xC3;
    data[1] = 0x8A;
    data[2] = 0;
    data[3] = 0;
    wiringPiSPIDataRW(1, data, 4);

    return (data[0] << 8) + data[1];
}
//-------------------------------------------------------------

static int16_t ads1118_transmit(uint8_t *data)
{
    unsigned char buff[4] = {0};

    buff[0] = data[1];
    buff[1] = data[0];
    buff[2] = 0;
    buff[3] = 0;

    wiringPiSPIDataRW(1, rx_buff, 4);

    return buff[0] << 8 | buff[1]; // SPI传输高位在前
}

void ads1118_convert_data(uint8 channel)
{
    switch (channel) {
    case 0:
        ads1118->bits.mux = MUX_S0;
        break;
    case 1:
        ads1118->bits.mux = MUX_S1;
        break;
    case 2:
        ads1118->bits.mux = MUX_S2;
        break;
    case 3:
        ads1118->bits.mux = MUX_S3;
        break;
    default:
        return;
    }

    ads1118_transmit((uint8_t *)ads1118);
}


int16_t ads1118_convert(void)
{
    return ads1118_transmit((uint8_t *)"\x00\x00");
}

// 初始化ADC
int ads1118_init(void)
{
    fd = wiringPiSPISetupMode(1, 1000000, 1); //1MHz
    if (fd < 0)
    {
        log_e("ads1118 init failed");
    }
    ads1118->bits.ss = SS_NONE;               // 不启动单发转换
    ads1118->bits.mux = MUX_S0;               // 通道 AIN0
    ads1118->bits.pga = PGA_2_048;            // 可编程增益放大 ±2.048v
    ads1118->bits.mode = MODE_CONTINOUS;      // 连续转换模式
    ads1118->bits.dr = DR_128_SPS;            // 每秒采样率 128
    ads1118->bits.ts_mode = TS_MODE_ADC;      // ADC 模式
    ads1118->bits.pull_up_en = PULL_UP_EN_ON; // 数据引脚上拉使能
    ads1118->bits.nop = NOP_DATA_VALID;       // 有效数据,更新配置寄存器
    ads1118->bits.reserved = RESERVED_BIT;    // 保留位

    ads1118_transmit((uint8_t *)ads1118); // 修改ads1118 配置寄存器
    return fd;
}
