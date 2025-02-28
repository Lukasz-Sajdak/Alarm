#include "frdm_bsp.h" 
#include "lcd1602.h"
#include "leds.h" 
#include "stdio.h"
#include "math.h"
#include "i2c.h"
#include "klaw.h"
#include "pir.h"

#define	ZYXDR_Mask	1<<3	// Maska bitu ZYXDR w rejestrze STATUS

volatile uint8_t S2_press=0;	// "1" - klawisz został wciśnięty "0" - klawisz "skonsumowany"
volatile uint8_t S3_press=0;
volatile uint8_t S4_press=0;
static uint8_t arrayXYZ[6];
static uint8_t sens;
static uint8_t status;
double X, Y, Z;
double baseX, baseY, baseZ; // Zapamiętana pozycja bazowa

static uint8_t code[] = {S2, S3, S4}; // Sekwencja klawiszy jako kod alarmu
static const uint8_t admin_code[] = {S4, S3, S2}; // Sekwencja klawiszy do trybu administratora
static uint8_t input_index = 0; // Indeks aktualnie wprowadzanego klawisza
static uint8_t admin_index = 0; // Indeks do sekwencji trybu administratora


volatile uint8_t alarm_armed = 0;
volatile uint8_t admin_mode = 0;

void PORTA_IRQHandler(void)	// Podprogram obsługi przerwania od klawiszy S2, S3 i S4
{
	uint32_t buf;
	buf=PORTA->ISFR & (S2_MASK | S3_MASK | S4_MASK);

	switch(buf)
	{
									
		case S2_MASK:	DELAY(100)
									if(!(PTA->PDIR&S2_MASK))		// Minimalizacja drgań zestyków
									{
										DELAY(100)
										if(!(PTA->PDIR&S2_MASK))	// Minimalizacja drgań zestyków (c.d.)
										{
											if(!S2_press)
											{
												S2_press=1;
											}
										}
									}
									break;
		case S3_MASK:	DELAY(100)
									if(!(PTA->PDIR&S3_MASK))		// Minimalizacja drgań zestyków
									{
										DELAY(100)
										if(!(PTA->PDIR&S3_MASK))	// Minimalizacja drgań zestyków (c.d.)
										{
											if(!S3_press)
											{
												S3_press=1;
											}
										}
									}
									break;
		case S4_MASK:	DELAY(100)
									if(!(PTA->PDIR&S4_MASK))		// Minimalizacja drgań zestyków
									{
										DELAY(100)
										if(!(PTA->PDIR&S4_MASK))	// Minimalizacja drgań zestyków (c.d.)
										{
											if(!S4_press)
											{
												S4_press=1;
											}
										}
									}
									break;
		default:			break;
	}	
	PORTA->ISFR |= S2_MASK | S3_MASK | S4_MASK;	// Kasowanie wszystkich bitów ISF
	NVIC_ClearPendingIRQ(PORTA_IRQn);
}

void check_admin_combination(void)
{
	if (S2_press || S3_press || S4_press) 
	{
		uint8_t pressed_key = 0;

		if (S2_press) pressed_key = S2;
		else if (S3_press) pressed_key = S3;
		else if (S4_press) pressed_key = S4;

		S2_press = S3_press = S4_press = 0; // Resetuj flagi klawiszy

		if (pressed_key == admin_code[admin_index]) 
		{
			// Poprawny klawisz trybu administratora
			admin_index++;
			
			if (admin_index == sizeof(admin_code)) 
			{
				// Poprawna sekwencja
				admin_index = 0;
				admin_mode = 1;
			}
		} 
		else 
		{
			// Błędny klawisz, reset sekwencji
			admin_index = 0;
		}
	}
}

void admin_mode_logic(void)
{
		PTB->PSOR |= LED_G_MASK;
		PTB->PSOR |= LED_R_MASK;
		PTB->PSOR |= LED_B_MASK;
	
    uint8_t new_code[sizeof(code)] = {0}; // Tymczasowy nowy kod
    uint8_t new_code_index = 0;          // Indeks wprowadzania nowego kodu

    LCD1602_ClearAll();
    LCD1602_Print("  TRYB ADMINA");
    LCD1602_SetCursor(0, 1);
    LCD1602_Print(" Wpisz nowy kod");

    while (new_code_index < sizeof(code))
    {
        if (S2_press || S3_press || S4_press) 
        {
            uint8_t pressed_key = 0;

            if (S2_press) pressed_key = S2;
            else if (S3_press) pressed_key = S3;
            else if (S4_press) pressed_key = S4;

            S2_press = S3_press = S4_press = 0; // Resetuj flagi klawiszy

            new_code[new_code_index] = pressed_key; // Zapisz klawisz do nowego kodu
            new_code_index++;

            // Aktualizacja ekranu
            LCD1602_ClearAll();
            LCD1602_Print("Wprowadzanie...");
            char buf[16];
						snprintf(buf, sizeof(buf), "Podano:     %d/%d", new_code_index, sizeof(code));
            LCD1602_SetCursor(0, 1);
            LCD1602_Print(buf);
        }
    }

    // Zapisanie nowego kodu
    for (uint8_t i = 0; i < sizeof(code); i++) 
    {
        ((uint8_t *)code)[i] = new_code[i];
    }

    // Potwierdzenie zmiany kodu
    LCD1602_ClearAll();
    LCD1602_Print("Kod zmieniony!");
    DELAY(20000);
		
		admin_mode = 0;
		alarm_armed = 0;
		LCD1602_ClearAll();
		LCD1602_Print("Alarm Rozbrojony");
}

