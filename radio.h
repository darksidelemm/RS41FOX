//
// Created by SQ5RWU on 2016-12-24.
//

#ifndef RS41HUP_RADIO_H
#define RS41HUP_RADIO_H

#include "config.h"
#include <stdint.h>
#include <stm32f10x_spi.h>
#include <stm32f10x_gpio.h>

static const uint16_t radioNSELpin = GPIO_Pin_13; // @ GPIOC
static const uint16_t radioSDIpin = GPIO_Pin_15; // @ GPIOB!
static const uint8_t WR = 0x80;
static const float SI4032_CLOCK = 26.0;

#ifdef __cplusplus
extern "C" {
#endif

uint8_t _spi_sendrecv(const uint16_t data_word);

uint8_t radio_rw_register(const uint8_t register_addr, uint8_t value, uint8_t write);

void radio_set_tx_frequency(const float radio_set_tx_frequency);

void radio_disable_tx();

void radio_soft_reset();

void radio_enable_tx();

void radio_inhibit_tx();

int8_t radio_read_temperature();

#ifdef __cplusplus
}
#endif

#endif //RS41HUP_RADIO_H
