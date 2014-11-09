#include <stdio.h>
#include "audio.h"
#include "oled.h"
#include "sleep.h"
#include <stdlib.h>
#include <math.h>
#include "xtime_l.h"
#include "xil_io.h"
#include "xparameters.h"

float total[128] = {}, sreal[7][128] = {{}}, simag[7][128] = {{}};
int count = 0, circular_buffer[8][128] = {{}}, i;
unsigned  int final[128];

Xint16 audio_data[128];  //Remember this should not be global

double x[128],y[128]={};

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
	for(i = 0; i < 128; i++)
	{
		final[i] = (int)total[i];
		
		if(total[i] > 40)
		{
			final[i] = 40;
		}
		
		if(total[i] < 10)
		{
			final[i] = 0;
		}
	}
}

void myaverage(unsigned int avg[128])
{
	if(count >= 8)
	{
		count = 0;
	}

	for(i = 0; i < 128; i++)
		{
			total[i] -= (circular_buffer[count][i] / 8);
			circular_buffer[count][i] = avg[i];
			total[i] += (avg[i] / 8);
		}
		count++;
}

void magnitude()
{
	float mag[128];
	
	for(i = 0; i < 128; i++)
	{
		mag[i] = (x[i] * x[i]) + (y[i] * y[i]);
		final[i] = (int)mag[i]>>16;
	}	

	//myaverage(final); //Make sure this is how function needs to be called
}

void myfft()
{
	int n = 128 ,m = 7;


int j,k,n1,n2;
double c,s,e,a,t1,t2;

for(i = 0;i<128;i++)
{
	x[i] = audio_data[i];
	y[i]=0;
}

j = 0; /* bit-reverse */
n2 = n/2;
for (i=1; i < n - 1; i++)
{
  n1 = n2;
  while ( j >= n1 )
   {
    j = j - n1;
    n1 = n1/2;
   }
  j = j + n1;

  if (i < j)
   {
    t1 = x[i];
    x[i] = x[j];
    x[j] = t1;
    t1 = y[i];
    y[i] = y[j];
    y[j] = t1;
   }
}


n1 = 0; /* FFT */
n2 = 1;

for (i=0; i < m; i++)
{
  n1 = n2;
  n2 = n2 + n2;
  e = -6.283185307179586/n2;
  a = 0.0;

  for (j=0; j < n1; j++)
   {
    c = cos(a);
    s = sin(a);
    a = a + e;

    for (k=j; k < n; k=k+n2)
     {
      t1 = c*x[k+n1] - s*y[k+n1];
      t2 = s*x[k+n1] + c*y[k+n1];
      x[k+n1] = x[k] - t1;
      y[k+n1] = y[k] - t2;
      x[k] = x[k] + t1;
      y[k] = y[k] + t2;
     }
   }
}
	magnitude();
}

int main()
{
	u8 *oled_equalizer_buf=(u8 *)malloc(128*sizeof(u8));
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

	 for(i = 0; i < 7; i++)
	 {
		get_audio(audio_data);
		myfft();
	 }
	 
	while(1)
	{
		print("Code Profiling\n\r");

		//Initialise the timer for performance monitoring
		init_timer(timer_ctrl, timer_counter_l, timer_counter_h);
		start_timer(timer_ctrl);
		
		get_audio(audio_data);

		myfft();

		stop_timer(timer_ctrl);
	
		//Calculate the time for the operation
		xil_printf("Communication time %d us\n\r", (*timer_counter_l)/333);
		
		for(i=0;i<128;i++)
		{
			oled_equalizer_buf[i]=final[i];
		}
		OLED_Clear();
		OLED_Equalizer_128(oled_equalizer_buf);
	}
    return 0;
}