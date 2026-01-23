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
#include "portmacro.h"

TaskHandle_t keypadrow_control_task= NULL;

TaskHandle_t keypadcol_control_task= NULL;


static void IRAM_ATTR gpio_isr_row_keypad_handler(void* arg)
{

BaseType_t xHigherPriorityTaskWoken;

xHigherPriorityTaskWoken= pdFALSE;

vTaskNotifyGiveFromISR(keypadrow_control_task, &xHigherPriorityTaskWoken);

portYIELD_FROM_ISR_ARG(xHigherPriorityTaskWoken);

}


void gpio_keypad_4x4_config (){

// THE THIRD COLUMN AND THE THIRD ROW ARE SPECIAL FUNCTIONS
// THE THIRD ROW ENABLE THE OTHERS,
// SO WILL USE INTERRUOTION TO DONT NEED TO SCAN UNTIL USER CALL MENU.
 
    gpio_config_t io_conf = {};

    //interrupt highlevel

    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = INPUT_ROW_SPECIALK_MASK;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;

    gpio_config(&io_conf);

    gpio_set_intr_type(GPIO_KEYPADROW3, GPIO_INTR_HIGH_LEVEL);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    gpio_isr_handler_add(GPIO_KEYPADROW3, gpio_isr_row_keypad_handler, (void*) gpio_isr_row_keypad_handler);


    io_conf.intr_type = GPIO_INTR_DISABLE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = INPUT_ROW_NUMBERS_MASK;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;

    gpio_config(&io_conf);

    io_conf.intr_type = GPIO_INTR_DISABLE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = OUTPUT_COL_NUMBERS_MASK;
    //set as input mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;

    gpio_config(&io_conf);


}


void special_keypad_row (void * pvparamenters){

ulTaskNotifyTake(pdTRUE,portMAX_DELAY);

}



