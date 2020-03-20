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

static int fd;


/**
 * Helper function to get to register
 */
int baseReg(int pin)
{
  // 计算获得对应引脚寄存器地址(见datasheet P9)
	return (pin >= PIN_ALL ? LEDALL_ON_L : LED0_ON_L + 4 * pin);
}

/**
 * @description: 初始化 I2C 和 PWM
 * @param : 
 * @return: 文件描述符 fd
 */
int pca9685Init(void)
{
  // PCA9685_OE_PIN，即 GPIOG11 低电平使能
  pinMode(PCA9685_OE_PIN, OUTPUT);
  digitalWrite(PCA9685_OE_PIN, LOW);

  fd = wiringPiI2CSetupInterface(PCA9685_I2C, PCA9685_ADDR);
  if (fd < 0)
  {
    log_e("pca9685 init failed");
  }
  log_i("pca9685 init");

  pca9685PWMSetFreq(50);

  pca9685PWMReset();
  return fd;
}

void pca9685PWMReset(void)
{
  wiringPiI2CWriteReg8(fd, PCA9685_MODE1, 0x0);
}

void pca9685PWMSetFreq(float freq)
{
	// Cap at min and max
	freq = (freq > 1000 ? 1000 : (freq < 40 ? 40 : freq));

  /*
   * To set pwm frequency we have to set the prescale register. The formula is:
   * prescale = round(osc_clock / (4096 * frequency))) - 1 where osc_clock = 25 MHz
   */
  int prescale = (int)(PCA9685_OSC_CLK / (4096 * freq) - 0.5f);
  log_i("pwm set %d Hz", freq);

	// Get settings and calc bytes for the different states.
	int settings = wiringPiI2CReadReg8(fd, PCA9685_MODE1) & 0x7F;	// Set restart bit to 0
	int sleep	= settings | 0x10;									// Set sleep bit to 1   (enable  sleep)
	int wake 	= settings & 0xEF;									// Set sleep bit to 0   (disable sleep)
	int restart = wake | 0x80;										// Set restart bit to 1 (enable restart)

	// Go to sleep, set prescale and wake up again.
	wiringPiI2CWriteReg8(fd, PCA9685_MODE1, sleep);
	wiringPiI2CWriteReg8(fd, PCA9685_PRESCALE, prescale);
	wiringPiI2CWriteReg8(fd, PCA9685_MODE1, wake);

	// Now wait 50 ms until oscillator finished stabilizing and restart PWM.
	delay(50);
	wiringPiI2CWriteReg8(fd, PCA9685_MODE1, restart);
}

void pca9685PWMWrite(uint16 pin, uint32 on, uint32 off)
{
  int reg = baseReg(pin);

	// Write to on and off registers and mask the 12 lowest bits of data to overwrite full-on and off
  // because max value = 4096 = 2^12
	wiringPiI2CWriteReg16(fd, reg	   , on  & 0x0FFF);
	wiringPiI2CWriteReg16(fd, reg + 2, off & 0x0FFF);

  log_d("SetPWM %2d, on %4d, off %4d", pin, on, off);
}


void pca9685PWMRead(int fd, int pin, int *on, int *off)
{
	int reg = baseReg(pin);

	if (on)
		*on  = wiringPiI2CReadReg16(fd, reg);
	if (off)
		*off = wiringPiI2CReadReg16(fd, reg + 2);
}