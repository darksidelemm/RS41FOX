#include <stm32f10x.h>
#include <core_cm3.h>
#include <stm32f10x_rcc.h>
#include <misc.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_dma.h>
#include <stdbool.h>
#include "pwm.h"
#include "config.h"
#include "radio.h"

DMA_Channel_TypeDef *pwm_dma_channel = DMA1_Channel2;

void pwm_timer_init()
{
    TIM_DeInit(TIM15);
    // GPIO_PinRemapConfig(GPIO_Remap_TIM15, DISABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM15, DISABLE);

    TIM_TimeBaseInitTypeDef tim_init;

    // Not needed: TIM_InternalClockConfig(TIM15);

    tim_init.TIM_Prescaler = 6 - 1; // tick every 1 us
    tim_init.TIM_CounterMode = TIM_CounterMode_Up;
    tim_init.TIM_Period = (uint16_t) (1000000 / (FM_TONE_FREQ * 4)) - 1;
    tim_init.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_init.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM15, &tim_init);

    // TIM_OCInitTypeDef TIM15_OCInitStruct;

    // TIM_OCStructInit(&TIM15_OCInitStruct);
    // TIM15_OCInitStruct.TIM_Pulse = 0; // Was: tim_init.TIM_Period / 2
    // TIM15_OCInitStruct.TIM_OCMode = TIM_OCMode_Toggle; // Was: TIM_OCMode_PWM1
    // TIM15_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    // TIM15_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;

    // // TIM15 channel 2 can be used to drive pin PB15, which is connected to RS41 Si4032 SDI pin for direct modulation
    // TIM_OC2Init(TIM15, &TIM15_OCInitStruct);

    // // These are not needed?
    // // TIM_SelectOCxM(TIM15, TIM_Channel_2, TIM_OCMode_PWM1);
    // // TIM_CCxCmd(TIM15, TIM_Channel_2, TIM_CCx_Enable);

    // // The following settings make transitions between generated frequencies smooth
    // TIM_ARRPreloadConfig(TIM15, ENABLE);
    // TIM_OC2PreloadConfig(TIM15, TIM_OCPreload_Enable);

    // TIM_OC2FastConfig(TIM15, TIM_OCFast_Enable);

    // TIM_CtrlPWMOutputs(TIM15, DISABLE);

    TIM_ClearITPendingBit(TIM15, TIM_IT_Update);
    TIM_ITConfig(TIM15, TIM_IT_Update, ENABLE);

    NVIC_InitTypeDef nvic_init;
    nvic_init.NVIC_IRQChannel = TIM1_BRK_TIM15_IRQn;
    nvic_init.NVIC_IRQChannelPreemptionPriority = 2;
    nvic_init.NVIC_IRQChannelSubPriority = 1;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init);

    TIM_Cmd(TIM15, ENABLE);
}

void pwm_timer_pwm_enable(bool enabled)
{
        TIM_Cmd(TIM15, enabled ? ENABLE : DISABLE);
    // TIM_CtrlPWMOutputs(TIM15, enabled ? ENABLE : DISABLE);
}

void pwm_timer_use(bool use)
{
    // Remapping the TIM15 outputs will allow TIM15 channel 2 can be used to drive pin PB15,
    // which is connected to RS41 Si4032 SDI pin for direct modulation
    // GPIO_PinRemapConfig(GPIO_Remap_TIM15, use ? ENABLE : DISABLE);
}

void pwm_timer_uninit()
{
    TIM_CtrlPWMOutputs(TIM15, DISABLE);
    TIM_Cmd(TIM15, DISABLE);

    TIM_DeInit(TIM15);

    // GPIO_PinRemapConfig(GPIO_Remap_TIM15, DISABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, DISABLE);
}

inline uint16_t pwm_calculate_period(uint32_t frequency_hz)
{
    return (uint16_t) (1000000 / frequency_hz) - 1;
}

inline void pwm_timer_set_frequency(uint32_t pwm_period)
{
    TIM_SetAutoreload(TIM15, pwm_period);
}

void TIM1_BRK_TIM15_IRQHandler(void)
{
    static bool pin_state = false;

    if (TIM_GetITStatus(TIM15, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM15, TIM_IT_Update);
        // Restrict the interrupt to DFM17 only just in case this ISR gets called on RS41
        pin_state = !pin_state;
        GPIO_WriteBit(GPIOB, radioSDIpin, pin_state);
    }
}