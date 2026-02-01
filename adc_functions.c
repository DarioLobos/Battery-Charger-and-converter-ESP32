/*
 * adc_functions.c
 *
 *  Created on: Jan 19, 2026
 *      Author: dario
 */

#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/mcpwm_cmpr.h"
#include "driver/mcpwm_types.h"
#include "hal/adc_types.h"
#include "mcpwm_bat_charge.c"
#include "gpio_keypad.c"
#include "portmacro.h"
#include "soc/soc_caps.h"
#include <stdint.h>


static adc_oneshot_unit_handle_t adc_handle_one_shoot= NULL;
static adc_cali_handle_t adc_cali_handle= NULL;

static adc_continuous_handle_t adc_handle_continous = NULL;
static adc_cali_handle_t * adc_cont_out_handle[5];


static uint8_t * ADC_BUFFER[ADC_BUFFER_SIZE];

static TaskHandle_t pwm_control_task;
static TaskHandle_t display_update_task;
static TaskHandle_t dc_pwm_control_task;


int * pointer_ADC_results_AC;

adc_digi_output_data_t * data_DC_wrapper_pointer;   

static uint32_t rxlength=0;

uint16_t *adc_dc_results_pointers[4]; 

int *adc_dc_voltage_pointers[4]; 

static const char *TAG = "error/message:";

static volatile uint8_t flag_device3out=0;

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

