/*
 * adc_functions.c
 *
 *  Created on: Jan 19, 2026
 *      Author: dario
 */

#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/mcpwm_cmpr.h"
#include "driver/mcpwm_types.h"
#include "mcpwm_bat_charge.c"
#include "gpio_keypad.c"


typedef struct ADC_HANDLERS{
adc_oneshot_unit_handle_t adc_handle;
adc_cali_handle_t adc_cali_handle;
	}ADC_handler_t;

static ADC_handler_t adc1[5];


int * pointer_ADC_result_AC;

TaskHandle_t pwm_control_task;
TaskHandle_t display_update_task;


static const char *TAG = "error/message:";


/*---------------------------------------------------------------
        ADC Calibration
---------------------------------------------------------------*/
static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .chan = channel,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else {
        ESP_LOGE(TAG, "Invalid arg or no memory");
    }

    return calibrated;
}



void adc_one_shoot_AC_reading(void *pvparameter ){

	TickType_t xLastWakeTime =  xTaskGetTickCount();

    int* adc_result = (int*)heap_caps_malloc(sizeof(int), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

	static int adc_raw[10];

	static int temp=0;

for(;;){

vPortEnterCritical(CORE0);

for(int i=0;i<10;i++){
ESP_ERROR_CHECK(adc_oneshot_read(adc1[0].adc_handle, ADC1_AC, &adc_raw[i]));
}

taskEXIT_CRITICAL(CORE0);

for(int i=0;i<10;i++){

temp +=  adc_raw[i]; 

}

temp= temp/10;

adc_cali_raw_to_voltage(adc1[0].adc_cali_handle, temp, adc_result); 

pointer_ADC_result_AC= adc_result;

xTaskNotifyGive(pwm_control_task);
xTaskNotifyGive(display_update_task);


vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(4));


}
}

void pwm_control (void *pvparameter ){


int nomAc= VNOMAC * 1000;

int maxAc= VMAXAC * 1000;

int measured = 0;

int offsetL=0;

int newtickH= MIN_COMP_H;

int newtickL= MIN_COMP_L;

int booster;


for(;;){


ulTaskNotifyTake(pdTRUE, portMAX_DELAY);


measured = (int) * pointer_ADC_result_AC;

booster= gpio_get_level(GPIO_INPUT_BOOSTER);

offsetL= abs(measured-nomAc);

if (booster==0){ 

if (measured > nomAc){

	if (measured > maxAc){

mcpwm_comparator_set_compare_value(comparatorsBoosters[0], MIN_COMP_H);

}else if (newtickH>MIN_COMP_H){

newtickH -= 1;
mcpwm_comparator_set_compare_value(comparatorsBoosters[0], newtickH);


} 
}else if(measured < nomAc){

if (newtickH<MAX_COMP_H){
newtickH += 1;

mcpwm_comparator_set_compare_value(comparatorsBoosters[0], newtickH);

}

}
}else if(booster==1){

if (measured > nomAc){

	if (measured > maxAc){

mcpwm_comparator_set_compare_value(comparatorsBoosters[1], MIN_COMP_L);

}else if (newtickL>MIN_COMP_L){

if (offsetL>300){
if(newtickL-5>MIN_COMP_L){

newtickL -= 5;

mcpwm_comparator_set_compare_value(comparatorsBoosters[1], newtickL);

}

else if(newtickL-2>MIN_COMP_L){

newtickL -= 2;

mcpwm_comparator_set_compare_value(comparatorsBoosters[1], newtickL);

}

}else if (offsetL>200){

if(newtickL-2>MIN_COMP_L){

newtickL -= 2;

mcpwm_comparator_set_compare_value(comparatorsBoosters[1], newtickL);

}
}else {

newtickL -= 1;

mcpwm_comparator_set_compare_value(comparatorsBoosters[1], newtickL);


}
}
}

else if(measured < nomAc){

}else if (newtickL<MAX_COMP_L){

if (offsetL>300){
if(newtickL+5<MAX_COMP_L){

newtickL += 5;

mcpwm_comparator_set_compare_value(comparatorsBoosters[1], newtickL);

}

else if(newtickL+2<MAX_COMP_L){

newtickL += 2;

mcpwm_comparator_set_compare_value(comparatorsBoosters[1], newtickL);

}

}else if (offsetL>200){

if(newtickL+2<MIN_COMP_L){

newtickL += 2;

mcpwm_comparator_set_compare_value(comparatorsBoosters[1], newtickL);

}
}else {

newtickL += 1;

mcpwm_comparator_set_compare_value(comparatorsBoosters[1], newtickL);


}
}
}
}
}


void adc_setup(){



    //-------------ADC1 Init---------------//
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
for(int i=0; i<5; i++){

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1[i].adc_handle));

}

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };


    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1[0].adc_handle, ADC1_AC, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1[1].adc_handle, ADC1_DC1, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1[2].adc_handle, ADC1_DC2, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1[3].adc_handle, ADC1_DC3, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1[4].adc_handle, ADC1_BAT, &config));


   //-------------ADC1 Calibration Init---------------//
 
	 ESP_ERROR_CHECK(adc_calibration_init(ADC_UNIT_1, ADC1_AC, ADC_ATTEN, &adc1[0].adc_cali_handle));
	 ESP_ERROR_CHECK(adc_calibration_init(ADC_UNIT_1, ADC1_DC1, ADC_ATTEN, &adc1[1].adc_cali_handle));
	 ESP_ERROR_CHECK(adc_calibration_init(ADC_UNIT_1, ADC1_DC2, ADC_ATTEN, &adc1[2].adc_cali_handle));
	 ESP_ERROR_CHECK(adc_calibration_init(ADC_UNIT_1, ADC1_DC3, ADC_ATTEN, &adc1[3].adc_cali_handle));
	 ESP_ERROR_CHECK(adc_calibration_init(ADC_UNIT_1, ADC1_BAT, ADC_ATTEN, &adc1[4].adc_cali_handle));




}