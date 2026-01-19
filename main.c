/*
 * display_commands.c
 *
 *  Created on: Jan 16, 2026
 *      Author: dario Lobos
 */

#include "main.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "driver/mcpwm_cmpr.h"
#include "driver/mcpwm_types.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "portmacro.h"
#include "driver/mcpwm_prelude.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "display_functions.c"
#include "mcpwm_init.c"
#include "adc_functions.c"

spi_device_handle_t* spi;
mcpwm_timer_handle_t timers[3];
ADC_handler_t adc1[5];

void booster_selector_read (void *pvparameter ){

gpio_install_isr_service(int intr_alloc_flags)
gpio_isr_handler_add(gpio_num_t gpio_num, gpio_isr_t isr_handler, void *args)

esp_err_t gpio_isr_register(void (*fn)(void*), void *arg, int intr_alloc_flags, gpio_isr_handle_t
*handle)
 
}

void app_main(void)
{

	psi_setup();

	timer_setup (timers[0],timers[1], timers[2] );

	adc_setup(adc1[0],adc1[1],adc1[2],adc1[3],adc1[4]); 

    xTaskCreatePinnedToCore(display_init, "display_init", (128*160*sizeof(uint16_t)+1024), &spi, TASK_PRIO_4, &xtaskHandleDisplay , CORE0);

    xTaskCreatePinnedToCore(display_frames, "display_frames", (30*30*sizeof(uint16_t)),NULL , TASK_PRIO_4, &xtaskHandleFrame, CORE1);

// TIMERS FOR BOOSTER WILL START AFTER ADC SIGNAL AND AT PRIORITY 3
// AFTER START AT MINIMUN PWM DUTY CYCLE WILL CHANGE USING THE FUNCTION   
//  mcpwm_comparator_set_compare_value(comparatorsMosfets[0], new_value_booster));

    xTaskCreatePinnedToCore(timer_mosfet_start, "Mosfet_signal_start", 4096 ,&timers[1] , TASK_PRIO_2, NULL, CORE1);

    xTaskCreatePinnedToCore(adc_one_shoot_reading, "adc_reading", 4096 ,&adc1[0] , TASK_PRIO_2, NULL, CORE0);

    xTaskCreatePinnedToCore(booster_selector_read, "booster_selector_read", 4096 ,&adc1 , TASK_PRIO_3, NULL, CORE0);

pending    
	xTaskCreatePinnedToCore(pwm_control, "pwm_control", 4096 ,NULL , TASK_PRIO_1, pwm_control_task, CORE1);

	xTaskCreatePinnedToCore(display_update, "display_update", (30*30*sizeof(uint16_t)) ,NULL , TASK_PRIO_0, pwm_control_task, tskNO_AFFINITY);


}
