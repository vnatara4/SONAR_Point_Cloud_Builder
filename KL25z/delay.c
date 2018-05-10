#include <MKL25Z4.H>
#include "delay.h"
#include "math.h"
volatile unsigned int waitcounter;
void Delay (uint32_t dly) {
  dly=dly*0.49;
	volatile uint32_t t;
	
	for (t=dly; t>0; t--)
		;
	
}
void SysTick_Handler()
{
	if(waitcounter>0)
		waitcounter--;
}
void waitms(uint16_t length_ms)
{	
		waitcounter=(length_ms*2);
		do
		{
			length_ms=waitcounter;
		}
		while(length_ms*2);
}
void SysTimerInit(void)
{ 
  SysTick->CTRL = 0;
  SysTick->VAL=0;
  SysTick->LOAD = 1500-1;
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
}
