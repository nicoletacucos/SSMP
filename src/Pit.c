#include "Pit.h"
#include "Uart.h"
#include "Gpio.h"
#include "i2c.h"

uint32_t timer_value;
volatile uint8_t read_xy_flag = 0;


typedef enum {
    WHITE, GREEN, BLUE, MAGENTA
} LedState;

LedState current_state = WHITE;

void PIT_Init(void) {
	// Activarea semnalului de ceas pentru perifericul PIT
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	// Utilizarea semnalului de ceas pentru tabloul de timere
	PIT_MCR &= ~PIT_MCR_MDIS_MASK;
	// Oprirea decrementarii valorilor numaratoarelor in modul debug
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	// Setarea valoarea numaratorului de pe canalul 0 la o perioada de 0.293 secunde(293ms)
	
	
	//Load Value = 0.293 sec * 48.000.000 Hz - 1
  //           = 14064000 - 1
  //           = 0x27FFFF
	
	
	PIT->CHANNEL[0].LDVAL = 0x27FFFF;
  // Activarea întreruperilor pe canalul 0
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
	// Activarea timerului de pe canalul 0
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;

	PIT->CHANNEL[1].LDVAL = 0x2DC6BFF;
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE_MASK;
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK;
	
	// Activarea întreruperii mascabile si setarea prioritatiis
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_SetPriority(PIT_IRQn,5);
	NVIC_EnableIRQ(PIT_IRQn);
}
uint8_t UTILS_PrintTimer(uint32_t value){
	uint8_t minutes = value/60;
	uint8_t seconds = value%60;
	
	UART0_Transmit(0x0D);
	UART0_Transmit(minutes/10 + 0x30);
	UART0_Transmit(minutes%10 + 0x30);
	UART0_Transmit(0x3A);
	UART0_Transmit(seconds/10 + 0x30);
	UART0_Transmit(seconds%10 + 0x30);
	
	if(timer_value == 99*60+59){
		timer_value = 0;
	}
	return NULL;
}

void PIT_IRQHandler(void) {
	
	if(PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {  //timeout occured
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
		 
		switch (current_state) {
			case WHITE:
				getWhite();
				current_state = GREEN;
				break;
			case GREEN:
				getGreen();
				current_state = BLUE;
				break;
			case BLUE:
				getBlue();
				current_state = MAGENTA;
				break;
			case MAGENTA:
				getMagenta();
				current_state = WHITE;
				break;
		}
	}
	if(PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {  //timeout occured
		PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK;

		read_xy_flag = 1;
	}

}
