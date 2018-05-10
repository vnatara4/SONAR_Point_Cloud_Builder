#include <MKL25Z4.H>
#include "GPIO.h"
#include "timers.h"
#define MASK(x) (1UL << (x))
uint8_t Port_no;
volatile unsigned long final_val;
unsigned long long counter=2;
void Init_GPIO()
{
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[Debug_Signal_0] &= ~PORT_PCR_MUX_MASK;          
  PORTB->PCR[Debug_Signal_0] |= PORT_PCR_MUX(1);
	
	PORTB->PCR[Debug_Signal_2] &= ~PORT_PCR_MUX_MASK;          
  PORTB->PCR[Debug_Signal_2] |= PORT_PCR_MUX(1);
	
	PORTB->PCR[Debug_Signal_3] &= ~PORT_PCR_MUX_MASK;          
  PORTB->PCR[Debug_Signal_3] |= PORT_PCR_MUX(1);
	
	PORTB->PCR[Debug_Signal_1] &= ~PORT_PCR_MUX_MASK;          
  PORTB->PCR[Debug_Signal_1] |= PORT_PCR_MUX(1);
	
	PTB->PDDR |= MASK(Debug_Signal_1);
	PTB->PDDR |= MASK(Debug_Signal_2);
	PTB->PDDR |= MASK(Debug_Signal_3);
	PTB->PDDR |= MASK(Debug_Signal_0);
}

void Debug_Signal_Activate(uint8_t GPIO_Signal)
{
	PTB->PSOR = MASK(Debug_Signal_1);
}
void Init_Echo(uint8_t no) {
	SIM->SCGC5 |=  SIM_SCGC5_PORTD_MASK; /* enable clock for port D */

	/* Select GPIO and enable pull-up resistors and interrupts 
		on falling edges for pins connected to switches */
	PORTD->PCR[no] |= PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_IRQC(0x0b);
	
	/* Set port D switch bit to inputs */
	PTD->PDDR &= ~MASK(no);
	Port_no=no;   
	/* Enable Interrupts */
	NVIC_SetPriority(PORTD_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(PORTD_IRQn); 
	NVIC_EnableIRQ(PORTD_IRQn);
}

void PORTD_IRQHandler(void) {  
	if ((PORTD->ISFR & MASK(Port_no)))
		{
			PTB->PTOR=MASK(Debug_Signal_1);
		  if(counter%2==0)
				TPM0->CNT=0;
			else
				final_val=(unsigned long)TPM0->CNT&0xFFFF;
			counter++;
		}
	// clear status flags 
	PORTD->ISFR = 0xffffffff;
}