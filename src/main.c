#include "Uart.h"
#include "Pit.h"
#include "Gpio.h"
#include "i2c.h"
#include "stdio.h"
#include <stdint.h>

extern uint8_t led_state;
extern volatile uint8_t read_accel_semaphore;
extern volatile uint8_t accel_data_ready;
extern volatile uint8_t led_semaphore;
extern volatile uint8_t led_timer_elapsed ;

uint8_t reverse = 0;  // 0 pentru secventa normala, 1 pentru secventa inversata
int16_t x_value;

typedef enum {
    STATE_IDLE,
    STATE_READ_ACCEL,
	STATE_WAIT_FOR_ACCEL,	
    STATE_LED_SEQUENCE_WHITE,
    STATE_LED_SEQUENCE_GREEN,
	STATE_LED_SEQUENCE_GREEN_INVERTED,
    STATE_LED_SEQUENCE_BLUE,
    STATE_LED_SEQUENCE_MAGENTA,
	STATE_TURN_ON_RED,
    STATE_TURN_ON_GREEN,
	STATE_WAIT_FOR_LED,
	STATE_WAIT_FOR_OUT_LED
} SystemState;

typedef enum {
    WHITE, GREEN, BLUE, MAGENTA
} LedState;

LedState current_state = WHITE;

SystemState current_system_state = STATE_IDLE;

void system_fsm() {
    switch (current_system_state) {
        case STATE_IDLE:	
			
			//UART0_TransmitString("STATE_IDLE\r\n");
            if (led_semaphore) {
				led_semaphore = 0;
				current_system_state = STATE_WAIT_FOR_LED;
                
            } else if (read_accel_semaphore) {
                read_accel_semaphore = 0;
                current_system_state = STATE_WAIT_FOR_ACCEL;
			}
            break;
		
		case STATE_WAIT_FOR_ACCEL:
            if (accel_data_ready) {
				accel_data_ready = 0;
				current_system_state = STATE_READ_ACCEL;
			}
            break;

        case STATE_READ_ACCEL:
            x_value=read_full_x();

            // if (x_value > 5 || x_value < -5) {
            //     current_system_state = STATE_WAIT_FOR_LED;
            // } else {
            current_system_state = STATE_IDLE;
            //}
            break;

		case STATE_WAIT_FOR_LED:
            if (led_timer_elapsed){
				led_timer_elapsed = 0;

				// if (x_value > 5)
				// 	current_system_state = STATE_TURN_ON_RED;
				// else if(x_value < -5)
				// 	current_system_state = STATE_TURN_ON_GREEN;

                if (reverse == 0) {
                    switch (current_state) {
                        case WHITE:
                            current_system_state = STATE_LED_SEQUENCE_WHITE;
                            break;
                        case GREEN:
                            current_system_state = STATE_LED_SEQUENCE_GREEN;
                            break;
                        case BLUE:
                            current_system_state = STATE_LED_SEQUENCE_BLUE;
                            break;
                        case MAGENTA:
                            current_system_state = STATE_LED_SEQUENCE_MAGENTA;
                            break;
                    }
                } else {
                    switch (current_state) {
                        case WHITE:
                            current_system_state = STATE_LED_SEQUENCE_WHITE;
                            break;
                        case MAGENTA:
                            current_system_state = STATE_LED_SEQUENCE_MAGENTA;
                            break;
                        case BLUE:
                            current_system_state = STATE_LED_SEQUENCE_BLUE;
                            break;
                        case GREEN:
                            current_system_state = STATE_LED_SEQUENCE_GREEN;
							//current_system_state = STATE_LED_SEQUENCE_GREEN_INVERTED;
                            break;
                    }
                }
            }
            break;

		// case STATE_TURN_ON_RED:
        //     getOutRed();
        //     current_system_state = STATE_IDLE;
		// 	led_timer_elapsed = 0;
        //     break;

        // case STATE_TURN_ON_GREEN:
        //     getOutGreen();
        //     current_system_state = STATE_IDLE;
		// 	led_timer_elapsed = 0;
        //     break;

        case STATE_LED_SEQUENCE_WHITE:
            getWhite();
			//UART0_TransmitString("STATE_LED_SEQUENCE_WHITE\r\n");
            if (reverse == 0) {
                current_state = GREEN;
            } else {
                current_state = MAGENTA;
            }
			led_timer_elapsed = 0;
            current_system_state = STATE_IDLE;
            break;

        case STATE_LED_SEQUENCE_GREEN:
            getGreen();
			//UART0_TransmitString("STATE_LED_SEQUENCE_GREEN\r\n");
            if (reverse == 0) {
                current_state = BLUE;
            } else {
                current_state = WHITE;
            }
			led_timer_elapsed = 0;
            current_system_state = STATE_IDLE;
            break;

// 		case STATE_LED_SEQUENCE_GREEN_INVERTED:
//             getGreenInv();
// 			UART0_TransmitString("STATE_LED_SEQUENCE_GREEN_INVERTED\r\n");
//             if (reverse == 0) {
//                 current_state = BLUE;
//             } else {
//                 current_state = WHITE;
//             }
// 			led_timer_elapsed = 0;
//             current_system_state = STATE_IDLE;
//             break;

        case STATE_LED_SEQUENCE_BLUE:
            getBlue();
			//UART0_TransmitString("STATE_LED_SEQUENCE_BLUE\r\n");
            if (reverse == 0) {
                current_state = MAGENTA;
            } else {
                current_state = GREEN;
            }
			led_timer_elapsed = 0;
            current_system_state = STATE_IDLE;
            break;

        case STATE_LED_SEQUENCE_MAGENTA:
            getMagenta();
			//UART0_TransmitString("STATE_LED_SEQUENCE_MAGENTA\r\n");
            if (reverse == 0) {
                current_state = WHITE;
            } else {
                current_state = BLUE;
            }
			led_timer_elapsed = 0;
            current_system_state = STATE_IDLE;
            break;
    }
}

int main() {
	
	UART0_Init(14400);
	i2c_init();
	uint8_t status = init_sensor();
	calibrate_x();
	
	OutputPIN_Init();
	PIT_Init();
	

	
	while(1) {   
		system_fsm();
	}
	return 0;
}