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
#include "gpio_init.c"

spi_device_handle_t* spi;

ADC_handler_t adc1[5];


timer_def_t timers_config;

mcpwm_timer_handle_t *timerstotask =&timers_config.timers[0];

mcpwm_cmpr_handle_t *comparatorstotask = &timers_config. comparatorsBoosters[0] ;


void app_main(void)
{

	psi_setup();

	timer_setup (timers_config);

	adc_setup(adc1[0],adc1[1],adc1[2],adc1[3],adc1[4]); 

	gpio_booster_config ();

    xTaskCreatePinnedToCore(display_init, "display_init", (128*160*sizeof(uint16_t)+1024), &spi, TASK_PRIO_4, &xtaskHandleDisplay , CORE0);

    xTaskCreatePinnedToCore(display_frames, "display_frames", (30*30*sizeof(uint16_t)),NULL , TASK_PRIO_4, &xtaskHandleFrame, CORE1);

// TIMERS FOR BOOSTER WILL START AFTER ADC SIGNAL AND AT PRIORITY 3
// AFTER START AT MINIMUN PWM DUTY CYCLE WILL CHANGE USING THE FUNCTION   
//  mcpwm_comparator_set_compare_value(comparatorsMosfets[0], new_value_booster));

    xTaskCreatePinnedToCore(timer_mosfet_start, "Mosfet_signal_start", 4096 ,timers_config.timers[0] , TASK_PRIO_2, NULL, CORE0);

    xTaskCreatePinnedToCore(adc_one_shoot_reading, "adc_reading", 4096 ,&adc1[0] , TASK_PRIO_1, NULL, CORE0);

	xTaskCreatePinnedToCore(booster_selection, "pwm_control", 4096 ,&timerstotask , TASK_PRIO_2, &booster_control_task, CORE1);
    
	xTaskCreatePinnedToCore(pwm_control, "pwm_control", 4096 ,&comparatorstotask , TASK_PRIO_1, &pwm_control_task, CORE1);

	xTaskCreatePinnedToCore(display_update, "display_update", (30*30*sizeof(uint16_t)) ,&pointer_ADC_result , TASK_PRIO_0,NULL , tskNO_AFFINITY);


}
