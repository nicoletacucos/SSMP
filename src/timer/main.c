#include "Uart.h"
#include "Pit.h"
#include "Gpio.h"


extern uint32_t timer_value;
extern uint8_t led_state;

int main() {
	
	UART0_Init(115200);
	OutputPIN_Init();
	PIT_Init();
	while(1) {}
	return 0;
}