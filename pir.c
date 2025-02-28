#include "pir.h"

void PIR_Init(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;     // Wlacz zegar dla portu A
    PORTA->PCR[5] = PORT_PCR_MUX(1);
    PTA->PDDR &= ~(1 << 5);
}

uint8_t PIR_Detected(void)
{
    // Sprawdz stan pinu PIR (1 - ruch wykryty, 0 - brak ruchu)
    return (PTA->PDIR & (1 << 5)) ? 1 : 0;
}