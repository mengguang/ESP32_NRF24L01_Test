/*
* ----------------------------------------------------------------------------
* “THE COFFEEWARE LICENSE” (Revision 1):
* <ihsan@kehribar.me> wrote this file. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a coffee in return.
* -----------------------------------------------------------------------------
* Please define your platform spesific functions in this file ...
* -----------------------------------------------------------------------------
*/

#include <Arduino.h>
#include <SPI.h>
#include <hardware_config.h>

/* ------------------------------------------------------------------------- */

void nrf24_setupPins()
{
    pinMode(NRF_CE_PIN, OUTPUT);
    pinMode(NRF_CSN_PIN, OUTPUT);
    pinMode(NRF_IRQ_PIN, INPUT);
}

uint8_t spi_transfer(uint8_t tx)
{
    return SPI.transfer(tx);
}

/* ------------------------------------------------------------------------- */
void nrf24_ce_digitalWrite(uint8_t state)
{
    digitalWrite(NRF_CE_PIN, state);
}
/* ------------------------------------------------------------------------- */
void nrf24_csn_digitalWrite(uint8_t state)
{
    digitalWrite(NRF_CSN_PIN, state);
}
