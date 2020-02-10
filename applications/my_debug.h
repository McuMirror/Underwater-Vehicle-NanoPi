#ifndef __MY_DEBUG__
#define __MY_DEBUG__

#define Nano_DEBUG 1

char *itoa(int num, char *str, int radix);
void i2c_debug_reg(int fd, int reg);

#endif
