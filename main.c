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
#include "display_functions.c"

void app_main(void)
{

	psi_setup();

	adc_setup(); 

	gpio_booster_config ();

    xTaskCreatePinnedToCore(display_init, "display_init", (128*160*sizeof(uint16_t)+1024), NULL, TASK_PRIO_4, &xtaskHandleDisplay , CORE0);

    xTaskCreatePinnedToCore(display_frames, "display_frames", (64*5*sizeof(uint16_t))*3+(64*8*sizeof(uint16_t))+4096,NULL , TASK_PRIO_4, &xtaskHandleFrame, CORE1);

    xTaskCreatePinnedToCore(timer_mosfet_start, "Mosfet_signal_start", 4096 ,NULL , TASK_PRIO_3, NULL, CORE0);

    xTaskCreatePinnedToCore(adc_one_shoot_AC_reading, "adc_reading", 4096 ,NULL , TASK_PRIO_2, NULL, CORE0);

	xTaskCreatePinnedToCore(booster_selection, "pwm_control", 4096 ,NULL , TASK_PRIO_3, &booster_control_task, CORE1);
    
	xTaskCreatePinnedToCore(pwm_control, "pwm_control", 4096 ,NULL , TASK_PRIO_2, &pwm_control_task, CORE1);

	xTaskCreatePinnedToCore(display_update_AC, "display_update_AC", (64*5*sizeof(uint16_t))+4096 ,NULL , TASK_PRIO_0,NULL , tskNO_AFFINITY);

	xTaskCreatePinnedToCore(special_keypad_row, "special_keypad_row", (64*8*sizeof(uint16_t))+4096 ,NULL , TASK_PRIO_0,&keypadrow_control_task , tskNO_AFFINITY);

	
}
