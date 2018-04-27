// STM32F100 and SI4032 RTTY transmitter
// released under GPL v.2 by anonymous developer
// enjoy and have a nice day
// ver 1.5a
#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_spi.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_adc.h>
#include <stm32f10x_rcc.h>
#include "stdlib.h"
#include <stdio.h>
#include <string.h>
#include <misc.h>
#include "f_rtty.h"
#include "init.h"
#include "config.h"
#include "radio.h"
#include "ublox.h"
#include "delay.h"
#include "util.h"
#include "morse.h"


// IO Pins Definitions. The state of these pins are initilised in init.c
#define GREEN  GPIO_Pin_7 // Inverted
#define RED  GPIO_Pin_8 // Non-Inverted (?)


// Telemetry Data to Transmit - used in RTTY & MFSK packet generation functions.
unsigned int send_count;        //frame counter
int voltage;
int8_t si4032_temperature;
GPSEntry gpsData;

char callsign[15] = {CALLSIGN};
char status[2] = {'N'};
char buf_rtty[200];

// Volatile Variables, used within interrupts.
volatile int adc_bottom = 2000;
volatile char flaga = 0; // GPS Status Flags
volatile int led_enabled = 1; // Flag to disable LEDs at altitude.

volatile unsigned char pun = 0;
volatile unsigned int cun = 10;
volatile unsigned char tx_on = 0;
volatile unsigned int tx_on_delay;
volatile unsigned char tx_enable = 0;
rttyStates send_rtty_status = rttyZero;
volatile char *tx_buffer;
volatile uint16_t current_mfsk_byte = 0;
volatile uint16_t packet_length = 0;
volatile uint16_t button_pressed = 0;
volatile uint8_t disable_armed = 0;


// Function Definitions
void collect_telemetry_data();
void send_rtty_message();
void send_morse_ident();
void power_down();
void check_supply_voltage();

/**
 * GPS data processing
 */
void USART1_IRQHandler(void) {
  if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
    ublox_handle_incoming_byte((uint8_t) USART_ReceiveData(USART1));
      }else if (USART_GetITStatus(USART1, USART_IT_ORE) != RESET) {
    USART_ReceiveData(USART1);
  } else {
    USART_ReceiveData(USART1);
  }
}


// Timer interrupt Handler.
// 
void TIM2_IRQHandler(void) {
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  }
  // Do nothing...

  if (tx_on) {
      // RTTY Symbol selection logic.
        send_rtty_status = send_rtty((char *) tx_buffer);

        if (!disable_armed){
          if (send_rtty_status == rttyEnd) {
            if (led_enabled) GPIO_SetBits(GPIOB, RED);
            if (*(++tx_buffer) == 0) {
              tx_on = 0;
              // Reset the TX Delay counter, which is decremented at the symbol rate.
              tx_on_delay = 1000 / (1000/BAUD_RATE);
              tx_enable = 0;
              //radio_disable_tx(); // Don't turn off the transmitter!
            }
          } else if (send_rtty_status == rttyOne) {
            radio_rw_register(0x73, RTTY_DEVIATION, 1);
            if (led_enabled) GPIO_SetBits(GPIOB, RED);
          } else if (send_rtty_status == rttyZero) {
            radio_rw_register(0x73, 0x00, 1);
            if (led_enabled) GPIO_ResetBits(GPIOB, RED);
          }
        }
  }
}

int main(void) {
  RCC_Conf();
  NVIC_Conf();
  init_port();

  init_timer(BAUD_RATE);

  delay_init();
  ublox_init();

  GPIO_SetBits(GPIOB, RED);
  // NOTE - Green LED is inverted. (Reset to activate, Set to deactivate)
  GPIO_SetBits(GPIOB, GREEN);
  USART_SendData(USART3, 0xc);

  radio_soft_reset();
  // setting RTTY TX frequency
  radio_set_tx_frequency(TRANSMIT_FREQUENCY);

  // Set the fine PLL offset register so our CW signal is on the 'high' tone.
  radio_rw_register(0x73, RTTY_DEVIATION, 1);

  // setting TX power
  radio_rw_register(0x6D, 00 | (TX_POWER & 0x0007), 1);

  // initial RTTY modulation
  radio_rw_register(0x71, 0x00, 1);

  // Temperature Value Offset
  radio_rw_register(0x13, 0xF0, 1);

  // Temperature Sensor Calibration
  radio_rw_register(0x12, 0x00, 1);

  // ADC configuration
  radio_rw_register(0x0f, 0x80, 1);

  // Why do we have to do this again?
  spi_init();
  radio_set_tx_frequency(TRANSMIT_FREQUENCY);   
  radio_rw_register(0x71, 0x00, 1);
  init_timer(BAUD_RATE);


  // Main Transmission Loop.
  while (1) {
    // Loop.
    send_morse_ident();
    _delay_ms(1000);

    for(int k = 0; k < ONOFF_REPEATS; k++){
      radio_enable_tx();
      _delay_ms(ON_TIME*1000);
      radio_disable_tx();
      _delay_ms(OFF_TIME*1000);
    }

    #ifdef LOW_VOLTAGE_BEACON
    send_rtty_message();
    #endif

    check_supply_voltage();
  }

}

// Possible power savings?
//NVIC_SystemLPConfig(NVIC_LP_SEVONPEND, DISABLE);
//__WFI();


void send_morse_ident(){
  // Grab battery voltage data
  voltage = ADCVal[0] * 600 / 4096;
  int _voltage_v = voltage/100;
  int _voltage_mv = voltage % 100;

  sprintf(buf_rtty, "DE %s FOX %d.%dV", callsign, _voltage_v, _voltage_mv);
  sendMorse(buf_rtty);
}


void send_rtty_message() {
  sprintf(buf_rtty,"THIS IS A TEST VK5QI\n");
  //Configure for transmit
  tx_buffer = buf_rtty;
  // Enable the radio, and set the tx_on flag to 1.
  start_bits = RTTY_PRE_START_BITS;
  radio_enable_tx();
  tx_on = 1;

  // Wait until transmit has finished.
  while(tx_on){
    NVIC_SystemLPConfig(NVIC_LP_SEVONPEND, DISABLE);
    __WFI();
  }
}


void collect_telemetry_data() {
  // Assemble and proccess the telemetry data we need to construct our RTTY and MFSK packets.
  send_count++;
  si4032_temperature = radio_read_temperature();
  voltage = ADCVal[0] * 600 / 4096;
  ublox_get_last_data(&gpsData);

  if (gpsData.fix >= 3) {
      flaga |= 0x80;
      // Disable LEDs if altitude is > 1000m. (Power saving? Maybe?)
      if ((gpsData.alt_raw / 1000) > 1000){
        led_enabled = 0;
      } else {
        led_enabled = 1;
      }
  } else {
      // No GPS fix.
      flaga &= ~0x80;
      led_enabled = 1; // Enable LEDs when there is no GPS fix (i.e. during startup)

      // Null out lat / lon data to avoid spamming invalid positions all over the map.
      gpsData.lat_raw = 0;
      gpsData.lon_raw = 0;
  }
}

void power_down(){
  // Pulsing GPIO 12 de-latches the power supply circuitry, 
  // killing power to the board.
  GPIO_SetBits(GPIOA, GPIO_Pin_12);
}

void check_supply_voltage(){
  voltage = ADCVal[0] * 600 / 4096;
  if( (float)(voltage)/100 < LOW_VOLTAGE_CUTOUT){
    sendMorse("LOW BATTERY");
    power_down();
  }
}