void adc_continous_DC_reading(void *pvparameter ){

    ADC_BUFFER[0] = (uint8_t *)heap_caps_malloc(sizeof(ADC_BUFFER), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

	for(int i=0; i<4;i++){
    adc_dc_results_pointers[i] = (uint16_t *)heap_caps_malloc(sizeof(uint16_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

}

	for(int i=0; i<4;i++){
    adc_dc_voltage_pointers[i] = (int *)heap_caps_malloc(sizeof(int), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

}


  ESP_ERROR_CHECK(adc_continuous_start(adc_handle_continous));

}


void adc_one_shoot_AC_reading(void *pvparameter ){

	TickType_t xLastWakeTime;

    pointer_ADC_results_AC = (int*)heap_caps_malloc(sizeof(int), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

	static int adc_raw[10];

	static int temp=0;

for(;;){

	xLastWakeTime =  xTaskGetTickCount();


vPortEnterCritical(CORE0);

for(int i=0;i<10;i++){
ESP_ERROR_CHECK(adc_oneshot_read(adc_handle_one_shoot, ADC1_AC, &adc_raw[i]));
}

taskEXIT_CRITICAL(CORE0);

for(int i=0;i<10;i++){

temp +=  adc_raw[i]; 

}

temp= temp/10;

adc_cali_raw_to_voltage(adc_cali_handle, temp, pointer_ADC_results_AC); 


xTaskNotifyGive(pwm_control_task);
xTaskNotifyGive(display_update_task);


vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(4));


}
}

void ac_pwm_control (void *pvparameter ){


int nomAc= VNOMAC * 1000;

int maxAc= VMAXAC * 1000;

int measured = 0;

int offsetL=0;

static volatile int newtickH= MIN_COMP_H;

static volatile int newtickL= MIN_COMP_L;

int booster;


for(;;){


ulTaskNotifyTake(pdTRUE, portMAX_DELAY);


measured = (int) * pointer_ADC_results_AC;

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

if (offsetL>GRADIENT_BOOST_hIGH){
if(newtickL-5>MIN_COMP_L){

newtickL -= 5;

mcpwm_comparator_set_compare_value(comparatorsBoosters[1], newtickL);

}

else if(newtickL-2>MIN_COMP_L){

newtickL -= 2;

mcpwm_comparator_set_compare_value(comparatorsBoosters[1], newtickL);

}

}else if (offsetL>GRADIENT_BOOST_MID){

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

if (offsetL>GRADIENT_BOOST_hIGH){
if(newtickL+5<MAX_COMP_L){

newtickL += 5;

mcpwm_comparator_set_compare_value(comparatorsBoosters[1], newtickL);

}

else if(newtickL+2<MAX_COMP_L){

newtickL += 2;

mcpwm_comparator_set_compare_value(comparatorsBoosters[1], newtickL);

}

}else if (offsetL>GRADIENT_BOOST_MID){

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

// ADC CALL BACK FOR WHEN FINISH READING 

static bool IRAM_ATTR cont_ADC_callback_done(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
{


BaseType_t xHigherPriorityTaskWoken = pdFALSE;

vTaskNotifyGiveFromISR(dc_pwm_control_task, &xHigherPriorityTaskWoken);

    return (xHigherPriorityTaskWoken = pdTRUE);
}

void dc_pwm_changer_BOOSTER(volatile int *tick, int adc_dc_results_vin, int adc_dc_results_vout, int mask, mcpwm_cmpr_handle_t  comparator){

//boost

if(((NON_DC_VOUT-adc_dc_results_vout)>GRADIENT_DC_HIGH)) {

if (*tick<(DC_MAX_D_BOOSTER -3)){

*tick=*tick+3;

mcpwm_comparator_set_compare_value(comparator, *tick);

}
else if(*tick<(DC_MAX_D_BOOSTER-2)){

*tick=*tick+2;

mcpwm_comparator_set_compare_value(comparator, *tick);


}
else if(*tick<(DC_MAX_D_BOOSTER-1)){

*tick=*tick+2;

mcpwm_comparator_set_compare_value(comparator, *tick);


}

}else if(((NON_DC_VOUT-adc_dc_results_vout)>GRADIENT_DC_MID)) {

if(*tick<(DC_MAX_D_BOOSTER-2)){

*tick=*tick+2;

mcpwm_comparator_set_compare_value(comparator, *tick);


}
else if(*tick<(DC_MAX_D_BOOSTER-1)){

*tick=*tick+1;

mcpwm_comparator_set_compare_value(comparator, *tick);


}

}else if(((NON_DC_VOUT-adc_dc_results_vout)>100)) {

if(*tick<(DC_MAX_D_BOOSTER-1)){

*tick=*tick+1;

mcpwm_comparator_set_compare_value(comparator, *tick);

}

} 
else if(((NON_DC_VOUT-adc_dc_results_vout)<GRADIENT_DC_HIGH)) {

if (*tick<(DC_MIN_D_BOOSTER+3)){

*tick=*tick-3;

mcpwm_comparator_set_compare_value(comparator, *tick);

}
else if(*tick<(DC_MIN_D_BOOSTER+2)){

*tick=*tick-2;

mcpwm_comparator_set_compare_value(comparator, *tick);

}
else if(*tick<(DC_MIN_D_BOOSTER+1)){

*tick=*tick-1;

mcpwm_comparator_set_compare_value(comparator, *tick);

}

}else if(((NON_DC_VOUT-adc_dc_results_vout)<GRADIENT_DC_MID)) {

if(*tick<(DC_MIN_D_BOOSTER+2)){

*tick=*tick-2;

mcpwm_comparator_set_compare_value(comparator, *tick);

}
else if(*tick<(DC_MIN_D_BOOSTER+1)){

*tick=*tick-1;

mcpwm_comparator_set_compare_value(comparator, *tick);

}

}else if(((NON_DC_VOUT-adc_dc_results_vout)<GRADIENT_DC_LOW)) {

if(*tick<(DC_MAX_D_BOOSTER+1)){

*tick=*tick-1;

mcpwm_comparator_set_compare_value(comparator, *tick);

}

}
}

void dc_pwm_changer_BUCK(volatile int *tick, int adc_dc_results_vin, int adc_dc_results_vout, int mask, mcpwm_cmpr_handle_t  comparator){

//buck

if(((NON_DC_VOUT-adc_dc_results_vout)>GRADIENT_DC_HIGH)) {

if (*tick<(DC_MAX_D_BUCK -3)){

*tick=*tick+3;

mcpwm_comparator_set_compare_value(comparator, *tick);

}
else if(*tick<(DC_MAX_D_BUCK-2)){

*tick=*tick+2;

mcpwm_comparator_set_compare_value(comparator, *tick);


}
else if(*tick<(DC_MAX_D_BUCK-1)){

*tick=*tick+2;

mcpwm_comparator_set_compare_value(comparator, *tick);


}

}else if(((NON_DC_VOUT-adc_dc_results_vout)>GRADIENT_DC_MID)) {

if(*tick<(DC_MAX_D_BUCK-2)){

*tick=*tick+2;

mcpwm_comparator_set_compare_value(comparator, *tick);


}
else if(*tick<(DC_MAX_D_BUCK-1)){

*tick=*tick+1;

mcpwm_comparator_set_compare_value(comparator, *tick);


}

}else if(((NON_DC_VOUT-adc_dc_results_vout)>GRADIENT_DC_LOW)) {

if(*tick<(DC_MAX_D_BUCK-1)){

*tick=*tick+1;

mcpwm_comparator_set_compare_value(comparator, *tick);


}

} 
else if(((NON_DC_VOUT-adc_dc_results_vout)<300)) {

if (*tick<(DC_MIN_D_BUCK+3)){

*tick=*tick-3;

mcpwm_comparator_set_compare_value(comparator, *tick);

}
else if(*tick<(DC_MIN_D_BUCK+2)){

*tick=*tick-2;

mcpwm_comparator_set_compare_value(comparator, *tick);


}
else if(*tick<(DC_MIN_D_BUCK+1)){

*tick=*tick-1;

mcpwm_comparator_set_compare_value(comparator, *tick);


}

}else if(((NON_DC_VOUT-adc_dc_results_vout)<GRADIENT_DC_MID)) {

if(*tick<(DC_MIN_D_BUCK+2)){

*tick=*tick-2;

mcpwm_comparator_set_compare_value(comparator, *tick);


}
else if(*tick<(DC_MIN_D_BUCK+1)){

*tick=*tick-1;

mcpwm_comparator_set_compare_value(comparator, *tick);


}

}else if(((NON_DC_VOUT-adc_dc_results_vout)<GRADIENT_DC_LOW)) {

if(*tick<(DC_MAX_D_BUCK+1)){

*tick=*tick-1;

mcpwm_comparator_set_compare_value(comparator, *tick);


			}
		}
	}


void dc_pwm_changer_BUCK_BOOST(volatile int *tick, int adc_dc_results_vin, int adc_dc_results_vout, int mask, mcpwm_cmpr_handle_t  comparator){

if(((NON_DC_VOUT-adc_dc_results_vout)>100)) {

if (*tick<(DC_MAX_D_BUCK_BOOST -1)){

*tick=*tick+1;

mcpwm_comparator_set_compare_value(comparator, *tick);

}
}else if(((NON_DC_VOUT-adc_dc_results_vout)<GRADIENT_DC_LOW)) {

if (*tick<(DC_MIN_D_BUCK_BOOST +1)){

*tick=*tick-1;

mcpwm_comparator_set_compare_value(comparator, *tick);

			}
 		}
	}

void dc_pwm_control(void *pvparameter ){

int tempresult[4];


uint16_t pres_Status=0;

int newtickBOOSTER[3]= { DC_MIN_D_BOOSTER, DC_MIN_D_BOOSTER , DC_MIN_D_BOOSTER };

int newtickBUCK[3]= {DC_MIN_D_BUCK, DC_MIN_D_BUCK, DC_MIN_D_BUCK};

static volatile int newtickBUCK_BOOST[3]= {DC_MIN_D_BUCK_BOOST, DC_MIN_D_BUCK_BOOST, DC_MIN_D_BUCK_BOOST};



/*
to do faster I use a mask of bits 2 bits for each Channel asking if Voltage read was:
	  Vread -(DCBAT+1)< -1 		bit 	01	BOOST
     -1 < Vread -(DCBAT+1) <1 	Bit		10 	BUCK-BOOST
	 Vread -(DCBAT+1) > 1		Bit		00	BUCK

This defines if it was running on Buck, Boost or Buck-boost converter
and to make the needed transition if change.
*/

for(;;){

// I am using the read to block the task, I there no egough time to run other task, taskdelay(] should be used

adc_continuous_read(adc_handle_continous,*ADC_BUFFER, ADC_FRAME_SIZE,&rxlength,portMAX_DELAY );

adc_continuous_flush_pool( adc_handle_continous);


int data_in_counter[4]={0,0,0,0};


for(int i=0; i <rxlength; i+= SOC_ADC_DIGI_RESULT_BYTES){

data_DC_wrapper_pointer= (adc_digi_output_data_t *) &ADC_BUFFER[i];


if (data_DC_wrapper_pointer->type1.channel==ADC1_DC1){
tempresult[0]+= data_DC_wrapper_pointer->type1.data;
data_in_counter[0]++;
}
else if(data_DC_wrapper_pointer->type1.channel==ADC1_DC2){
tempresult[1]+= data_DC_wrapper_pointer->type1.data;
data_in_counter[1]++;
}
else if(data_DC_wrapper_pointer->type1.channel==ADC1_DC3){
tempresult[2]+= data_DC_wrapper_pointer->type1.data;
data_in_counter[2]++;
}
else if(data_DC_wrapper_pointer->type1.channel==ADC1_BAT){
tempresult[3]+= data_DC_wrapper_pointer->type1.data;
data_in_counter[3]++;
}

}

for (int i=0;i<4;i++){
*adc_dc_results_pointers[i]= tempresult[i]/data_in_counter[i];

}

for (int i=0;i<4; i++){

adc_cali_raw_to_voltage(*adc_cont_out_handle[i], (uint32_t) adc_dc_results_pointers[i], adc_dc_voltage_pointers[i]);

*adc_dc_voltage_pointers[i]=*adc_dc_voltage_pointers[i]*((R1+R2)/R1);

	}


for (int i=0;i<4; i++){

if ((*adc_dc_voltage_pointers[0]-*adc_dc_voltage_pointers[3])>1){ 
pres_Status=(0<<(2*i));
}
else if ((*adc_dc_voltage_pointers[3]-*adc_dc_voltage_pointers[0])>1){
pres_Status=(2<<(2*i));
}
else{

pres_Status=(1<<(2*i));
}

}

int device3out;

if (flag_device3out){

device3out=2;

}
else{

device3out=3;

}


for (int i=0;i<device3out;i++){

if((MAX_DC_VIN<*adc_dc_voltage_pointers[i]) | (MIN_DC_VIN>*adc_dc_voltage_pointers[i]) |
(MAX_DC_VOUT<*adc_dc_voltage_pointers[3]) |(MIN_DC_VOUT>*adc_dc_voltage_pointers[3]) ){

mcpwm_generator_set_force_level(generators_DC_control[i][0] , 1, true);

} 
else {

int mask =3*2^(2*i);

if ((pres_Status&mask)==2){
//boost

mcpwm_generator_set_force_level(generators_DC_control[i][0] , 0, true);

mcpwm_generator_set_force_level(generators_DC_control[i][1] , -1, true);

dc_pwm_changer_BOOSTER(&newtickBOOSTER[i], *adc_dc_voltage_pointers[i], *adc_dc_voltage_pointers[3], mask, comparators_DC_control[i]);

}
else if ((pres_Status&mask)==1){
// buck-boost

mcpwm_generator_set_force_level(generators_DC_control[i][0] , -1, true);

mcpwm_generator_set_force_level(generators_DC_control[i][1] , -1, true);

dc_pwm_changer_BUCK_BOOST(&newtickBUCK_BOOST[i], *adc_dc_voltage_pointers[i], *adc_dc_voltage_pointers[3], mask, comparators_DC_control[i]);


}

else {
// buck

mcpwm_generator_set_force_level(generators_DC_control[i][0] , -1, true);

mcpwm_generator_set_force_level(generators_DC_control[i][1] , 0, true);

dc_pwm_changer_BUCK(&newtickBUCK[i], *adc_dc_voltage_pointers[i], *adc_dc_voltage_pointers[3], mask, comparators_DC_control[i]);

}

}

}

}

}

void adc_setup(){

    //-------------ADC1 continous Init---------------//
    adc_continuous_handle_cfg_t adc_config_continous = {
        .max_store_buf_size = ADC_BUFFER_SIZE,
        .conv_frame_size = ADC_FRAME_SIZE,
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config_continous, &adc_handle_continous));


    //-------------ADC1 oneshoot Init---------------//
    adc_oneshot_unit_init_cfg_t adc_one_shoot_config = {
        .unit_id = ADC_UNIT_1,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_one_shoot_config, &adc_handle_one_shoot));

	adc_channel_t channels[4]={ADC1_DC1,ADC1_DC2,ADC1_DC3, ADC1_BAT};

    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = 20000,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
		.format =  ADC_DIGI_OUTPUT_FORMAT_TYPE1
    };


adc_digi_pattern_config_t adc_pattern[4] = {0};

for (int i = 0; i < sizeof(channels); i++) {
        adc_pattern[i].atten = ADC_ATTEN;
        adc_pattern[i].channel = channels[i] & 0x7;
        adc_pattern[i].unit = ADC_UNIT_1;
        adc_pattern[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;

}

dig_cfg.adc_pattern = adc_pattern;

    ESP_ERROR_CHECK(adc_continuous_config(adc_handle_continous, &dig_cfg));

    

    //-------------ADC1 one shoot Config---------------//
    adc_oneshot_chan_cfg_t config_oneshoot = {
        .atten = ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };


    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle_one_shoot, ADC1_AC, &config_oneshoot));
    

   //-------------ADC1 Calibration Init---------------//
 
	 ESP_ERROR_CHECK(adc_calibration_init(ADC_UNIT_1, ADC1_AC, ADC_ATTEN, adc_cont_out_handle[0]));
	 ESP_ERROR_CHECK(adc_calibration_init(ADC_UNIT_1, ADC1_DC1, ADC_ATTEN, adc_cont_out_handle[1]));
	 ESP_ERROR_CHECK(adc_calibration_init(ADC_UNIT_1, ADC1_DC2, ADC_ATTEN, adc_cont_out_handle[2]));
	 ESP_ERROR_CHECK(adc_calibration_init(ADC_UNIT_1, ADC1_DC3, ADC_ATTEN, adc_cont_out_handle[3]));
	 ESP_ERROR_CHECK(adc_calibration_init(ADC_UNIT_1, ADC1_BAT, ADC_ATTEN, adc_cont_out_handle[4]));

    adc_continuous_evt_cbs_t cbs = {
        .on_conv_done = cont_ADC_callback_done,
    };
    ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(adc_handle_continous, &cbs, NULL));
    ESP_ERROR_CHECK(adc_continuous_start(adc_handle_continous));

}