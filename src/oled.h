/*
 * oled.h
 *
 *  Created on: 2012-12-16
 *      Author: Administrator
 */

#ifndef OLED_H_
#define OLED_H_
#include "xil_types.h"				// 数据类型定义
#include "xil_io.h"

extern int usleep(unsigned int useconds);

#define OLED_BASE_ADDR 0X74e00000 //need to change when address is changed in EDK

#define OLED_DC			0
#define OLED_RES		1
#define OLED_SCLK		2
#define OLED_SDIN		3
#define OLED_VBAT		4
#define OLED_VDD		5
//Xil_Out32(OLED_BASE_ADDR,1<<i);

// DC
#define Set_OLED_DC  (Xil_Out32(OLED_BASE_ADDR,Xil_In32(OLED_BASE_ADDR)|(1<<OLED_DC)))
#define Clr_OLED_DC  (Xil_Out32(OLED_BASE_ADDR,Xil_In32(OLED_BASE_ADDR)&(~(1<<OLED_DC))))
// RES
#define Set_OLED_RES  (Xil_Out32(OLED_BASE_ADDR,Xil_In32(OLED_BASE_ADDR)|(1<<OLED_RES)))
#define Clr_OLED_RES  (Xil_Out32(OLED_BASE_ADDR,Xil_In32(OLED_BASE_ADDR)&(~(1<<OLED_RES))))

// SCLK
#define Set_OLED_SCLK  (Xil_Out32(OLED_BASE_ADDR,Xil_In32(OLED_BASE_ADDR)|(1<<OLED_SCLK)))
#define Clr_OLED_SCLK  (Xil_Out32(OLED_BASE_ADDR,Xil_In32(OLED_BASE_ADDR)&(~(1<<OLED_SCLK))))
// SDIN
#define Set_OLED_SDIN  (Xil_Out32(OLED_BASE_ADDR,Xil_In32(OLED_BASE_ADDR)|(1<<OLED_SDIN)))
#define Clr_OLED_SDIN  (Xil_Out32(OLED_BASE_ADDR,Xil_In32(OLED_BASE_ADDR)&(~(1<<OLED_SDIN))))
// OLED_VBAT
#define Set_OLED_VBAT  (Xil_Out32(OLED_BASE_ADDR,Xil_In32(OLED_BASE_ADDR)|(1<<OLED_VBAT)))
#define Clr_OLED_VBAT  (Xil_Out32(OLED_BASE_ADDR,Xil_In32(OLED_BASE_ADDR)&(~(1<<OLED_VBAT))))

// OLED_VDD
#define Set_OLED_VDD  (Xil_Out32(OLED_BASE_ADDR,Xil_In32(OLED_BASE_ADDR)|(1<<OLED_VDD)))
#define Clr_OLED_VDD  (Xil_Out32(OLED_BASE_ADDR,Xil_In32(OLED_BASE_ADDR)&(~(1<<OLED_VDD))))


//OLED控制用函数
void write_data(u8 data);
void write_cmd(u8 data);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);

void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y,const u8 *p);
void OLED_Equalizer_128(u8 *data);

#endif /* OLED_H_ */
