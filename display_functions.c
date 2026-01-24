/*
 * display_functions.c
 *
 *  Created on: Jan 19, 2026
 *      Author: dario Lobos
 */

#include "freertos/projdefs.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/types.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "display_commands.c"
#include "portmacro.h"
#include "background.c"
#include "setuptimebackground.c"
#include "font_numbers.c"
#include "adc_functions.c"
#include "ic2_commands_RTC_CLK.c"

TaskHandle_t xtaskHandleDisplay= NULL;
TaskHandle_t xtaskHandleFrame = NULL;
TaskHandle_t xtaskHandleSetTime = NULL;
TaskHandle_t xtaskHandleResetTime = NULL;


static volatile uint16_t* ac_pointers_to_send[ROWAC];

static volatile uint16_t* timeH1_pointers_to_send[ROWTIME];

static volatile uint16_t* timeH2_pointers_to_send[ROWTIME];

static volatile uint16_t* timeM1_pointers_to_send[ROWTIME];

static volatile uint16_t* timeM2_pointers_to_send[ROWTIME];

static volatile uint16_t* timeS1_pointers_to_send[ROWTIME];

static volatile uint16_t* timeS2_pointers_to_send[ROWTIME];

static uint8_t array_of_commands_poll[11]={ NORON,COLMOD,PCOLMOD,DISPON,CASET,0,COLARRAY-1,RASET,0,ROWARRAY-1,RAMWR};

static uint8_t * pointer_to_commands_poll=&array_of_commands_poll[0]; 

