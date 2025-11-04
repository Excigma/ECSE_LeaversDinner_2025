#ifndef TONEGEN_HPP
#define TONEGEN_HPP

#include <stdint.h>

#define TONEGEN_PWM_PIN 13

void ToneGen_Init(void);
void ToneGen_Enable(void);
void ToneGen_Sleep(void);
void ToneGen_WritePeriod(uint16_t period);
void ToneGen_WriteCompare(uint16_t compare);

#endif // TONEGEN_HPP
