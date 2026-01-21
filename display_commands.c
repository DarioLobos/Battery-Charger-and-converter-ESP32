/*
 * display_commands.c
 *
 *  Created on: Jan 16, 2026
 *      Author: dario Lobos
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "portmacro.h"
#include "display_commands.h"

// polling 8 bits for command and parameters 

void spi_polling(spi_device_handle_t spi, const uint8_t data, bool keep_cs_active)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length = 8;                   //Command is 8 bits
    t.tx_buffer = &data;             //The data is the command and next polling parameters
    t.user = (void*)0;              //D/C needs to be set to 0
    if (keep_cs_active) {
        t.flags = SPI_TRANS_CS_KEEP_ACTIVE;   //Keep CS active after data transfer
    }
    ret = spi_device_polling_transmit(spi, &t); //Transmit!
    assert(ret == ESP_OK);          //Should have had no issues.
}



void spi_transmit_isr(spi_device_handle_t spi,void* commands, void* data, bool keep_cs_active) {

    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length = 8;                   //Command is 8 bits
    t.tx_buffer = &commands;             //The the command as data and data  next transmit parameters
    t.user = (void*)0;              //D/C needs to be set to 0
    if (keep_cs_active) {
        t.flags = SPI_TRANS_CS_KEEP_ACTIVE;   //Keep CS active after data transfer
    }
    ret = spi_device_queue_trans(spi, &t, portMAX_DELAY); //Transmit!
    assert(ret == ESP_OK);          //Should have had no issues.

	spi_transaction_t *tresult=&t; // not used just to don't free the spi and unblock task to next send.

    spi_device_get_trans_result(spi,&tresult, portMAX_DELAY);
	

    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length = 8;                   //Command is 8 bits
    t.tx_buffer = &data;             //The data commands have been sent in previous transaction
    t.user = (void*)0;              //D/C needs to be set to 0
    if (keep_cs_active) {
        t.flags = SPI_TRANS_CS_KEEP_ACTIVE;   //Keep CS active after data transfer
    }

    ret = spi_device_queue_trans(spi, &t, portMAX_DELAY); //Transmit!
    assert(ret == ESP_OK);          //Should have had no issues.

    spi_device_get_trans_result(spi,&tresult, portMAX_DELAY);

    assert(ret == ESP_OK);          //Should have had no issues.

}