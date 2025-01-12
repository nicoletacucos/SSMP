#include "Uart.h"
#include "Pit.h"
#include "Gpio.h"
#include "i2c.h"
#include "stdio.h"
#include "ClockSettings.h"


extern uint32_t timer_value;
extern uint8_t led_state;

int main() {
	//SystemClock_Configure();
	//SystemClockTick_Configure();
	UART0_Init(115200);
	i2c_init();
	uint8_t status = init_sensor();
	int16_t x = read_full_x();
	UART0_Transmit(0X0D);
	UART0_Transmit(0X0A);
	
	
	//OutputPIN_Init();
	//PIT_Init();


	//while(1) {}
	return 0;
}