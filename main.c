/*
 * display_commands.c
 *
 *  Created on: Jan 16, 2026
 *      Author: dario Lobos
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "display_functions.c"

void app_main(void)
{

	GPIO.out_w1ts=((1<<15&~(1<<12)&1));

	psi_setup();

	adc_setup(); 

	timer_setup_AC();

	timer_setup_DC();

	gpio_booster_config ();

  	//display_init in display_functions.c
    xTaskCreatePinnedToCore(display_init, "display_init", (COLARRAY*ROWARRAY*sizeof(uint16_t)+4096), NULL, TASK_PRIO_4, &xtaskHandleDisplay , CORE0);

	//display_frames in display_functions.c
    xTaskCreatePinnedToCore(display_frames, "display_frames",
    (ROWTIME*COLTIME*sizeof(uint16_t))+(ROWAC*COLAC*sizeof(uint16_t))+(ROWDC*COLDC*sizeof(uint16_t))+5*(STROWARRAY*STCOLARRAY*sizeof(uint16_t))+4096,
    NULL ,TASK_PRIO_4, &xtaskHandleFrame, CORE1);

	//timer_mosfet_start in mcpwm_init.c
    xTaskCreatePinnedToCore(timer_mosfet_start, "Mosfet_signal_start", 4096 ,NULL , TASK_PRIO_3, NULL, CORE0);

	//adc_continous_DC_reading in adc_function.c
    xTaskCreatePinnedToCore(adc_continous_DC_reading, "adc_readingDC", 4096 ,NULL , TASK_PRIO_2, NULL, CORE0);

	//adc_one_shoot_AC_reading in adc_function.c
    xTaskCreatePinnedToCore(adc_one_shoot_AC_reading, "adc_readingAC", 4096 ,NULL , TASK_PRIO_2, NULL, CORE0);

	//booster_selection in mcpwm_init.c
	xTaskCreatePinnedToCore(booster_selection, "booster_selectionl", 4096 ,NULL , TASK_PRIO_3, &booster_control_task, CORE1);
    
  	//device3_scheduler in display_functions.c
	xTaskCreatePinnedToCore(device3_scheduler, "device3_scheduler", 4096 ,NULL , TASK_PRIO_3, &booster_control_task, CORE1);

	//dc_pwm_control in adc_function.c
	xTaskCreatePinnedToCore(dc_pwm_control, "pwm_controlDC", 4096 ,NULL , TASK_PRIO_2, &dc_pwm_control_task, CORE0);

	// ac_pwm_control in adc_function.c
	xTaskCreatePinnedToCore(ac_pwm_control, "pwm_controlAC", 4096 ,NULL , TASK_PRIO_2, &pwm_control_task, CORE1);

  	//display_update_SET_TIME in display_functions.c
	xTaskCreatePinnedToCore(display_update_SET_TIME, "display_update_SET_TIME", (STROWARRAY*STCOLARRAY*sizeof(uint16_t))+4096 ,NULL , TASK_PRIO_1,&xtaskHandleSetTime , tskNO_AFFINITY);

  	//display_update_RESET_BKG_TIME in display_functions.c
	xTaskCreatePinnedToCore(display_update_RESET_BKG_TIME, "display_update_RESET_BKG_TIME", (STROWARRAY*STCOLARRAY*sizeof(uint16_t))+4096 ,NULL , TASK_PRIO_1,&xtaskHandleReset_BKG_Time, tskNO_AFFINITY);

  	//display_update_AC in display_functions.c
	xTaskCreatePinnedToCore(display_update_AC, "display_update_AC", (ROWAC*COLAC*sizeof(uint16_t))+4096 ,NULL , TASK_PRIO_0,NULL , tskNO_AFFINITY);

  	//display_update_DC in display_functions.c
	xTaskCreatePinnedToCore(display_update_DC, "display_update_AC", (ROWDC*COLDC*sizeof(uint16_t))+4096 ,NULL , TASK_PRIO_0,&xtaskHandledisplay_update_DC , tskNO_AFFINITY);


}