static uint8_t array_of_commands_ISR_timeH1[7]={CASET,H1TCASETL,H1TCASETH,RASET,TIMERASETL,TIMERASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_timeH1=&array_of_commands_ISR_timeH1[0];

static uint8_t array_of_commands_ISR_timeH2[7]={CASET,H2TCASETL,H2TCASETH,RASET,TIMERASETL,TIMERASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_timeH2=&array_of_commands_ISR_timeH2[0];

static uint8_t array_of_commands_ISR_timeM1[7]={CASET,M1TCASETL,M1TCASETH,RASET,TIMERASETL,TIMERASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_timeM1=&array_of_commands_ISR_timeM1[0];

static uint8_t array_of_commands_ISR_timeM2[7]={CASET,M2TCASETL,M2TCASETH,RASET,TIMERASETL,TIMERASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_timeM2=&array_of_commands_ISR_timeM2[0];

static uint8_t array_of_commands_ISR_timeS1[7]={CASET,S1TCASETL,S1TCASETH,RASET,TIMERASETL,TIMERASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_timeS1=&array_of_commands_ISR_timeS1[0];

static uint8_t array_of_commands_ISR_timeS2[7]={CASET,S2TCASETL,S2TCASETH,RASET,TIMERASETL,TIMERASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_timeS2=&array_of_commands_ISR_timeS2[0];



uint8_t array_of_commands_ISR_AC[7]={CASET,ACCASETL,ACCASETH,RASET,ACRASETL,ACRASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_ac=&array_of_commands_ISR_AC[0];

static uint8_t array_of_commands_ISR_DC[7]={CASET,DCCASETL,DCCASETH,RASET,DCRASETL,DCRASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_dc=&array_of_commands_ISR_DC[0];

static uint8_t array_of_commands_ISR_BANNERST[7]={CASET,STCASETL,STCASETH,RASET,STRASETL,STRASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_BANNERST=&array_of_commands_ISR_BANNERST[0];

spi_device_handle_t spi;


static void display_init(void *pvparameter){

display_allocation();

setup_time_bkg_allocation();

xTaskNotifyGive(xtaskHandleFrame);


spi_polling(spi, *pointer_to_commands_poll,sizeof(array_of_commands_poll), true);

//spi_polling(spi, NORON,8, true);
//spi_polling(spi, COLMOD,8, true);
//spi_polling(spi, PCOLMOD,8, true);
//spi_polling(spi, DISPON,8, true);
//spi_polling(spi, CASET,8, true);
//spi_polling(spi, 0,8, true);// ALL THE SCREEN
//spi_polling(spi, COLARRAY-1, 8,true); // ALL THE SCREEN
//spi_polling(spi, RASET,8, true);
//spi_polling(spi, 0,8, true);// ALL THE SCREEN
//spi_polling(spi, ROWARRAY-1,8, true);// ALL THE SCREEN
//spi_polling(spi, RAMWR,8, true);// ALL THE SCREEN


spi_polling(spi, background_pointers[0][0],sizeof(background_pointers), true);// ALL THE SCREEN


spi_device_polling_end(spi, portMAX_DELAY);

// BACKGROUND READY AT FIRST

// awaiting small frames are done

ulTaskNotifyTake(pdTRUE,portMAX_DELAY);

// free memory and delete task awaiting next task finish

free(background_pointers);

  vTaskDelete(NULL);
}

static void display_frames(void *pvparameter){


ulTaskNotifyTake(pdTRUE,portMAX_DELAY);

// allocation of frames that change in background, then to use it a bipmap of font change color when bitmap is 1 

frame_AC();
frame_DC();
frame_set_time();
frame_digits_time();

xTaskNotifyGive(xtaskHandleDisplay);

  vTaskDelete(NULL);

}

static void psi_setup(){
	const char *TAG = "error/message:";

    ESP_LOGI(TAG, "Initialize SPI bus");
    spi_bus_config_t buscfg = {
        .sclk_io_num = PIN_NUM_CLK,
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = (160 * 130 + 50) * sizeof(uint16_t),
    };
	ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

}


static void display_update_AC (void *pvparameter){

int received_voltage;
 
int digits;

uint8_t received_digit;


// Allocate memory for each row in PSRAM
for (int i = 0; i < ROWAC; i++) {
    ac_pointers_to_send[i] = (uint16_t*)heap_caps_malloc(COLAC * sizeof(uint16_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (ac_pointers_to_send[i] == NULL) {
        printf("Failed to allocate AC row %d\n", i);
        return;
        }
}


for(;;){

ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

digits=-1;

received_voltage= *pointer_ADC_result_AC; 

received_voltage= (int)((received_voltage*110000/2350)+5)/10; //transform to AC , eliminate one digit rounding, 

if ((received_digit=received_voltage-received_voltage%10000/10000)>0){

digits=0;

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				ac_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				ac_pointers_to_send[i][j]=ac_pointers[i][j];

				}
		}
	}

}

else if((received_digit=((received_voltage%10000-received_voltage%1000)/1000)>0) | (digits >-1)){
digits++; 
	for (int j=digits*8;j<(digits*8+8);j++){
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				ac_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				ac_pointers_to_send[i][j]=ac_pointers[i][j];

				}
		}
	}


}
else if((received_digit=((received_voltage%1000-received_voltage%100)/100)>0) | (digits >-1)){
digits++; 

	for (int j=digits;j<(digits*8+8);j++){
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				ac_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				ac_pointers_to_send[i][j]=ac_pointers[i][j];

				}
		}
	}
}
else if((received_digit=((received_voltage%100-received_voltage%10)/10)>0) | (digits >-1)){
digits++; 

	for (int j=digits;j<(digits*8+8);j++){
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				ac_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				ac_pointers_to_send[i][j]=ac_pointers[i][j];

				}
		}
	}
}
else {
received_digit=(received_voltage-received_voltage)%10;

digits++; 

	for (int j=digits;j<(digits*8+8);j++){
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				ac_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				ac_pointers_to_send[i][j]=ac_pointers[i][j];

				}
		}
	}
}
	for (int j=24;j<32;j++){
		for(int i=0;i<8;i++){
			if((font_bits[11][j]&(1<<i))>0){
				ac_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				ac_pointers_to_send[i][j]=ac_pointers[i][j];

				}
		}
	}
spi_transmit_isr(spi,true,*pointer_to_commands_isr_ac, sizeof(array_of_commands_ISR_AC), true);

spi_transmit_isr(spi,false, ac_pointers_to_send[0][0], sizeof(ac_pointers_to_send), true);
 

}
}

