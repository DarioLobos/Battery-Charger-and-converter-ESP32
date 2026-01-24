/*
 * gio_keypad.c
 *
 *  Created on: Jan 23, 2026
 *      Author: dario
 */

#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
# include "gpio_init.c"
#include "hal/gpio_types.h"
#include "soc/gpio_struct.h"
#include "soc/gpio_reg.h" // Optional: for direct register definitions

#include "portmacro.h"
#include <stdint.h>

TaskHandle_t xtaskHandleSetTime= NULL;


static void IRAM_ATTR gpio_isr_keypad_handler1(void* arg)
{

BaseType_t xHigherPriorityTaskWoken;

xHigherPriorityTaskWoken= pdFALSE;


xTaskNotifyFromISR(xtaskHandleSetTime,1,eSetValueWithoutOverwrite ,&xHigherPriorityTaskWoken);

portYIELD_FROM_ISR_ARG(xHigherPriorityTaskWoken);

}

static void IRAM_ATTR gpio_isr_keypad_handler2(void* arg)
{

BaseType_t xHigherPriorityTaskWoken;

xHigherPriorityTaskWoken= pdFALSE;

xTaskNotifyFromISR(xtaskHandleSetTime,2,eSetValueWithoutOverwrite ,&xHigherPriorityTaskWoken);

portYIELD_FROM_ISR_ARG(xHigherPriorityTaskWoken);

}

static void IRAM_ATTR gpio_isr_keypad_handler3(void* arg)
{

BaseType_t xHigherPriorityTaskWoken;

xHigherPriorityTaskWoken= pdFALSE;

xTaskNotifyFromISR(xtaskHandleSetTime,3,eSetValueWithoutOverwrite ,&xHigherPriorityTaskWoken);

portYIELD_FROM_ISR_ARG(xHigherPriorityTaskWoken);

}

static void IRAM_ATTR gpio_isr_keypad_handler4(void* arg)
{

BaseType_t xHigherPriorityTaskWoken;

xHigherPriorityTaskWoken= pdFALSE;

xTaskNotifyFromISR(xtaskHandleSetTime,4,eSetValueWithoutOverwrite ,&xHigherPriorityTaskWoken);

portYIELD_FROM_ISR_ARG(xHigherPriorityTaskWoken);

}


void gpio_keypad_4x3_config(void){

// THE THIRD COLUMN AND THE THIRD ROW ARE SPECIAL FUNCTIONS
// THE THIRD ROW ENABLE THE OTHERS,
// SO WILL USE INTERRUOTION TO DONT NEED TO SCAN UNTIL USER CALL MENU.

    //interrupt rising edge


    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = io_conf.pin_bit_mask | INPUT_ROW_NUMBERS_MASK;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;

    gpio_config(&io_conf);

    gpio_set_intr_type(GPIO_KEYPADROW0, GPIO_INTR_POSEDGE);
    gpio_set_intr_type(GPIO_KEYPADROW1, GPIO_INTR_POSEDGE);
    gpio_set_intr_type(GPIO_KEYPADROW2, GPIO_INTR_POSEDGE);
    gpio_set_intr_type(GPIO_KEYPADROW3, GPIO_INTR_POSEDGE);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    gpio_isr_handler_add(GPIO_KEYPADROW0, gpio_isr_keypad_handler1, (void*) gpio_isr_keypad_handler1);
    gpio_isr_handler_add(GPIO_KEYPADROW1, gpio_isr_keypad_handler2, (void*) gpio_isr_keypad_handler2);
    gpio_isr_handler_add(GPIO_KEYPADROW2, gpio_isr_keypad_handler3, (void*) gpio_isr_keypad_handler3);
    gpio_isr_handler_add(GPIO_KEYPADROW3, gpio_isr_keypad_handler4, (void*) gpio_isr_keypad_handler4);

    io_conf.intr_type = GPIO_INTR_DISABLE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = OUTPUT_COL_NUMBERS_MASK;
    //set as input mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;

    gpio_config(&io_conf);

GPIO.out_w1ts= GPIO.out_w1ts | GPIO_KEYPADCOL0;

}





