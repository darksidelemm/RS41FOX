#ifndef __PWM_H_
#define __PWM_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stm32f10x.h>
#include <core_cm3.h>
#include <stm32f10x_rcc.h>
#include <misc.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_gpio.h>
#include <stdbool.h>
#include "pwm.h"
#include "config.h"

void pwm_timer_init();

void pwm_timer_pwm_enable(bool enabled);

void pwm_timer_use(bool use);

void pwm_timer_uninit();

uint16_t pwm_calculate_period(uint32_t frequency_hz);

void pwm_timer_set_frequency(uint32_t pwm_period);

#ifdef __cplusplus
}
#endif
#endif
