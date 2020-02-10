#define LOG_TAG "adc"

#include "drv_adc.h"
#include "../easylogger/inc/elog.h"

#include "../Tools/filter.h"

#include <stdio.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

// #define FSR (6.144f) // 满量程：正负 6.144 V
// #define LSB (187.5f) // 最低有效位： 187.5 uV

#define FSR (4.096f) // 满量程：正负 4.096 V
#define LSB (125.0f) // 最低有效位： 125.0 uV

static int adc_fd = 0;

uint8 i = 0;
float voltage = 0.0f;
uint32 adc_value[10] = {0};
/* 冒泡 get电压 */
float get_voltage_value(void)
{
    for (i = 0; i < 10; i++)
    {
        adc_value[i] = get_adc1(); //采样
    }
    voltage = Bubble_Filter(adc_value) * LSB / 1000 / 1000;

    return voltage;
}

uint8 j = 0;
uint32 adc1_value[10] = {0};
float voltage1 = 0.0f, current = 0.0f;

/* 冒泡 get电流 */
float get_current_value(void)
{
    // TODO

    // for (j = 0; j < 10; j++)
    // {
    //     rt_thread_mdelay(10);
    //     adc1_value[j] = get_adc2(ADC_Channel_11); //采样
    // }
    // voltage1 = Bubble_Filter(adc1_value) * REFER_VOLTAGE / CONVERT_BITS; //电压计算公式：voltage = adc采样值 * 采样精度(3.3V/4096)
    // current = voltage1;                                                  // Is = Vo*1K/(Rs*Rl) = (Vo*1K)/(0.01*100K) = (1*Vo)详细见INA16 Datasheet
    // return current;

    return 0.0;
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

//初始化ADC
int adc_init(void)
{
    adc_fd = wiringPiSPISetupMode(1, 1000000, 1); //1MHz
    if (adc_fd < 0)
    {
        log_e("adc_init failed");
    }

    log_d("adc_fd:%d", adc_fd);
    return adc_fd;
}
