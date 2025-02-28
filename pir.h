#ifndef PIR_H
#define PIR_H
#include "MKL05Z4.h"
#include <stdint.h>

void PIR_Init(void);
uint8_t PIR_Detected(void);

#endif // PIR_H