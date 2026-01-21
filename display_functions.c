/*
 * display_functions.c
 *
 *  Created on: Jan 19, 2026
 *      Author: dario Lobos
 */

#include "freertos/projdefs.h"
#include "main.h"
#include "display_commands.h"
#include "display_commands.c"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "background.c"
#include "portmacro.h"
#include "setuptimebackground.c"
#include "font_numbers.c"

TaskHandle_t xtaskHandleDisplay= NULL;
TaskHandle_t xtaskHandleFrame = NULL;

static volatile uint16_t* ac_pointers_to_send[ROWAC];

static void display_init(void *pvparameter){

spi_device_handle_t spi= pvparameter;

display_allocation();

setup_time_bkg_allocation();

xTaskNotifyGive(xtaskHandleFrame);

spi_polling(spi, NORON, true);
spi_polling(spi, COLMOD, true);
spi_polling(spi, PCOLMOD, true);
spi_polling(spi, DISPON, true);
spi_polling(spi, CASET, true);
spi_polling(spi, 0, true);// ALL THE SCREEN
spi_polling(spi, 0X7F, true); // ALL THE SCREEN
spi_polling(spi, RASET, true);
spi_polling(spi, 0, true);// ALL THE SCREEN
spi_polling(spi, 0X9F, true);// ALL THE SCREEN
spi_polling(spi, RAMWR, true);// ALL THE SCREEN

for (int i = 0; i < ROWARRAY; i++) {

	for (int j = 0; j < COLARRAY; j++) {

spi_polling(spi, background_pointers[i][j], true);// ALL THE SCREEN

}
}

spi_device_polling_end(spi, portMAX_DELAY);

// BACKGROUND READY AT FIRST

// free memory and delete task awaiting next task finish


ulTaskNotifyTake(pdTRUE,portMAX_DELAY);

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

static void display_update (void *pvparameter){

int received_voltage;

int received_digits[5];
 
int prev_digits[5];

int digits;

uint8_t received_digit;

uint8_t commands[10];//unfinished

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

received_voltage= (int) pvparameter; 

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

unfinished
spi_transmit_isr(spi_device_handle_t spi,void* commands, void* data, bool keep_cs_active)

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

unfinished
spi_transmit_isr(spi_device_handle_t spi,void* commands, void* data, bool keep_cs_active)


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

unfinished
spi_transmit_isr(spi_device_handle_t spi,void* commands, void* data, bool keep_cs_active)

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

unfinished
spi_transmit_isr(spi_device_handle_t spi,void* commands, void* data, bool keep_cs_active)


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

unfinished
spi_transmit_isr(spi_device_handle_t spi,void* commands, void* data, bool keep_cs_active)

 

}
}