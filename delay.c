#include <stm32f10x.h>
#include <core_cm3.h>
#include <stm32f10x_rcc.h>
#include <misc.h>
#include <stm32f10x_tim.h>
#include "delay.h"

volatile uint8_t done;

void delay_init() {
  TIM_TimeBaseInitTypeDef ts;
  // TIM3 @ APB1 -> 6MHz WTF?!
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3, DISABLE);

  ts.TIM_Prescaler = 6 - 1;
  ts.TIM_CounterMode = TIM_CounterMode_Up;
  ts.TIM_Period = 0;
  ts.TIM_ClockDivision = TIM_CKD_DIV1;
  ts.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM3,&ts);
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
  TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE);
  NVIC_InitTypeDef NVIC_InitStructure; //create NVIC structure
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  TIM_Cmd(TIM3, DISABLE);
}

void _delay_us(uint16_t us, uint8_t precise) {
  TIM_Cmd(TIM3, DISABLE);
  TIM_SetAutoreload(TIM3, us);
  TIM_SetCounter(TIM3, 0);
  TIM_Cmd(TIM3, ENABLE);
  done = 0;
  while(!done){

  }
  TIM_Cmd(TIM3, DISABLE);
}

inline void _delay_ms(uint32_t ms) {
  while(ms-- > 0){
    _delay_us(1000, 0);
  }
}

void TIM3_IRQHandler(void) {
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    done = 1;
  }
}

