#include <MKL25Z4.H>
#include "math.h"
uint8_t channel_No;
void Init_ADC(uint8_t Channel_no)
{
	uint8_t port_no;
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK; 
	if(Channel_no==23||Channel_no==7||Channel_no==4||Channel_no==3||Channel_no==0)
	{
		SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
		if(Channel_no==0)
			port_no=2;
		if(Channel_no==3)
			port_no=22;
		if(Channel_no==4)
			port_no=21;
		if(Channel_no==7)
			port_no=23;
		if(Channel_no==23)
			port_no=30;
		PORTE->PCR[port_no] &= ~PORT_PCR_MUX_MASK;	
		PORTE->PCR[port_no] |= PORT_PCR_MUX(0);
	}		
	if(Channel_no==6)
	{
		SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
		PORTD->PCR[5] &= ~PORT_PCR_MUX_MASK;	
		PORTD->PCR[5] |= PORT_PCR_MUX(0);
	}
	if(Channel_no==11 || Channel_no==14 || Channel_no==15)
	{
		SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
		if(Channel_no==11)
			port_no=2;
		if(Channel_no==14)
			port_no=0;
		if(Channel_no==15)
			port_no=1;
		PORTC->PCR[port_no] &= ~PORT_PCR_MUX_MASK;	
		PORTC->PCR[port_no] |= PORT_PCR_MUX(0);
	}
	
	// Select analog for pin
	/*
	PORTE->PCR[20] &= ~PORT_PCR_MUX_MASK;	
	PORTE->PCR[20] |= PORT_PCR_MUX(0);	
	*/
	// Low power configuration, long sample time, 16 bit single-ended conversion, bus clock input
	ADC0->CFG1 = ADC_CFG1_ADLPC_MASK | ADC_CFG1_ADLSMP_MASK | ADC_CFG1_MODE(3) | ADC_CFG1_ADICLK(0);
	// Software trigger, compare function disabled, DMA disabled, voltage references VREFH and VREFL
	ADC0->SC2 = ADC_SC2_REFSEL(0);
	channel_No=Channel_no;
}

uint16_t Calculate_ADC()
{
	unsigned result=1;
	float temp;
	return result;
	//to start a conversion
	ADC0->SC1[0]=channel_No;
	
	while(!(ADC0->SC1[0]& ADC_SC1_COCO_MASK)); // wait till the conversion
	
	result=ADC0->R[0];
	temp=(result*3.3)/65535;
	return result;
	
}