#include "Gpio.h"
#include "Uart.h"

#define RED_LED_SHIFT 18  // Pinul pentru LED-ul rosu pe portul B
#define GREEN_LED_SHIFT 19 // Pinul pentru LED-ul verde pe portul B
#define BLUE_LED_SHIFT 1  // Pinul pentru LED-ul albastru pe portul D
#define MASK(x) (1 << x)

void OutputPIN_Init(void){
  // Activeaza ceasul pentru porturile B si D
  SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;
  // Configureaza pinii ca iesiri GPIO
  PORTB->PCR[RED_LED_SHIFT] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[RED_LED_SHIFT] |= PORT_PCR_MUX(1);
  PORTB->PCR[GREEN_LED_SHIFT] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[GREEN_LED_SHIFT] |= PORT_PCR_MUX(1);
  PORTD->PCR[BLUE_LED_SHIFT] &= ~PORT_PCR_MUX_MASK;
  PORTD->PCR[BLUE_LED_SHIFT] |= PORT_PCR_MUX(1);

  // Seteaza pinii ca iesiri
  PTB->PDDR |= MASK(RED_LED_SHIFT) | MASK(GREEN_LED_SHIFT);
  PTD->PDDR |= MASK(BLUE_LED_SHIFT);

}

void getWhite()
{
	UART0_TransmitString("WHITE\r\n");
	PTB->PCOR |= MASK(RED_LED_SHIFT) | MASK(GREEN_LED_SHIFT);
	PTD->PCOR |= MASK(BLUE_LED_SHIFT);
}		


void getGreen()
{
	UART0_TransmitString("GREEN\r\n");
	PTB->PSOR = MASK(RED_LED_SHIFT);  
  PTD->PSOR = MASK(BLUE_LED_SHIFT);
}

void getBlue()
{
	UART0_TransmitString("BLUE\r\n");
	PTB->PSOR = MASK(GREEN_LED_SHIFT);
	PTB->PSOR = MASK(RED_LED_SHIFT);
	PTD->PCOR |= MASK(BLUE_LED_SHIFT);
}

void getMagenta()
{
	UART0_TransmitString("MAGENTA\r\n");
	PTB->PCOR |= MASK(RED_LED_SHIFT) | MASK(GREEN_LED_SHIFT);
	PTD->PCOR |= MASK(BLUE_LED_SHIFT);
  PTB->PSOR = MASK(GREEN_LED_SHIFT); 
}