/*
 * @Description: 主程序
 * @Author: chenxi
 * @Date: 2020-01-01 13:06:46
 * @LastEditTime: 2020-03-18 20:39:05
 * @LastEditors: chenxi
 */

#define LOG_TAG "main"

#include "../easylogger/inc/elog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "DataType.h"
#include "DeviceThread.h"

#include "../applications/pwm.h"
#include "../applications/sensor.h"
#include "../applications/server.h"
#include "../applications/gyroscope.h"
#include "../applications/propeller.h"

#include <wiringPi.h>

void easyloggerInit(void)
{
  setbuf(stdout, NULL);
  /* initialize EasyLogger */
  elog_init();
  /* set EasyLogger log format */
  elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
  elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
  elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
  elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
  elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_TIME);
  elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
#ifdef ELOG_COLOR_ENABLE
  elog_set_text_color_enabled(true);
#endif
  /* start EasyLogger */
  elog_start();
}

void *I2C_PWM_callback_fun(void *arg)
{
  // 测试
  for (int i = PropellerPower_Med - 10; i < PropellerPower_Med + 10; i++)
  {
    I2C_PWM_SetPWM(1, 0, i);
    delay(2000);
  }
  return NULL;
}




int main()
{
  easyloggerInit();
  if (wiringPiSetup() < 0)
  {
    log_e("Unable to start wiringPi: %s", strerror(errno));
    return 1;
  }

  // Propeller_Init();
  // I2C_PWM_callback_fun(NULL);

  // sensor_thread_init(); //初始化传感器
  server_thread_init(); //初始化服务器
  // devices_thread_init();
  
  while (1)
  {
    sleep(1);
  }
  return 0;
}
