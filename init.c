#include <stm32f10x_flash.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_adc.h>
#include <stm32f10x.h>
#include <stm32f10x_dma.h>
#include <stm32f10x_spi.h>
#include <misc.h>
#include "init.h"
#include "radio.h"

SPI_InitTypeDef   SPI_InitStructure;
USART_InitTypeDef USART_InitStructure;
GPIO_InitTypeDef GPIO_Conf;
ADC_InitTypeDef ADC_InitStructure;
DMA_InitTypeDef DMA_InitStructure;


#define ADC1_DR_Address    ((uint32_t)0x4001244C)
#if defined(STM32F10X_CL)
#error "clock oscillator problem!"
#endif
void init_usart_gps(const uint32_t speed, const uint8_t enable_irq) {
  NVIC_DisableIRQ(USART1_IRQn);
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	USART_ClearITPendingBit(USART1, USART_IT_ORE);

  USART_Cmd(USART1, DISABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  USART_InitStructure.USART_BaudRate = speed;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(USART1, &USART_InitStructure);

  NVIC_InitTypeDef NVIC_InitStructure; 							//create NVIC structure
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  USART_Cmd(USART1, ENABLE);
  if (enable_irq){
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    NVIC_EnableIRQ(USART1_IRQn);
  }
}

void init_usart_debug() {
  NVIC_DisableIRQ(USART3_IRQn);
  USART_Cmd(USART3, DISABLE);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  USART_InitStructure.USART_BaudRate = 19200; //0x9c4;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(USART3, &USART_InitStructure);
  USART_Cmd(USART3, ENABLE);
}

void NVIC_Conf()
{
#ifdef  VECT_TAB_RAM
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  // VECT_TAB_FLASH
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif
}

void RCC_Conf()
{
	 ErrorStatus HSEStartUpStatus;
	  RCC_DeInit();
	  RCC_HSEConfig(RCC_HSE_ON);
	  HSEStartUpStatus = RCC_WaitForHSEStartUp();
	  if(HSEStartUpStatus == SUCCESS)
	  {
			FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
			FLASH_SetLatency(FLASH_Latency_2);
			RCC_HCLKConfig(RCC_SYSCLK_Div4);
			RCC_PCLK2Config(RCC_HCLK_Div4);
			RCC_PCLK1Config(RCC_HCLK_Div2);
			RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
			while(RCC_GetSYSCLKSource() != 0x04);
  }
}

void init_port()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_Conf.GPIO_Pin = GPIO_Pin_12;
	GPIO_Conf.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Conf.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_Conf);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  	GPIO_Conf.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
  	GPIO_Conf.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Conf.GPIO_Speed = GPIO_Speed_10MHz;
  	GPIO_Init(GPIOB, &GPIO_Conf);

	// SPI2_SCK & SPI2_MOSI
    GPIO_Conf.GPIO_Pin = GPIO_Pin_13 | radioSDIpin;
 	GPIO_Conf.GPIO_Mode = GPIO_Mode_AF_PP;
 	GPIO_Conf.GPIO_Speed = GPIO_Speed_10MHz;
 	GPIO_Init(GPIOB, &GPIO_Conf);

	// SPI2_MISO
 	GPIO_Conf.GPIO_Pin = GPIO_Pin_14;
 	GPIO_Conf.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 	GPIO_Init(GPIOB, &GPIO_Conf);

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	// radioNSELpin
 	GPIO_Conf.GPIO_Pin = radioNSELpin;
 	GPIO_Conf.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_Conf.GPIO_Speed = GPIO_Speed_10MHz;
 	GPIO_Init(GPIOC,&GPIO_Conf);

  spi_init();

	GPIO_Conf.GPIO_Pin = GPIO_Pin_9;
	GPIO_Conf.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Conf.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_Conf);
	GPIO_Conf.GPIO_Pin = GPIO_Pin_10;
	GPIO_Conf.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_Conf);

  init_usart_gps(9600, 0);

	GPIO_Conf.GPIO_Pin = GPIO_Pin_10;
	GPIO_Conf.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Conf.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_Conf);
	GPIO_Conf.GPIO_Pin = GPIO_Pin_11;
	GPIO_Conf.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_Conf);

  init_usart_debug();

	RCC_AHBPeriphClockCmd ( RCC_AHBPeriph_DMA1 , ENABLE ) ;
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_BufferSize = 2;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &ADCVal;
    ADC_DMACmd(ADC1, ENABLE);
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel1, ENABLE);
	GPIO_Conf.GPIO_Mode  = GPIO_Mode_AIN;
	GPIO_Conf.GPIO_Pin   = GPIO_Pin_6 ;							// that's ADC1 (PA5 on STM32)
	GPIO_Init(GPIOA, &GPIO_Conf);
	GPIO_Conf.GPIO_Mode  = GPIO_Mode_AIN;
	GPIO_Conf.GPIO_Pin   = GPIO_Pin_5 ;							// that's ADC1 (PA3 on STM32)
	GPIO_Init(GPIOA, &GPIO_Conf);
	RCC_ADCCLKConfig (RCC_PCLK2_Div2);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			// we work in continuous sampling mode
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 2;
	ADC_Init ( ADC1, &ADC_InitStructure);						//set config of ADC1
	ADC_RegularChannelConfig(ADC1,ADC_Channel_5, 1,ADC_SampleTime_28Cycles5); // define regular conversion config
	ADC_RegularChannelConfig(ADC1,ADC_Channel_6, 2,ADC_SampleTime_28Cycles5); // define regular conversion config
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd (ADC1,ENABLE);										//enable ADC
	ADC_ResetCalibration(ADC1);									// Reset previous calibration
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);									// start new calibration (ADC must be off at that time)
	while(ADC_GetCalibrationStatus(ADC1));
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);						// start conversion (will be endless as we are in continuous mode)
}

void spi_init() {
  GPIO_Conf.GPIO_Pin = radioSDIpin;
  GPIO_Conf.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Conf.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOB, &GPIO_Conf);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);
  SPI_SSOutputCmd(SPI2, ENABLE);
  SPI_Cmd(SPI2, ENABLE);
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(SPI2, &SPI_InitStructure);
}

void spi_deinit() {
  SPI_I2S_DeInit(SPI2);
  GPIO_Conf.GPIO_Pin = radioSDIpin;
  GPIO_Conf.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Conf.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOB, &GPIO_Conf);

}

void init_timer(const int rtty_speed) {
  TIM_TimeBaseInitTypeDef TIM2_TimeBaseInitStruct;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2, DISABLE);

  TIM2_TimeBaseInitStruct.TIM_Prescaler = 6 - 1;				// tick every 1/1000000 s
  TIM2_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM2_TimeBaseInitStruct.TIM_Period = (uint16_t) ((1000000 / rtty_speed) - 1);
  TIM2_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM2_TimeBaseInitStruct.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM2,&TIM2_TimeBaseInitStruct);
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);
  NVIC_InitTypeDef NVIC_InitStructure; 							//create NVIC structure
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  TIM_Cmd(TIM2,ENABLE);
}
