#include "Pit.h"
#include "Uart.h"

#define OUT_RED_LED_PIN (12) // PORT A
#define OUT_GREEN_LED_PIN (5) // PORT A
uint32_t timer_value;
uint8_t red_led_state = 1;
uint8_t green_led_state;

void PIT_Init(void) {
	
	// Activarea semnalului de ceas pentru perifericul PIT
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	// Utilizarea semnalului de ceas pentru tabloul de timere
	PIT_MCR &= ~PIT_MCR_MDIS_MASK;
	// Oprirea decrementarii valorilor numaratoarelor in modul debug
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	// Setarea valoarea numaratorului de pe canalul 0 la o perioada de 0.293 secunda
	PIT->CHANNEL[0].LDVAL = 0x2EE147;
	
  // Activarea �ntreruperilor pe canalul 0
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
	// Activarea timerului de pe canalul 0
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
	
	// Setarea valoarea numaratorului de pe canalul 1 la o perioada de 0.5 secunde
	PIT->CHANNEL[1].LDVAL = 0x4FFFFF;
	
	// Activara �ntreruperilor pe canalul 1
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE_MASK;
	// Activarea timerului de pe canalul 1
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK;
	
	
	// Activarea �ntreruperii mascabile si setarea prioritatiis
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_SetPriority(PIT_IRQn,5);
	NVIC_EnableIRQ(PIT_IRQn);
}

	

void PIT_IRQHandler(void) {
	
	if(PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {  //timeout occured
		
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
		
		if(red_led_state) {
			GPIOA->PCOR |= (1<<OUT_RED_LED_PIN);
			red_led_state = 0;
			
			GPIOA->PSOR |= (1<<OUT_GREEN_LED_PIN);
			green_led_state = 1;
		}


	}
	
}