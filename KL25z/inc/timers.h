#ifndef TIMERS_H
#define TIMERS_H
#include "MKL25Z4.h"

#define PWM_PERIOD (30000) // 20 ms
#define PWM_WIDTH_RANGE (PWM_PERIOD/20)
#define T_NEUTRAL (PWM_PERIOD * 1.5 / 20 ) // 1.5 ms
#define T_MAX_CCW (PWM_PERIOD * 1 / 20 ) // 1 ms
#define T_MAX_CW (PWM_PERIOD * 2 / 20 ) // 2 ms

#define POS_RES			(1000)
#define POS_NEUTRAL (0)
#define POS_MAX_CCW (-POS_RES/2)
#define POS_MAX_CW 	(POS_RES/2)

void Init_PIT(unsigned period);
void Start_PIT(void);
void Stop_PIT(void);

void Init_Timer(void);

#endif
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
