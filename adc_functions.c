/*
 * adc_functions.c
 *
 *  Created on: Jan 19, 2026
 *      Author: dario
 */

#include "main.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"


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

typedef struct ADC_HANDLERS{
adc_oneshot_unit_handle_t adc_handle;
adc_cali_handle_t adc_cali_handle;
	}ADC_handler_t;

int * pointer_ADC_result;

TaskHandle_t pwm_control_task;
TaskHandle_t display_update_task;


void adc_one_shoot_reading(void *pvparameter ){

	TickType_t xLastWakeTime =  xTaskGetTickCount();

	ADC_handler_t* adc1;

	adc1=  pvparameter;


    int* adc_result = (int*)heap_caps_malloc(sizeof(int), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

	static int adc_raw[10];

	static int temp=0;

for(;;){

vPortEnterCritical(CORE0);

for(int i=0;i<10;i++){
ESP_ERROR_CHECK(adc_oneshot_read(adc1->adc_handle, ADC1_CHAN0, &adc_raw[i]));
}

taskEXIT_CRITICAL(CORE0);

for(int i=0;i<10;i++){

temp +=  adc_raw[i]; 

}

temp= temp/10;

adc_cali_raw_to_voltage(adc1->adc_cali_handle, temp, adc_result); 

pointer_ADC_result= adc_result;

xTaskNotifyGive(pwm_control_task);
xTaskNotifyGive(display_update_task);


vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(16));


}
}

void adc_setup(	ADC_handler_t adc10, ADC_handler_t adc11,ADC_handler_t adc12, ADC_handler_t adc13, ADC_handler_t adc14){

ADC_handler_t adc1[5]={adc10,adc11,adc12,adc13,adc14};


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


    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1[0].adc_handle, ADC1_CHAN1, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1[1].adc_handle, ADC1_CHAN2, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1[2].adc_handle, ADC1_CHAN3, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1[4].adc_handle, ADC1_CHAN6, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1[5].adc_handle, ADC1_CHAN7, &config));


   //-------------ADC1 Calibration Init---------------//
 
	 ESP_ERROR_CHECK(adc_calibration_init(ADC_UNIT_1, ADC1_CHAN1, ADC_ATTEN, &adc1[0].adc_cali_handle));
	 ESP_ERROR_CHECK(adc_calibration_init(ADC_UNIT_1, ADC1_CHAN2, ADC_ATTEN, &adc1[1].adc_cali_handle));
	 ESP_ERROR_CHECK(adc_calibration_init(ADC_UNIT_1, ADC1_CHAN3, ADC_ATTEN, &adc1[2].adc_cali_handle));
	 ESP_ERROR_CHECK(adc_calibration_init(ADC_UNIT_1, ADC1_CHAN6, ADC_ATTEN, &adc1[3].adc_cali_handle));
	 ESP_ERROR_CHECK(adc_calibration_init(ADC_UNIT_1, ADC1_CHAN7, ADC_ATTEN, &adc1[4].adc_cali_handle));




}