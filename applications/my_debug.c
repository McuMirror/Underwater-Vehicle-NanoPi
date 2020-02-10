/*
 * @Description: 
 * @Author: chenxi
 * @Date: 2020-01-27 11:52:28
 * @LastEditTime : 2020-01-27 13:47:20
 * @LastEditors  : chenxi
 */
#include <stdio.h>
#include <stdlib.h>

#include <wiringPi.h>
#include <wiringSerial.h>
#include <wiringPiI2C.h>

#include "my_debug.h"

char *itoa(int num, char *str, int radix)
{ /*索引表*/
  char index[] = "0123456789ABCDEF";
  unsigned unum; /*中间变量*/
  int i = 0, j, k;
  /*确定unum的值*/
  if (radix == 10 && num < 0) /*十进制负数*/
  {
    unum = (unsigned)-num;
    str[i++] = '-';
  }
  else
    unum = (unsigned)num; /*其他情况*/
  /*转换*/
  do
  {
    str[i++] = index[unum % (unsigned)radix];
    unum /= radix;
  } while (unum);
  str[i] = '\0';
  /*逆序*/
  if (str[0] == '-')
    k = 1; /*十进制负数*/
  else
    k = 0;

  for (j = k; j <= (i - 1) / 2; j++)
  {
    char temp;
    temp = str[j];
    str[j] = str[i - 1 + k - j];
    str[i - 1 + k - j] = temp;
  }
  return str;
}

void i2c_debug_reg(int fd, int reg)
{
  int value = wiringPiI2CReadReg8(fd, reg);
  char s[10] = {0};
  itoa(value, s, 2);
  printf("reg:0x%X\n", reg);
  printf(" 二进制 : 0B%s\n", s);
  printf("十六进制: 0x%X\n", value);
}
