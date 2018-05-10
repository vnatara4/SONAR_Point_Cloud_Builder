#include "timers.h"
#include "MKL25Z4.h"
#include "LEDs.h"
#include "GPIO.h"
#include "delay.h"
#define MASK(x) (1UL << (x))
void Init_Timer()
{
	//turn on clock to TPM, Port E
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	//set clock source for tpm
	//SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK);
	
	//set TPM to divide by 128 prescaler and enable counting (CMOD)
	TPM0->SC =(TPM_SC_CPWMS(0)|TPM_SC_CMOD(1) | TPM_SC_PS(7));
}

void Init_PIT(unsigned period) {
	// Enable clock to PIT module
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	
	// Enable module, freeze timers in debug mode
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	
	// Initialize PIT0 to count down from argument 
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(period);

	// No chaining
	PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_CHN_MASK;
	
#if 1
	// Generate interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;

	/* Enable Interrupts */
	NVIC_SetPriority(PIT_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(PIT_IRQn); 
	NVIC_EnableIRQ(PIT_IRQn);	
#endif
}


void Start_PIT(void) {
// Enable counter
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
}

void Stop_PIT(void) {
// Enable counter
	PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;
}


void PIT_IRQHandler() {
	
	//clear pending IRQ
	NVIC_ClearPendingIRQ(PIT_IRQn);
	
	PTB->PSOR=MASK(Debug_Signal_0);
	Delay(40);
	PTB->PCOR=MASK(Debug_Signal_0);
	
	// check to see which channel triggered interrupt 
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
		// clear status flag for timer channel 0
		PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
		
	} else if (PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {
		// clear status flag for timer channel 1
		PIT->CHANNEL[1].TFLG |= PIT_TFLG_TIF_MASK;
		// Do ISR work here
		
	} 
}



// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
