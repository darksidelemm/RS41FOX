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
#include "init.h"
#include "config.h"
#include "radio.h"
#include "ublox.h"
#include "delay.h"
#include "util.h"
#include "morse.h"


// IO Pin Definitions. The state of these pins are initilised in init.c
#define GREEN  GPIO_Pin_7 // Inverted
#define RED  GPIO_Pin_8 // Non-Inverted (?)
#define SHUTDOWN  GPIO_Pin_12 // Set high to trigger the power circuitry to power down.


// Telemetry Data to Transmit
int voltage;
GPSEntry gpsData;
int gpsFixed = 0;
// String and Transmit Buffers
char callsign[15] = {CALLSIGN};
char buf_tx[200];


// Function Definitions
void collect_telemetry_data();
void send_rtty_message();
void send_morse_ident();
void power_down();
void check_supply_voltage();
void check_gps_lock();

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


int main(void) {
  RCC_Conf();
  NVIC_Conf();
  init_port();

  delay_init();
  ublox_init();

  GPIO_SetBits(GPIOB, RED);
  // NOTE - Green LED is inverted. (Reset to activate, Set to deactivate)
  GPIO_SetBits(GPIOB, GREEN);
  USART_SendData(USART3, 0xc);

  radio_soft_reset();
  // Set Morse TX Frequency
  radio_set_tx_frequency(TRANSMIT_FREQUENCY);

  // Set PLL offset to 0
  radio_rw_register(0x73, 0x00, 1);

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

  // For some reason we have to do this again...
  spi_init();
  radio_set_tx_frequency(TRANSMIT_FREQUENCY);   
  radio_rw_register(0x71, 0x00, 1);

  // If we aren't doing a low-voltage GPS position beacon,
  // just disable the GPS from the start.
  #ifndef LOW_VOLTAGE_BEACON
  ublox_gps_stop();
  #endif

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
    check_gps_lock();
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

  sprintf(buf_tx, "DE %s FOX %d.%dV", callsign, _voltage_v, _voltage_mv);
  sendMorse(buf_tx);
}


void power_down(){
  // Pulsing GPIO 12 de-latches the power supply circuitry, 
  // killing power to the board.
  GPIO_SetBits(GPIOA, SHUTDOWN);
}

void check_supply_voltage(){
  voltage = ADCVal[0] * 600 / 4096;

  #ifdef LOW_VOLTAGE_BEACON
  if( (float)(voltage)/100.0 < LOW_VOLTAGE_BEACON_THRESHOLD){
    // Send the calculated GPS position if we have GPS lock.
    if(gpsData.fix < 3){
      sendMorse("NO GPS LOCK ");
    } 
    else {
      // Convert raw lat/lon values into degrees and decimal degree values.
      uint8_t lat_d = (uint8_t) abs(gpsData.lat_raw / 10000000);
      uint32_t lat_fl = (uint32_t) abs(abs(gpsData.lat_raw) - lat_d * 10000000) / 1000;
      uint8_t lon_d = (uint8_t) abs(gpsData.lon_raw / 10000000);
      uint32_t lon_fl = (uint32_t) abs(abs(gpsData.lon_raw) - lon_d * 10000000) / 1000;

      sprintf(buf_tx, "VK5QI FOX %s%d.%04ld %s%d.%04ld ",        
          gpsData.lat_raw < 0 ? "S" : "N", lat_d, lat_fl,
          gpsData.lon_raw < 0 ? "W" : "E", lon_d, lon_fl
      );
      sendMorse(buf_tx);
    }
  }
  #endif

  if( (float)(voltage)/100 < LOW_VOLTAGE_CUTOUT){
    sendMorse("LOW BATTERY ");
    power_down();
  }
}

void check_gps_lock(){
  // If we have lock, return immediately.
  if(gpsFixed){
    return;
  } 

  // Otherwise, check to see if we have lock.
  else {
    ublox_get_last_data(&gpsData);
  }

  if(gpsData.fix >= 3){
    // We have GPS lock!
    sendMorse("GPS LOCK ");
    gpsFixed = 1;

    // Disable the GPS unit. This saves ~200mW of power.
    ublox_gps_stop();
  }

}
