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
#include "aware.c"


void app_main(void)
{

esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

	GPIO.out_w1ts=((1<<15&~(1<<12)&1));

	psi_setup();

	adc_setup();

	timer_setup_AC();

	timer_setup_DC();

	gpio_booster_config ();

	gpio_photoresistor_config ();
	flag_photoresistor =  REG_READ(GPIO_IN_REG);


	mcp23017_config();

// THIS PUT BIT 0 OF PORTA MCP IN 1 FOR KEYPAD READ THE * KEY WITHOUT MAP AT UNTIL KEY IS PRESSED
	mcp23017_set_pins_PortA_high(MCPA0);

  	//display_init in background.c
    xTaskCreatePinnedToCore(display_init, "display_init",
     (2*(COLARRAY*ROWARRAY*sizeof(uint16_t))+4096),
     NULL, TASK_PRIO_4, &xtaskHandleDisplay , CORE0);

	//schedulerBackground in schedulerBackgorund.c
    xTaskCreatePinnedToCore(schedulerBackground, "display_frames",
    (2*(STROWARRAY*STCOLARRAY*sizeof(uint16_t))+4096),
    NULL ,TASK_PRIO_4, NULL, CORE1);

	//schedulerOffBackground in schedulerOffbackgorund.c
    xTaskCreatePinnedToCore(schedulerOffBackground, "display_frames",
    (2*(STROWARRAY*STCOLARRAY*sizeof(uint16_t))+4096),
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
	xTaskCreatePinnedToCore(devices_scheduler, "device3_scheduler", 4096 ,NULL , TASK_PRIO_3, &booster_control_task, CORE1);

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
	
// wifi_aware_publish in aware.c
	xTaskCreatePinnedToCore(wifi_aware_publish,     "socket_srv",     4096,     NULL,     TASK_PRIO_1,NULL,     tskNO_AFFINITY);

	// wifi_aware_socket_task in aware.c
	xTaskCreatePinnedToCore(wifi_aware_socket_task,     "socket_srv",     4096,     NULL,     TASK_PRIO_1,&xserver_task,     tskNO_AFFINITY);

	// nan_discovery_task_task in aware.c
	xTaskCreatePinnedToCore(nan_discovery_task,     "socket_srv",     4096,     NULL,     TASK_PRIO_1,&xdiscovery_task,     tskNO_AFFINITY);


}
