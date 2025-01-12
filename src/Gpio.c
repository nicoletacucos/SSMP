#include "Gpio.h"

#define OUT_RED_LED_PIN (12) // PORT A
#define OUT_GREEN_LED_PIN (5) // PORT A


void OutputPIN_Init(void){
	
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	//red led mux
	PORTA->PCR[OUT_RED_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[OUT_RED_LED_PIN] |= PORT_PCR_MUX(1);
	
	GPIOA->PDDR |= (1<<OUT_RED_LED_PIN);
	GPIOA->PSOR |= (1<<OUT_RED_LED_PIN);
	//green red mux
	PORTA->PCR[OUT_GREEN_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[OUT_GREEN_LED_PIN] |= PORT_PCR_MUX(1);
	
	GPIOA->PDDR |= (1<<OUT_GREEN_LED_PIN);
	GPIOA->PCOR |= (1<<OUT_GREEN_LED_PIN);
	
}