static void display_update_TIME (void *pvparameter){



int  prev_H1=0;
int  prev_H2=0;
int  prev_M1=0;
int  prev_M2=0;
int  prev_S1=0;
int  prev_S2=0;


uint8_t received_digit;

TickType_t xLastWakeTime;


// Allocate memory for each row and digit in PSRAM
for (int i = 0; i < ROWTIME; i++) {
    timeH1_pointers_to_send[i] = (uint16_t*)heap_caps_malloc(H1COLTIME * sizeof(uint16_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (timeH1_pointers_to_send[i] == NULL) {
        printf("Failed to allocate AC row %d\n", i);
        return;
        }
}

for (int i = 0; i < ROWTIME; i++) {
    timeH2_pointers_to_send[i] = (uint16_t*)heap_caps_malloc(H2COLTIME * sizeof(uint16_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (timeH2_pointers_to_send[i] == NULL) {
        printf("Failed to allocate AC row %d\n", i);
        return;
        }
}

for (int i = 0; i < ROWTIME; i++) {
    timeM1_pointers_to_send[i] = (uint16_t*)heap_caps_malloc(M1COLTIME * sizeof(uint16_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (timeM1_pointers_to_send[i] == NULL) {
        printf("Failed to allocate AC row %d\n", i);
        return;
        }
}

for (int i = 0; i < ROWTIME; i++) {
    timeM2_pointers_to_send[i] = (uint16_t*)heap_caps_malloc(M2COLTIME * sizeof(uint16_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (timeM2_pointers_to_send[i] == NULL) {
        printf("Failed to allocate AC row %d\n", i);
        return;
        }
}

for (int i = 0; i < ROWTIME; i++) {
    timeS1_pointers_to_send[i] = (uint16_t*)heap_caps_malloc(S1COLTIME * sizeof(uint16_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (timeS1_pointers_to_send[i] == NULL) {
        printf("Failed to allocate AC row %d\n", i);
        return;
        }
}

for (int i = 0; i < ROWTIME; i++) {
    timeS2_pointers_to_send[i] = (uint16_t*)heap_caps_malloc(S2COLTIME * sizeof(uint16_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (timeS2_pointers_to_send[i] == NULL) {
        printf("Failed to allocate AC row %d\n", i);
        return;
        }
}

for(;;){

xLastWakeTime = xTaskGetTickCount();

ic2_read_time();


// *received_time[0]= seconds  *received_time[1]= minutes  *received_time[2]= hours

received_digit=*received_time[2]-*received_time[2]%10/10;

if ((received_digit>0) & (prev_H1!= received_digit)){

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				timeH1_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				timeH1_pointers_to_send[i][j]=H1_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,*pointer_to_commands_isr_timeH1, sizeof(array_of_commands_ISR_timeH1), true);

spi_transmit_isr(spi,false, timeH1_pointers_to_send[0][0], sizeof(timeH1_pointers_to_send), true);

prev_H1 = received_digit;

}
	
received_digit=*received_time[2]%10;

if (prev_H2!= received_digit){

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				timeH2_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				timeH2_pointers_to_send[i][j]=H2_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,*pointer_to_commands_isr_timeH2, sizeof(array_of_commands_ISR_timeH2), true);

spi_transmit_isr(spi,false, timeH2_pointers_to_send[0][0], sizeof(timeH2_pointers_to_send), true);

prev_H2 = received_digit;

}


received_digit=*received_time[1]-*received_time[1]%10/10;

if (prev_M1!= received_digit){


	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				timeM1_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				timeM1_pointers_to_send[i][j]=M1_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,*pointer_to_commands_isr_timeM1, sizeof(array_of_commands_ISR_timeM1), true);

spi_transmit_isr(spi,false, timeM1_pointers_to_send[0][0], sizeof(timeM1_pointers_to_send), true);

prev_M1 = received_digit;

}

received_digit=*received_time[1]%10;

if (prev_M2!= received_digit){

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				timeM2_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				timeM2_pointers_to_send[i][j]=M2_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,*pointer_to_commands_isr_timeM2, sizeof(array_of_commands_ISR_timeM2), true);

spi_transmit_isr(spi,false, timeM2_pointers_to_send[0][0], sizeof(timeM2_pointers_to_send), true);

prev_M2 = received_digit;

}

received_digit=*received_time[0]-*received_time[0]%10/10;

if (prev_S1!= received_digit){


	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				timeS1_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				timeS1_pointers_to_send[i][j]=S1_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,*pointer_to_commands_isr_timeS1, sizeof(array_of_commands_ISR_timeS1), true);

spi_transmit_isr(spi,false, timeS1_pointers_to_send[0][0], sizeof(timeS1_pointers_to_send), true);

prev_S1 = received_digit;

}

received_digit=*received_time[0]%10;

if (prev_S2!= received_digit){

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				timeS2_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				timeS2_pointers_to_send[i][j]=S2_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,*pointer_to_commands_isr_timeS2, sizeof(array_of_commands_ISR_timeS2), true);

spi_transmit_isr(spi,false, timeS2_pointers_to_send[0][0], sizeof(timeS2_pointers_to_send), true);

prev_M2 = received_digit;

}

vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));

}
}

int  pressed_key (void ){

uint32_t col=-1;

uint8_t key=-1;


xTaskNotifyWait(0, 0, &col, portMAX_DELAY);

// KEYS 11 * ,12 # ARE SPECIAL KEYS


uint32_t low_pins_status = REG_READ(GPIO_IN_REG);

if (col==1){

if (low_pins_status&(1<<GPIO_KEYPADROW0)){

key=1;

}

if (low_pins_status&(1<<GPIO_KEYPADROW1)){

key=4;

}

if (low_pins_status&(1<<GPIO_KEYPADROW3)){

key=7;

}

if (low_pins_status&(1<<GPIO_KEYPADROW3)){

key=11;

}
}else {

taskYIELD();
}

if (col==2){

if (low_pins_status&(1<<GPIO_KEYPADROW0)){

key=2;

}

if (low_pins_status&(1<<GPIO_KEYPADROW1)){

key=5;

}

if (low_pins_status&(1<<GPIO_KEYPADROW3)){

key=8;

}

if (low_pins_status&(1<<GPIO_KEYPADROW3)){

key=0;

}
}else {

taskYIELD();
}


if (col==3){

if (low_pins_status&(1<<GPIO_KEYPADROW0)){

key=3;

}

if (low_pins_status&(1<<GPIO_KEYPADROW1)){

key=6;

}

if (low_pins_status&(1<<GPIO_KEYPADROW3)){

key=9;

}

if (low_pins_status&(1<<GPIO_KEYPADROW3)){

key=12;

}

}else {

taskYIELD();
}

return key;

}


void display_update_SET_TIME(void * pvparameters){

int key=-1;

int h1=-1;

//time[0]=seconds time[1]=minutes time[0]=seconds

uint8_t time[3];

key=pressed_key ();

if ((key!=11)){

taskYIELD();

}

spi_transmit_isr(spi,true,*pointer_to_commands_isr_BANNERST, sizeof(array_of_commands_ISR_BANNERST), true);

spi_transmit_isr(spi,false, setup_time_bkg_pointers[0][0], sizeof(setup_time_bkg_pointers), true);


key=pressed_key();

while ((key!=12)|(key>2)|key<0 ){

key=pressed_key();

}

if(key==12){

xTaskNotifyGive(xtaskHandleResetTime);
taskYIELD();

}

else if((key>0)&(key<3)){

time[2]= key<<1;


	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[key][j]&(1<<i))>0){
				timeH1_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				timeH1_pointers_to_send[i][j]=H1_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,*pointer_to_commands_isr_timeH1, sizeof(array_of_commands_ISR_timeH1), true);

spi_transmit_isr(spi,false, timeH1_pointers_to_send[0][0], sizeof(timeH1_pointers_to_send), true);

h1=key;
}
else if (key==0){

spi_transmit_isr(spi,true,*pointer_to_commands_isr_timeH1, sizeof(array_of_commands_ISR_timeH1), true);

spi_transmit_isr(spi,false, H1_time_pointers[0][0], sizeof(H1_time_pointers), true);


}


key=pressed_key();

while ((key!=12)|!(h1==2 & key<4) |((h1<2) & ((key==0) | (key>0))) ){

key=pressed_key();

}


if(key==12){

xTaskNotifyGive(xtaskHandleResetTime);
taskYIELD();

}

time[2]= time[2] | key;

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[key][j]&(1<<i))>0){
				timeH2_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				timeH2_pointers_to_send[i][j]=H2_time_pointers[i][j];

				}
		}

