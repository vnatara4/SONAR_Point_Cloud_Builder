#include <MKL25Z4.H>
#include <stdio.h>
#include "gpio_defs.h"
#include "UART.h"
#include "LEDs.h"
#include "delay.h"
#include "math.h"
#include "ADC.h"
#include "GPIO.h"
#include "timers.h"
extern volatile unsigned long final_val;
float sampling_buffer[100];
int main (void)
{	
	Init_UART0(115200);
	Init_UART2(115200);
	Init_RGB_LEDs();
	SysTimerInit();
	int i=0;
	float ADC_value;
	float time=0,distance=0;
	Control_RGB_LEDs(0,0,0);
	Init_GPIO();
  Init_PIT(600000);
	Start_PIT();
	Init_Echo(6);
	Init_Timer();
	char str_val[5];
	printf("Ultra Sonic Sensor\n\r");
	while(1)
	{
		//PE22->TX for UART2
		Init_UART2(115200);
		/*UART2_Transmit_Poll(65);
		Input=UART0_Receive_Poll();
		printf("%d",Input);*/
		sampling_buffer[i]=Calculate_ADC();
		ADC_value=(sampling_buffer[i]*3.3)/65535.0;
		time=(final_val*2.6e-6);
		distance=(time*340)/2.0;
		snprintf(str_val,5,"%f", distance);
		Send_String_Poll(str_val);
		printf("distance=%s m time=%f ms\n\r",str_val,(time*1000));
		if(i>=2000)
			i=0;
	}
}