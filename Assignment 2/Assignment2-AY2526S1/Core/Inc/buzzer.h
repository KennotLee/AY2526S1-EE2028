/*
 * Notes.h
 *
 *  Created on: Oct 29, 2025
 *      Author: kenne
 */

#ifndef BUZZER_H
#define BUZZER_H

#include "main.h"           // pulls in HAL + your board clocks

// ---- Hardware mapping (Grove Base Shield on D3) ----
#define BUZZER_GPIO_Port            GPIOB
#define BUZZER_Pin                  GPIO_PIN_0       // D3 = PB0
#define BUZZER_AF                   GPIO_AF2_TIM3    // PB0 uses AF2 for TIM3
#define BUZZER_TIMER                TIM3
#define BUZZER_CHANNEL              TIM_CHANNEL_3

// Clock helpers for portability
#define BUZZER_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()
#define BUZZER_TIM_CLK_ENABLE()     __HAL_RCC_TIM3_CLK_ENABLE()

// ---- Public API ----
void Buzzer_Init(void);                 // call once after HAL_Init()
void Buzzer_Tone(uint32_t hz);          // 0 = stop
void Buzzer_Stop(void);                 // stop tone
void Buzzer_Beep(uint32_t hz, uint32_t start, uint32_t ms); // blocking beep
void Buzzer_ToneDuty(uint32_t hz, uint8_t duty_percent); // optional "volume" (1-99)
void CheckBuzzer(uint32_t now);

// ---- Optional note constants (A4=440) ----
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_A5  880
#define NOTE_B5  988

#endif // BUZZER_H

