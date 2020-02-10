/*
 * @Description: 
 * @Author: chenxi
 * @Date: 2020-01-22 20:13:17
 * @LastEditTime : 2020-02-04 12:18:05
 * @LastEditors  : chenxi
 */
#ifndef __DRV_OLED_H
#define __DRV_OLED_H

#include "..//user//DataType.h"
#include "..//applications//oledfont.h"

#define OLED_CMD 0  //д����
#define OLED_DATA 1 //д����
//OLED�����ú���
void OLED_WR_Byte(uint8 dat, uint8 cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);

int OLED_Init(void);
void OLED_Clear(void);

void OLED_DrawPoint(uint8 x, uint8 y, uint8 t);
void OLED_Fill(uint8 x1, uint8 y1, uint8 x2, uint8 y2, uint8 dot);
void OLED_ShowChar(uint8 x, uint8 y, uint8 chr, uint8 size, uint8 mode);
void OLED_ShowNum(uint8 x, uint8 y, uint32 num, uint8 len, uint8 size);
void OLED_ShowString(uint8 x, uint8 y, const uint8 *p, uint8 size);
void OLED_ShowPicture(uint8 x, uint8 y, const uint8 *p, uint8 p_w, uint8 p_h);

void OLED_DrawBMP(uint8 x, uint8 y, uint8 chr, uint8 size, uint8 mode);

#endif
