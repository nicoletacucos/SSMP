#include "Pit.h"
#include "Uart.h"
#include "Gpio.h"
#include "i2c.h"

volatile uint8_t read_accel_semaphore = 0;
volatile uint8_t led_semaphore = 0;
volatile uint8_t accel_data_ready = 0;
volatile uint8_t led_timer_elapsed = 0;

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

	//PIT->CHANNEL[1].LDVAL = 0x497453F;
	PIT->CHANNEL[1].LDVAL = 0x2DC6BFF;
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE_MASK;
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK;
	
	// Activarea întreruperii mascabile si setarea prioritatiis
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_SetPriority(PIT_IRQn,5);
	NVIC_EnableIRQ(PIT_IRQn);
}


void PIT_IRQHandler(void) {
	
	if(PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {  //timeout occured
		 
		led_timer_elapsed = 1;
        led_semaphore = 1;
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
		
	}
	if(PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {  //timeout occured

		read_accel_semaphore = 1;
		accel_data_ready = 1; 
		PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK;
	}

}
