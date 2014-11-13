
#include "audio.h"
#include <stdio.h>
#include <xil_io.h>
#include <sleep.h>
#include "xiicps.h"
#include <xil_printf.h>
#include <xparameters.h>
#include "xuartps.h"


XIicPs Iic;


unsigned char IicConfig(unsigned int DeviceIdPS)
{
	XIicPs_Config *Config;
	int Status;

	//Initialize the IIC driver so that it's ready to use
	//Look up the configuration in the config table, then initialize it.
	Config = XIicPs_LookupConfig(DeviceIdPS);
	if(NULL == Config) {
		return XST_FAILURE;
	}

	Status = XIicPs_CfgInitialize(&Iic, Config, Config->BaseAddress);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	//Set the IIC serial clock rate.
	XIicPs_SetSClk(&Iic, IIC_SCLK_RATE);

	return XST_SUCCESS;
}


/******************************************************************************
 * Configures audio codes's internal PLL. With MCLK = 8 MHz it configures the
 * PLL for a VCO frequency = 49.152 MHz, and an audio sample rate of 48 KHz.
 *
 * @param	none.
 *
 * @return	none.
 *****************************************************************************/
void AudioPllConfig() {

	unsigned char u8TxData[8], u8RxData[6];
	int Status;

	Status = IicConfig(XPAR_XIICPS_0_DEVICE_ID);//, FMC_IIC_ID, HDMI_IIC_ID);
	if(Status != XST_SUCCESS) {
		xil_printf("\nError initializing IIC");
		//return XST_FAILURE;
	}

	AudioWriteToReg(R0_CLOCK_CONTROL, 0x0E);  //Set PLL clock enable

	// Write 6 bytes to R1
	u8TxData[0] = 0x40;
	u8TxData[1] = 0x02;
	u8TxData[2] = 0x02; // byte 1
	u8TxData[3] = 0x71; // byte 2
	u8TxData[4] = 0x02; // byte 3
	u8TxData[5] = 0x3C; // byte 4
	u8TxData[6] = 0x21; // byte 5
	u8TxData[7] = 0x01; // byte 6


	XIicPs_MasterSendPolled(&Iic, u8TxData, 8, (IIC_SLAVE_ADDR >> 1));
	while(XIicPs_BusIsBusy(&Iic));

	// Poll PLL Lock bit
	u8TxData[0] = 0x40;
	u8TxData[1] = 0x02;

	do {
		XIicPs_MasterSendPolled(&Iic, u8TxData, 2, (IIC_SLAVE_ADDR >> 1));
		while(XIicPs_BusIsBusy(&Iic));
		XIicPs_MasterRecvPolled(&Iic, u8RxData, 6, (IIC_SLAVE_ADDR >> 1));
		while(XIicPs_BusIsBusy(&Iic));
	}
	while((u8RxData[5] & 0x02) == 0);
    print("Codec PLL is locked\n\r");
	AudioWriteToReg(R0_CLOCK_CONTROL, 0x0F);//COREN
}

/******************************************************************************
 * Function to write one byte (8-bits) to one of the registers from the audio
 * controller.
 *
 * @param	u8RegAddr is the LSB part of the register address (0x40xx).
 * @param	u8Data is the data byte to write.
 *
 * @return	none.
 *****************************************************************************/
void AudioWriteToReg(unsigned char u8RegAddr, unsigned char u8Data) {

	unsigned char u8TxData[3];

	u8TxData[0] = 0x40;
	u8TxData[1] = u8RegAddr;
	u8TxData[2] = u8Data;

	XIicPs_MasterSendPolled(&Iic, u8TxData, 3, (IIC_SLAVE_ADDR >> 1));
	while(XIicPs_BusIsBusy(&Iic));
}

/******************************************************************************
 * Configures audio codes's various mixers, ADC's, DAC's, and amplifiers to
 * accept stereo input from line in and push stereo output to line out
 *
 * @param	none.
 *
 * @return	none.
 *****************************************************************************/
void AudioConfigure()
{
	AudioWriteToReg(R4_RECORD_MIXER_LEFT_CONTROL_0, 0x1);
	AudioWriteToReg(R5_RECORD_MIXER_LEFT_CONTROL_1, 0x10);//20 dB gain
	AudioWriteToReg(R8_LEFT_DIFFERENTIAL_INPUT_VOLUME_CONTROL, 0x63); //Left channel volume control
	AudioWriteToReg(R10_RECORD_MICROPHONE_BIAS_CONTROL, 0x07);
	AudioWriteToReg(R11_ALC_CONTROL_0,0x02);
	AudioWriteToReg(R12_ALC_CONTROL_1,0x3B);
	AudioWriteToReg(R13_ALC_CONTROL_2,0x26);
	AudioWriteToReg(R14_ALC_CONTROL_3,0x3F);
	AudioWriteToReg(R17_CONVERTER_CONTROL_0, 0x06);//96 kHz
    AudioWriteToReg(R19_ADC_CONTROL, 0x11); //enable ADCs
    AudioWriteToReg(R20_LEFT_INPUT_DIGITAL_VOLUME, 0x10);
	AudioWriteToReg(R59_SERIAL_OUTPUT_ROUTE_CONTROL, 0x01); //connect I2S serial port input (SDATA_I) to ADCs
	AudioWriteToReg(R64_SERIAL_PORT_SAMPLING_RATE, 0x06);//96 kHz
	AudioWriteToReg(R65_CLOCK_ENABLE_0, 0x7F); //Enable clocks
	AudioWriteToReg(R66_CLOCK_ENABLE_1, 0x03); //Enable rest of clocks
}


void get_audio(Xint16* audio_data)
{
	unsigned long u32Temp;
	Xint32 i;
	Xint32 in_data;
    i = 0;
	while (i<128)
	{

		do //wait for RX data to become available
		{
			u32Temp = Xil_In32(I2S_STATUS_REG);
		} while ( u32Temp == 0);

		in_data = Xil_In32(I2S_DATA_RX_L_REG);
		*audio_data = ((in_data<<8)>>16);
		Xil_Out32(I2S_STATUS_REG, 0x00000001); //Clear data rdy bit
		audio_data++;
		i++;
	}
}
