#ifndef GPIO_H
#define GPIO_H

#define Debug_Signal_0 (0)	
#define Debug_Signal_1 (1)	
#define Debug_Signal_2 (2)
#define Debug_Signal_3 (3)

void Init_GPIO();
void Debug_Signal_Activate(uint8_t GPIO_Signal);
void Init_Echo(uint8_t no);


#endif