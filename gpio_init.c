/*
 * gpio_init.c
 *
 *  Created on: Jan 19, 2026
 *      Author: dario
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "config_main.h"
#include "hal/gpio_types.h"
#include "portmacro.h"
#include "hal/gpio_types.h"
#include "soc/gpio_struct.h"
#include "soc/gpio_reg.h" // Optional: for direct register definitions


TaskHandle_t booster_control_task= NULL;

gpio_config_t io_conf = {};

static volatile int32_t flag_photoresistor;

	static void IRAM_ATTR gpio_isr_handler_BOOSTER(void *arg) {

	BaseType_t xHigherPriorityTaskWoken;

	xHigherPriorityTaskWoken = pdFALSE;

	vTaskNotifyGiveFromISR(booster_control_task, &xHigherPriorityTaskWoken);

	portYIELD_FROM_ISR_ARG(xHigherPriorityTaskWoken);
}

	static void IRAM_ATTR gpio_isr_handler_PHOTO(void *arg) {

	BaseType_t xHigherPriorityTaskWoken;

	xHigherPriorityTaskWoken = pdFALSE;
	
	int32_t pin_status = REG_READ(GPIO_IN_REG);

	if ((pin_status & (1UL<<GPIO_PHOTOSWITCH))!=0){
	
	flag_photoresistor=1;

	}else {
	flag_photoresistor=0;
	}

	portYIELD_FROM_ISR_ARG(xHigherPriorityTaskWoken);
}


void gpio_booster_config (void ){

    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = io_conf.pin_bit_mask | INPUT_BOOSTER_MASK;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;

    gpio_config(&io_conf);

    gpio_set_intr_type(GPIO_INPUT_BOOSTER, GPIO_INTR_ANYEDGE);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    gpio_isr_handler_add(GPIO_INPUT_BOOSTER, gpio_isr_handler_BOOSTER, (void*) GPIO_INPUT_BOOSTER);


}

void gpio_photoresistor_config (void ){

    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = io_conf.pin_bit_mask | INPUT_PHOTORESISTOR_MASK ;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;

    gpio_config(&io_conf);

    gpio_set_intr_type(GPIO_PHOTOSWITCH, GPIO_INTR_ANYEDGE);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    gpio_isr_handler_add(GPIO_PHOTOSWITCH, gpio_isr_handler_PHOTO, (void*) GPIO_PHOTOSWITCH);


}
