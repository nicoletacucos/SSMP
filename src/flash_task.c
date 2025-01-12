#define RED_LED_PIN    18  // Port B, Pin 18
#define GREEN_LED_PIN  19  // Port B, Pin 19
#define BLUE_LED_PIN   1   // Port D, Pin 1

void Control_RGB_LEDS(int r, int g, int b) {

	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;
	  // Configure LED pins as outputs
  PTB->PDDR |= (1 << RED_LED_PIN) | (1 << GREEN_LED_PIN);
  PTD->PDDR |= (1 << BLUE_LED_PIN);

  if (r) {
    PTB->PCOR |= (1 << RED_LED_PIN);   
  } else {
    PTB->PSOR |= (1 << RED_LED_PIN);   
  }

  if (g) {
    PTB->PCOR |= (1 << GREEN_LED_PIN); 
  } else {
    PTB->PSOR |= (1 << GREEN_LED_PIN); 
  }

  if (b) {
    PTD->PCOR |= (1 << BLUE_LED_PIN);  
  } else {
    PTD->PSOR |= (1 << BLUE_LED_PIN);  
  }
}

