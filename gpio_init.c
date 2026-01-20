/*
 * gpio_init.c
 *
 *  Created on: Jan 19, 2026
 *      Author: dario
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "main.h"
#include "portmacro.h"

TaskHandle_t booster_control_task= NULL;


static void IRAM_ATTR gpio_isr_handler(void* arg)
{

BaseType_t xHigherPriorityTaskWoken;

xHigherPriorityTaskWoken= pdFALSE;

vTaskNotifyGiveFromISR(booster_control_task, &xHigherPriorityTaskWoken);

portYIELD_FROM_ISR_ARG(xHigherPriorityTaskWoken);

}


void gpio_booster_config (void ){

    gpio_config_t io_conf = {};
    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = INPUT_BOOSTER_MASK;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;

    gpio_config(&io_conf);

    gpio_set_intr_type(GPIO_INPUT_BOOSTER, GPIO_INTR_ANYEDGE);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    gpio_isr_handler_add(GPIO_INPUT_BOOSTER, gpio_isr_handler, (void*) GPIO_INPUT_BOOSTER);


}

