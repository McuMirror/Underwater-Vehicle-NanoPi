/*
 * @Description: 
 * @Author: chenxi
 * @Date: 2020-01-01 13:06:46
 * @LastEditTime : 2020-02-04 13:21:02
 * @LastEditors  : chenxi
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "DataType.h"

#include "..//applications//gyroscope.h"
#include "..//applications//IIC_PWM.h"
#include "..//applications//my_debug.h"
#include "..//drivers//drv_spl1301.h"
#include "..//drivers//drv_oled.h"
#include "..//applications//led.h"
#include "..//applications//ioDevices.h"

#include <wiringPi.h>
#include <wiringSerial.h>
#include <wiringPiI2C.h>
#include <wiringPiSPI.h>

extern Sensor_Type Sensor;

int JY901_Serial()
{
  int fd;

  if ((fd = serialOpen("/dev/ttyS2", 115200)) < 0)
  {
    fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
    return 1;
  }
  if (Nano_DEBUG)
  {
    printf("JY901_Serial fd:%d\n", fd);
  }

  while (1)
  {
    delay(1000);
    while (serialDataAvail(fd))
    {
      CopeSerial2Data((uint8)serialGetchar(fd));
      // printf("%x", serialGetchar(fd));
    }
  }
  return 0;
}

void I2C_PWM_Test()
{
  int fd = I2C_PWM_Init("/dev/i2c-0", I2C_PWM_Addr);
  // I2C_PWM_SetPWMFreq(50.0);

  int i = 0;
  while (1)
  {
    I2C_PWM_SetPWM(0, i, 4096 - i);
    if (i <= 4096)
      i += 32;
    else
      i = 0;
    delay(1000);
  }
}

void I2C_spl1301_Test()
{
  spl1301_init("/dev/i2c-1");
  while (1)
  {
    spl1301_get_raw_temp();
    printf("temperature:%f\n", get_spl1301_temperature());
    spl1301_get_raw_pressure();
    printf("pressure:%f\n", get_spl1301_pressure());
    printf("\n");
    delay(1000);
  }
}

void SPI_Test()
{
  int fd;
  fd = wiringPiSPISetupMode(1, 1000000, 1); //100MHz
  if (Nano_DEBUG)
  {
    printf("SPI fd:%d\n", fd);
  }

  char data[] = {0, 0x8A, 0, 0, 0, 0x8A, 0, 0, 0, 0};
  printf("write %d\n", wiringPiSPIDataRW(1, data, 8));
  for (int i = 0; i < 9; i++)
  {
    printf("%x ", data[i]);
  }
  printf("\n");
}

int main()
{
  printf("\n");

  if (wiringPiSetup() == -1)
  {
    fprintf(stdout, "Unable to start wiringPi: %s\n", strerror(errno));
    return 1;
  }

  // JY901_Serial();

  return 0;
}
