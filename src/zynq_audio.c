#include <stdio.h>
#include "audio.h"
#include "oled.h"
#include "sleep.h"
#include <stdlib.h>
#include <math.h>
#include "xtime_l.h"
#include "xil_io.h"
#include "xparameters.h"


u8 count = 0, noise_count = 0;
Xint16 audio_data[128];
u32 finaldata[128], noise[128];
u32 magnitude[8][128];
u8 oled[128] = {};
//Remember this should not be global
#define timer_base 0xF8F00000
/***********************************************************
Timer Registers
************************************************************/
static volatile int *timer_counter_l=(volatile int *)(timer_base+0x200);
static volatile int *timer_counter_h=(volatile int *)(timer_base+0x204);
static volatile int *timer_ctrl=(volatile int *)(timer_base+0x208);
/***********************************************************/
/***********************************************************
Function definitions
************************************************************/
void init_timer(volatile int *timer_ctrl, volatile int *timer_counter_l, volatile int *timer_counter_h){
        *timer_ctrl=0x0;
        *timer_counter_l=0x1;
        *timer_counter_h=0x0;
        DATA_SYNC;
}

void start_timer(volatile int *timer_ctrl){
        *timer_ctrl=*timer_ctrl | 0x00000001;
        DATA_SYNC;
}

void stop_timer(volatile int *timer_ctrl){
        *timer_ctrl=*timer_ctrl & 0xFFFFFFFE;
        DATA_SYNC;
}

void noise_cancellation()
{
	int i;
	for(i = 0; i < 128; i++)
	{
		noise[i]= (magnitude[0][i]+magnitude[1][i]+magnitude[2][i]+\
								magnitude[3][i]+magnitude[4][i]+magnitude[5][i]+\
								magnitude[6][i]+magnitude[7][i])>>3;
	}
}

void myaverage()
{
	int i=0;
	for (i=0;i<128;i++)
	{
		finaldata[i]= (magnitude[0][i]+magnitude[1][i]+magnitude[2][i]+\
						magnitude[3][i]+magnitude[4][i]+magnitude[5][i]+\
						magnitude[6][i]+magnitude[7][i])>>3;
		finaldata[i]= finaldata[i]-noise[i];
		oled[i]= (finaldata[i]>>10)&0xff;
	}
}

int main()
{

	Xil_Out32(OLED_BASE_ADDR,0xff);
	OLED_Init();			//oled init
	IicConfig(XPAR_XIICPS_0_DEVICE_ID);
	AudioPllConfig(); //enable core clock for ADAU1761
	AudioConfigure();

	xil_printf("ADAU1761 configured\n\r");

	/*
	 * perform continuous read and writes from the codec that result in a loopback
	 * from Line in to Line out
	 */

	int i;

	while(1)
	{
		//print("Code Profiling\n\r");

		//Initialise the timer for performance monitoring
		init_timer(timer_ctrl, timer_counter_l, timer_counter_h);
		get_audio(audio_data);
		start_timer(timer_ctrl);
		for(i = 0; i < 128; i++)
				{
					Xil_Out32(XPAR_MYPERIPHERAL_0_S_AXI_MEM0_BASEADDR + (4 * i), audio_data[i]);
				}

				for(i = 0; i < 128; i++)
				{
					magnitude[count][i] = Xil_In32(XPAR_MYPERIPHERAL_0_S_AXI_MEM0_BASEADDR + (4 * i));
					//printf("%d\n", magnitude[count][i]);
				}

				if(noise_count > 8)
				{
					myaverage();
				}

				else
				{
					noise_cancellation();
					noise_count++;
				}

				count=(count==7)?0:count+1;

				stop_timer(timer_ctrl);

		//Calculate the time for the operation
		xil_printf("Communication time %d us\n\r", (*timer_counter_l)/333);
		OLED_Clear();
		OLED_Equalizer_128(oled);
	}
    return 0;
}
