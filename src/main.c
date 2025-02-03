#include "Uart.h"
#include "Pit.h"
#include "Gpio.h"
#include "i2c.h"
#include "stdio.h"


extern uint32_t timer_value;
extern uint8_t led_state;
extern volatile uint8_t read_xy_flag;

int main() {
	//SystemClock_Configure();
	//SystemClockTick_Configure();
	UART0_Init(115200);
	i2c_init();
	uint8_t status = init_sensor();
	calibrate_xOy();
	
	OutputPIN_Init();
	PIT_Init();
	
	

	
;
	while(1) {   
		if (read_xy_flag) {
      		read_xy_flag = 0; 
      		read_full_x(); 
      		read_full_y(); 
    	}
	}
	return 0;
}