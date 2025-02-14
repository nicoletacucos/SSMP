#include "MKL25Z4.h"
#include "stdio.h"


void UART0_Transmit(uint8_t data); // Functie folostia pentru a trimite un octet catre interfata UART
uint8_t UART0_Receive(void); // Functie ce returneaza un octet de pe interfata UART, atunci cand acesta exista un buffer
void UART0_TransmitDecimalNr(uint8_t byte);
void UART0_Init(uint32_t baud_rate); // Initializare a modulului UART pentru trasmiterea datelor, ce primeste ca parametru baud rate-ul dorit
void UART0_TransmitString();
void UART0_TransmitInt16(int16_t);