spi_transmit_isr(spi,true,*pointer_to_commands_isr_timeH2, sizeof(array_of_commands_ISR_timeH2), true);

spi_transmit_isr(spi,false, timeH2_pointers_to_send[0][0], sizeof(timeH2_pointers_to_send), true);


}
 
key=pressed_key();

while ((key!=12)|(key>6) | (key<0)) {

key=pressed_key();

}


if(key==12){

xTaskNotifyGive(xtaskHandleResetTime);
taskYIELD();

}


time[1]= key<<1;

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[key][j]&(1<<i))>0){
				timeM1_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				timeM1_pointers_to_send[i][j]=M1_time_pointers[i][j];;

				}
		}
	}

spi_transmit_isr(spi,true,*pointer_to_commands_isr_timeM1, sizeof(array_of_commands_ISR_timeM1), true);

spi_transmit_isr(spi,false, timeM1_pointers_to_send[0][0], sizeof(timeM1_pointers_to_send), true);


key=pressed_key();

while ((key!=12)|(key>9) | (key<0)) {

key=pressed_key();

}


if(key==12){

xTaskNotifyGive(xtaskHandleResetTime);
taskYIELD();

}

time[1]= time[1] | key;

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[key][j]&(1<<i))>0){
				timeM2_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				timeM2_pointers_to_send[i][j]=M2_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,*pointer_to_commands_isr_timeM2, sizeof(array_of_commands_ISR_timeM2), true);

