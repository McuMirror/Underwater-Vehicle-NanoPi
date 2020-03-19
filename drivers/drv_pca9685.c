/*
 * @Description: 通过 I2C 控制 PCA9685 来使用 PWM 功能
 * @Author: chenxi
 * @Date: 2020-02-10 12:15:34
 * @LastEditTime: 2020-03-18 10:53:56
 * @LastEditors: chenxi
 */

#define LOG_TAG "pwm"

#include "../easylogger/inc/elog.h"
#include "drv_pca9685.h"

#include <wiringPi.h>
#include <wiringPiI2C.h>

static int pca9685_fd;


/**
 * Helper function to get to register
 */
int baseReg(int pin)
{
	return (pin >= PIN_ALL ? LEDALL_ON_L : LED0_ON_L + 4 * pin);
}

/**
 * @description: 初始化 I2C 和 PWM
 * @param : 
 * @return: 文件描述符 pca9685_fd
 */
int pca9685Init(void)
{
  // PCA9685_OE_PIN，即 GPIOG11 低电平使能
  pinMode(PCA9685_OE_PIN, OUTPUT);
  digitalWrite(PCA9685_OE_PIN, LOW);

  pca9685_fd = wiringPiI2CSetupInterface(PCA9685_I2C, PCA9685_ADDR);
  log_d("pca9685_fd:%d", pca9685_fd);

  if (pca9685_fd < 0)
  {
    log_e("pca9685 init failed");
  }
  log_i("pca9685 init");

  pca9685PWMReset();
  return pca9685_fd;
}

void pca9685PWMReset(void)
{
  wiringPiI2CWriteReg8(pca9685_fd, PCA9685_MODE1, 0x0);
}

void pca9685PWMFreq(float freq)
{
	// Cap at min and max
	freq = (freq > 1000 ? 1000 : (freq < 40 ? 40 : freq));

  /*
   * To set pwm frequency we have to set the prescale register. The formula is:
   * prescale = round(osc_clock / (4096 * frequency))) - 1 where osc_clock = 25 MHz
   */
  int prescale = (int)(25000000.0f / (4096 * freq) - 0.5f);
  log_i("pwm set %d Hz", freq);

	// Get settings and calc bytes for the different states.
  uint16 oldmode = wiringPiI2CReadReg8(pca9685_fd, PCA9685_MODE1);
  uint16 newmode = (oldmode & 0x7F) | 0x10;                     // sleep

  wiringPiI2CWriteReg8(pca9685_fd, PCA9685_MODE1, newmode);     // go to sleep
  wiringPiI2CWriteReg8(pca9685_fd, PCA9685_PRESCALE, prescale); // set the prescaler

  delay(50);
  wiringPiI2CWriteReg8(pca9685_fd, PCA9685_MODE1, oldmode);
  delay(50);
  wiringPiI2CWriteReg8(pca9685_fd, PCA9685_MODE1, oldmode | 0xa1); // This sets the MODE1 register to turn on auto increment.

}

void pca9685PWMWrite(uint16 pin, uint32 on, uint32 off)
{
  int reg = baseReg(pin);

  wiringPiI2CWriteReg8(pca9685_fd, reg, on);
  wiringPiI2CWriteReg8(pca9685_fd, reg + 1, on >> 8);
  wiringPiI2CWriteReg8(pca9685_fd, reg + 2, off);
  wiringPiI2CWriteReg8(pca9685_fd, reg + 3, off >> 8);

  log_d("SetPWM %2d, on %4d, off %4d", pin, on, off);
}