void check_alarm_combination(void)
{
	if (S2_press || S3_press || S4_press) 
	{
		uint8_t pressed_key = 0;

		if (S2_press) pressed_key = S2;
		else if (S3_press) pressed_key = S3;
		else if (S4_press) pressed_key = S4;

		S2_press = S3_press = S4_press = 0; // Resetuj flagi klawiszy

		if (pressed_key == code[input_index]) 
		{
			// Poprawny klawisz
			input_index++;
			LCD1602_ClearAll();
			LCD1602_Print("Wprowadzanie...");
			
			if (input_index == sizeof(code)) 
			{
				// Wpisano poprawny kod
				input_index = 0; // Reset indeksu
				alarm_armed = !alarm_armed; // Zmiana stanu alarmu
				
				LCD1602_ClearAll();
				if (alarm_armed) 
				{
					LCD1602_Print("Uzbrajanie...");
					DELAY(20000);
					LCD1602_ClearAll();
					LCD1602_Print("Alarm Uzbrojony");
					I2C_ReadRegBlock(0x1d, 0x1, 6, arrayXYZ);
					baseX = ((double)((int16_t)((arrayXYZ[0] << 8) | arrayXYZ[1]) >> 2) / (4096 >> sens));
					baseY = ((double)((int16_t)((arrayXYZ[2] << 8) | arrayXYZ[3]) >> 2) / (4096 >> sens));
					baseZ = ((double)((int16_t)((arrayXYZ[4] << 8) | arrayXYZ[5]) >> 2) / (4096 >> sens));
				} 
				else 
				{
					LCD1602_Print("Alarm Rozbrojony");
				}
			}
		} 
		else 
		{
			// Błędny klawisz
			LCD1602_ClearAll();
			LCD1602_PrintPL("    B//l//edna");
			LCD1602_SetCursor(0, 1);
			LCD1602_Print("  kombinacja");
			input_index = 0; // Resetuj indeks
			DELAY(20000);
			LCD1602_ClearAll();
			LCD1602_PrintPL("Spr//obuj ponownie");
		}
	}
}

int main (void)
{ 
	char display[]={0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
	
	Klaw_Init();
	Klaw_S1_4_Int();
	LED_Init();
	PIR_Init();
	LCD1602_Init();
	LCD1602_Backlight(TRUE);
	LCD1602_PL_CH();
	PIR_Init();
	PIR_Detected();

	sens=0;	// Wybór czułości: 0 - 2g; 1 - 4g; 2 - 8g (akcelerometr)
	I2C_WriteReg(0x1d, 0x2a, 0x0);	// ACTIVE=0 - stan czuwania
	I2C_WriteReg(0x1d, 0xe, sens);	 		// Ustaw czułość zgodnie ze zmienną sens
	I2C_WriteReg(0x1d, 0x2a, 0x1);	 		// ACTIVE=1 - stan aktywny
	
	LCD1602_SetCursor(0, 0);
	LCD1602_Print("   Oczekiwanie");
	LCD1602_SetCursor(0, 1);
	LCD1602_PrintPL("    na akcj//e");
	
	while(1)
	{
		// Sprawdzanie kombinacji do uzbrajania/rozbrajania alarmu
		if (admin_mode) 
		{
			admin_mode_logic();
		} 
		else 
		{
			if(!(PTA->PDIR&S1_MASK))
				{
				PTB->PCOR |= LED_G_MASK;
				PTB->PSOR |= LED_R_MASK;
				PTB->PSOR |= LED_B_MASK;
				check_admin_combination();
			}
			else
			{
				PTB->PSOR |= LED_G_MASK;
				check_alarm_combination();
			}
		}
		
		I2C_ReadReg(0x1d, 0x0, &status);
		status&=ZYXDR_Mask;
		
		
		if(status)	// Jeśli dane gotowe do odczytu
		{
			I2C_ReadRegBlock(0x1d, 0x1, 6, arrayXYZ);
			X=((double)((int16_t)((arrayXYZ[0]<<8)|arrayXYZ[1])>>2)/(4096>>sens));
			Y=((double)((int16_t)((arrayXYZ[2]<<8)|arrayXYZ[3])>>2)/(4096>>sens));
			Z=((double)((int16_t)((arrayXYZ[4]<<8)|arrayXYZ[5])>>2)/(4096>>sens));

			// Aktywacja diody po poruszeniu płytką, jeśli alarm uzbrojony
			if (alarm_armed && (fabs(X - baseX) > 0.1 || fabs(Y - baseY) > 0.1 || fabs(Z - baseZ) > 0.1))
			{
				LCD1602_ClearAll();
				LCD1602_PrintPL("Uwaga z//lodziej!");
				PTB->PCOR |= LED_R_MASK;	// Włącz czerwoną diodę LED (alarm)
			}
			else if (alarm_armed && PIR_Detected())
			{
				LCD1602_SetCursor(0, 1);
				LCD1602_Print("Wykryto ruch!");
				PTB->PCOR |= LED_B_MASK;
			}
			else if (!PIR_Detected() && alarm_armed)
			{
				LCD1602_SetCursor(0, 1);
				LCD1602_Print("              ");
				PTB->PSOR |= LED_B_MASK;	
			}	
			if (!alarm_armed)
			{
				PTB->PSOR |= LED_R_MASK;	// Wyłącz czerwoną diodę LED
				PTB->PSOR |= LED_B_MASK;	
			}
		}
	}
}
