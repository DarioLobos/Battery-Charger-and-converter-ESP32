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
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "main.h"
#include "display_commands.h"
#include "display_commands.c"
#include "background.c"
#include "portmacro.h"
#include "setuptimebackground.c"
#include "font_numbers.c"
#include "adc_functions.c"
#include "ic2_commands_RTC_CLK.c"

TaskHandle_t xtaskHandleDisplay= NULL;
TaskHandle_t xtaskHandleFrame = NULL;

static volatile uint16_t* ac_pointers_to_send[ROWAC];

static volatile uint16_t* time_pointers_to_send[ROWTIME];


static uint8_t array_of_commands_poll[11]={ NORON,COLMOD,PCOLMOD,DISPON,CASET,0,COLARRAY-1,RASET,0,ROWARRAY-1,RAMWR};

static uint8_t * pointer_to_commands_poll=&array_of_commands_poll[0]; 

static uint8_t array_of_commands_ISR_time[7]={CASET,TIMECASETL,TIMECASETH,RASET,TIMERASETL,TIMERASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_time=&array_of_commands_ISR_time[0];

uint8_t array_of_commands_ISR_AC[7]={CASET,ACCASETL,ACCASETH,RASET,ACRASETL,ACRASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_ac=&array_of_commands_ISR_AC[0];

static uint8_t array_of_commands_ISR_DC[7]={CASET,DCCASETL,DCCASETH,RASET,DCRASETL,DCRASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_dc=&array_of_commands_ISR_DC[0];

static uint8_t array_of_commands_ISR_BANNERST[7]={CASET,STCASETL,STCASETH,RASET,STRASETL,STRASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_BANNERST=&array_of_commands_ISR_DC[0];

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

frame_time();
frame_AC();
frame_DC();

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


int time_array[3];

 
int digits;

uint8_t received_digit;

TickType_t xLastWakeTime;


// Allocate memory for each row in PSRAM
for (int i = 0; i < ROWTIME; i++) {
    time_pointers_to_send[i] = (uint16_t*)heap_caps_malloc(COLTIME * sizeof(uint16_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (time_pointers_to_send[i] == NULL) {
        printf("Failed to allocate AC row %d\n", i);
        return;
        }
}


for(;;){

xLastWakeTime = xTaskGetTickCount();

ic2_read_time();

digits=-1;

// *received_time[0]= seconds  *received_time[1]= minutes  *received_time[2]= hours

if ((received_digit=*received_time[2]-*received_time[2]%10/10)>0){

digits=0;

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				time_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				time_pointers_to_send[i][j]=ac_pointers[i][j];

				}
		}
	}
}

received_digit=*received_time[2]%10;
digits++; 

	for (int j=digits*8;j<(digits*8+8);j++){
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				time_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				time_pointers_to_send[i][j]=ac_pointers[i][j];

				}
		}
	}

digits++;

	for (int j=digits*8;j<(digits*8+8);j++){
		for(int i=0;i<8;i++){
			if((font_bits[10][j]&(1<<i))>0){
				ac_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				ac_pointers_to_send[i][j]=ac_pointers[i][j];

				}
		}
	}


received_digit=*received_time[1]-*received_time[1]%10/10;

digits++; 

	for (int j=digits*8;j<(digits*8+8);j++){
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				time_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				time_pointers_to_send[i][j]=ac_pointers[i][j];

				}
		}
	}


}

received_digit=*received_time[1]%10;
digits++; 

	for (int j=digits*8;j<(digits*8+8);j++){
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				time_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				time_pointers_to_send[i][j]=ac_pointers[i][j];

				}
		}
	}


digits++;

	for (int j=digits*8;j<(digits*8+8);j++){
		for(int i=0;i<8;i++){
			if((font_bits[10][j]&(1<<i))>0){
				ac_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				ac_pointers_to_send[i][j]=ac_pointers[i][j];

				}
		}
	}

received_digit=*received_time[0]-*received_time[0]%10/10;
digits++; 

	for (int j=digits*8;j<(digits*8+8);j++){
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				time_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				time_pointers_to_send[i][j]=ac_pointers[i][j];

				}
		}
	}



received_digit=*received_time[0]%10;
digits++; 

	for (int j=digits*8;j<(digits*8+8);j++){
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				time_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				time_pointers_to_send[i][j]=ac_pointers[i][j];

				}
		}
	}



spi_transmit_isr(spi,true,*pointer_to_commands_isr_time, sizeof(array_of_commands_ISR_time), true);

spi_transmit_isr(spi,false, time_pointers_to_send[0][0], sizeof(time_pointers_to_send), true);
 
vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));

}
