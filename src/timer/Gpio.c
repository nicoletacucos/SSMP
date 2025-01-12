#include "Gpio.h"

#define LED_PIN (12) // PORT A

void OutputPIN_Init(void){
	
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	PORTA->PCR[LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[LED_PIN] |= PORT_PCR_MUX(1);
	
	GPIOA->PDDR |= (1<<LED_PIN);
	GPIOA->PCOR |= (1<<LED_PIN);
	
}