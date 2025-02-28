#include "MKL05Z4.h"
#define LED_R_MASK		(1<<8)		// Maska dla diody czerwonej (R)
#define LED_G_MASK		(1<<9)		// Maska dla diody zielonej (G)
#define LED_B_MASK		(1<<10)		// Maska dla diody niebieskiej (B)
#define LED_R					8					// Numer bitu didody R
#define LED_G					9					// Numer bitu didody G
#define LED_B					10				// Numer bitu didody B

void LED_Init(void);