spi_transmit_isr(spi,false, timeM2_pointers_to_send[0][0], sizeof(timeM2_pointers_to_send), true);


key=pressed_key();

while ((key!=12)|(key>6) | (key<0)) {

key=pressed_key();

}

if(key==12){

xTaskNotifyGive(xtaskHandleResetTime);
taskYIELD();

}

time[0]= key<<1;


	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[key][j]&(1<<i))>0){
				timeS1_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				timeS1_pointers_to_send[i][j]=S1_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,*pointer_to_commands_isr_timeS1, sizeof(array_of_commands_ISR_timeS1), true);

spi_transmit_isr(spi,false, timeS1_pointers_to_send[0][0], sizeof(timeS1_pointers_to_send), true);


key=pressed_key();


while ((key!=12)|(key>9) | (key<0)) {

key=pressed_key();

}


if(key==12){

xTaskNotifyGive(xtaskHandleResetTime);
taskYIELD();

}

else if(key>0|key==0){

time[0]= time[0] | key;

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[key][j]&(1<<i))>0){
				timeS2_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				timeS2_pointers_to_send[i][j]=S2_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,*pointer_to_commands_isr_timeS2, sizeof(array_of_commands_ISR_timeS2), true);

spi_transmit_isr(spi,false, timeS2_pointers_to_send[0][0], sizeof(timeS2_pointers_to_send), true);

}

ic2_setup_time(time[0], time[1], time[2]);

}


void display_update_RESET_TIME(void * pvparameters){

for(;;){

ulTaskNotifyTake(pdTRUE,portMAX_DELAY);

spi_transmit_isr(spi,true,*pointer_to_commands_isr_BANNERST, sizeof(array_of_commands_ISR_BANNERST), true);

spi_transmit_isr(spi,false, set_time_pointers[0][0], sizeof(set_time_pointers), true);

}

}

void special_key_call (void * pvparamenters){

for(;;){
ulTaskNotifyTake(pdTRUE,portMAX_DELAY);

uint32_t low_pins_status = REG_READ(GPIO_IN_REG);

if (low_pins_status&(1<<GPIO_KEYPADROW3)){

xTaskNotifyGive(xtaskHandleSetTime);

}
}
}


