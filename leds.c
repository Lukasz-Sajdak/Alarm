#include "leds.h"

void LED_Init(void)
{
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;      // Włączenie portu B
	PORTB->PCR[LED_R] |= PORT_PCR_MUX(1);
	PORTB->PCR[LED_G] |= PORT_PCR_MUX(1);	
	PORTB->PCR[LED_B] |= PORT_PCR_MUX(1);
	PTB->PDDR |= LED_R_MASK|LED_G_MASK|LED_B_MASK;	// Ustaw na 1 bity 8, 9 i 10 - rola jako wyjścia
	PTB->PDOR|= LED_R_MASK|LED_G_MASK|LED_B_MASK;	// Zgaś wszystkie diody
}
