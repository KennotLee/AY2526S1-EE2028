/*
 * Notes.cpp
 *
 *  Created on: Oct 29, 2025
 *      Author: kenne
 */

#include "buzzer.h"

static TIM_HandleTypeDef htim_buzz;
static uint32_t start_tick = 0;
static uint32_t stop_tick = 0;
void Buzzer_Init(void)
{
    // GPIO AF for timer channel
    BUZZER_GPIO_CLK_ENABLE();
    GPIO_InitTypeDef g = {0};
    g.Pin       = BUZZER_Pin;
    g.Mode      = GPIO_MODE_AF_PP;
    g.Pull      = GPIO_NOPULL;
    g.Speed     = GPIO_SPEED_FREQ_LOW;
    g.Alternate = BUZZER_AF;
    HAL_GPIO_Init(BUZZER_GPIO_Port, &g);

    // Timer base: 1 MHz tick for easy math (assumes core clocks ~80 MHz)
    BUZZER_TIM_CLK_ENABLE();
    htim_buzz.Instance           = BUZZER_TIMER;
    htim_buzz.Init.Prescaler     = 79;                 // 80 MHz / (79+1) = 1 MHz
    htim_buzz.Init.CounterMode   = TIM_COUNTERMODE_UP;
    htim_buzz.Init.Period        = 1000 - 1;          // default 1 kHz
    htim_buzz.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim_buzz.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&htim_buzz);

    TIM_OC_InitTypeDef sOC = {0};
    sOC.OCMode       = TIM_OCMODE_PWM1;
    sOC.Pulse        = 0;                             // start silent
    sOC.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sOC.OCFastMode   = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim_buzz, &sOC, BUZZER_CHANNEL);
    HAL_TIM_PWM_Start(&htim_buzz, BUZZER_CHANNEL);
}
//974
//
void Buzzer_Tone(uint32_t hz)
{
    if (hz == 0) {
        __HAL_TIM_SET_COMPARE(&htim_buzz, BUZZER_CHANNEL, 0);
        return;
    }
    uint32_t arr = (1000000UL / hz);
    if (arr < 2) arr = 2;        // limit
    arr -= 1;

    __HAL_TIM_SET_AUTORELOAD(&htim_buzz, arr);
    __HAL_TIM_SET_COMPARE(&htim_buzz, BUZZER_CHANNEL, (arr + 1) / 2); // 50% duty
    __HAL_TIM_SET_COUNTER(&htim_buzz, 0);
}

void Buzzer_ToneDuty(uint32_t hz, uint8_t duty_percent)
{
    if (hz == 0 || duty_percent == 0) {
        __HAL_TIM_SET_COMPARE(&htim_buzz, BUZZER_CHANNEL, 0);
        return;
    }
    if (duty_percent > 99) duty_percent = 99;

    uint32_t arr = (1000000UL / hz);
    if (arr < 2) arr = 2;
    arr -= 1;

    __HAL_TIM_SET_AUTORELOAD(&htim_buzz, arr);
    uint32_t ccr = ((arr + 1) * duty_percent) / 100;
    if (ccr == 0) ccr = 1;
    __HAL_TIM_SET_COMPARE(&htim_buzz, BUZZER_CHANNEL, ccr);
    __HAL_TIM_SET_COUNTER(&htim_buzz, 0);
}

void Buzzer_Stop(void)
{
    __HAL_TIM_SET_COMPARE(&htim_buzz, BUZZER_CHANNEL, 0);
}

void Buzzer_Beep(uint32_t hz, uint32_t start, uint32_t ms)
{
    Buzzer_Tone(hz);
    start_tick = start;
    stop_tick = start + ms;
}

void CheckBuzzer(uint32_t now){
	if (now  > stop_tick) {
		Buzzer_Stop();
	}
}
