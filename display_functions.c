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
#include <rom/ets_sys.h>
#include <inttypes.h>
#include <sys/types.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "background.c"


static volatile uint16_t* ac_pointers_to_send[ROWAC];

static volatile uint16_t* dc_pointers_to_send[ROWDC];

static volatile uint16_t* timeH1_pointers_to_send[ROWTIME];

static volatile uint16_t* timeH2_pointers_to_send[ROWTIME];

static volatile uint16_t* timeD1_pointers_to_send[ROWTIME];

static volatile uint16_t* timeM1_pointers_to_send[ROWTIME];

static volatile uint16_t* timeM2_pointers_to_send[ROWTIME];

static volatile uint16_t* timeD2_pointers_to_send[ROWTIME];

static volatile uint16_t* timeS1_pointers_to_send[ROWTIME];

static volatile uint16_t* timeS2_pointers_to_send[ROWTIME];


static uint8_t array_of_commands_ISR_timeH1[7]={CASET,H1TCASETL,H1TCASETH,RASET,TIMERASETL,TIMERASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_timeH1=&array_of_commands_ISR_timeH1[0];

static uint8_t array_of_commands_ISR_timeH2[7]={CASET,H2TCASETL,H2TCASETH,RASET,TIMERASETL,TIMERASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_timeH2=&array_of_commands_ISR_timeH2[0];

static uint8_t array_of_commands_ISR_timeD1[7]={CASET,D1TCASETL,D1TCASETH,RASET,TIMERASETL,TIMERASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_timeD1=&array_of_commands_ISR_timeH2[0];

static uint8_t array_of_commands_ISR_timeM1[7]={CASET,M1TCASETL,M1TCASETH,RASET,TIMERASETL,TIMERASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_timeM1=&array_of_commands_ISR_timeM1[0];

static uint8_t array_of_commands_ISR_timeM2[7]={CASET,M2TCASETL,M2TCASETH,RASET,TIMERASETL,TIMERASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_timeM2=&array_of_commands_ISR_timeM2[0];

static uint8_t array_of_commands_ISR_timeD2[7]={CASET,D2TCASETL,D2TCASETH,RASET,TIMERASETL,TIMERASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_timeD2=&array_of_commands_ISR_timeH2[0];

static uint8_t array_of_commands_ISR_timeS1[7]={CASET,S1TCASETL,S1TCASETH,RASET,TIMERASETL,TIMERASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_timeS1=&array_of_commands_ISR_timeS1[0];

static uint8_t array_of_commands_ISR_timeS2[7]={CASET,S2TCASETL,S2TCASETH,RASET,TIMERASETL,TIMERASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_timeS2=&array_of_commands_ISR_timeS2[0];


static uint8_t array_of_commands_ISR_SCH_timeH1[7]={CASET,SCHH1TCASETL,SCHH1TCASETH,RASET,SCHTIMERASETL,SCHTIMERASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_SCH_timeH1=&array_of_commands_ISR_SCH_timeH1[0];

static uint8_t array_of_commands_ISR_SCH_timeH2[7]={CASET,SCHH2TCASETL,SCHH2TCASETH,RASET,SCHTIMERASETL,SCHTIMERASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_SCH_timeH2=&array_of_commands_ISR_SCH_timeH2[0];

static uint8_t array_of_commands_ISR_SCH_timeD1[7]={CASET,SCHD1TCASETL,SCHD1TCASETH,RASET,SCHTIMERASETL,SCHTIMERASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_SCH_timeD1=&array_of_commands_ISR_timeH2[0];

static uint8_t array_of_commands_ISR_SCH_timeM1[7]={CASET,SCHM1TCASETL,SCHM1TCASETH,RASET,SCHTIMERASETL,SCHTIMERASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_SCH_timeM1=&array_of_commands_ISR_SCH_timeM1[0];

static uint8_t array_of_commands_ISR_SCH_timeM2[7]={CASET,SCHM2TCASETL,SCHM2TCASETH,RASET,SCHTIMERASETL,SCHTIMERASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_SCH_timeM2=&array_of_commands_ISR_SCH_timeM2[0];

uint8_t array_of_commands_ISR_AC[7]={CASET,ACCASETL,ACCASETH,RASET,ACRASETL,ACRASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_ac=&array_of_commands_ISR_AC[0];

static uint8_t array_of_commands_ISR_DC[7]={CASET,DCCASETL,DCCASETH,RASET,DCRASETL,DCRASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_dc=&array_of_commands_ISR_DC[0];

static uint8_t array_of_commands_ISR_BANNERST[7]={CASET,STCASETL,STCASETH,RASET,STRASETL,STRASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_BANNERST=&array_of_commands_ISR_BANNERST[0];

static uint8_t array_of_commands_ISR_BANNERSCH[7]={CASET,STCASETL,STCASETH,RASET,STRASETL,STRASETH,RAMWR};

static uint8_t * pointer_to_commands_isr_BANNERSCH=&array_of_commands_ISR_BANNERSCH[0];


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

received_voltage= *pointer_ADC_results_AC; 

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
else if((received_digit=((received_voltage%100-received_voltage%10)/10)>0) | (digits >-1)){
digits++; 

	for (int j=digits*8+8;j<(digits*8+16);j++){
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
received_digit=(received_voltage)%10;

digits++; 

	for (int j=digits*8+8;j<(digits*8+16);j++){
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
	for (int j=32;j<40;j++){
		for(int i=0;i<8;i++){
			if((font_bits[11][j]&(1<<i))>0){
				ac_pointers_to_send[i][j]=ACCOLOR;
				}
			else{
				ac_pointers_to_send[i][j]=ac_pointers[i][j];

				}
		}
	}
spi_transmit_isr(spi,true,pointer_to_commands_isr_ac, sizeof(array_of_commands_ISR_AC), true);


for(int i =0; i<8;i++){
spi_transmit_isr(spi,false, (uint8_t*) ac_pointers_to_send[i], (ACCASETH-ACCASETL)*16, true);
 }

xTaskNotifyGive(xtaskHandledisplay_update_DC);

}
}

// THE ONLY VOLTAGE THAT SCREEN SHOW IS THE BATTERY VOLTAGE TO DON'T COMPLICATE THE DEVICE

static void display_update_DC (void *pvparameter){

int received_voltage;
 
int digits;

uint8_t received_digit;


// Allocate memory for each row in PSRAM
for (int i = 0; i < ROWDC; i++) {
    dc_pointers_to_send[i] = (uint16_t*)heap_caps_malloc(COLDC * sizeof(uint16_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (dc_pointers_to_send[i] == NULL) {
        printf("Failed to allocate AC row %d\n", i);
        return;
        }
}


for(;;){

ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

digits=-1;

received_voltage= *adc_dc_voltage_pointers[3]; 


if ((received_digit=received_voltage-received_voltage%1000/1000)>0){

digits=0;

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				dc_pointers_to_send[i][j]=DCCOLOR;
				}
			else{
				dc_pointers_to_send[i][j]=dc_pointers[i][j];

				}
		}
	}

}

else if((received_digit=((received_voltage%1000-received_voltage%100)/100)>0) | (digits >-1)){
digits++; 
	for (int j=digits*8;j<(digits*8+8);j++){
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				dc_pointers_to_send[i][j]=DCCOLOR;
				}
			else{
				dc_pointers_to_send[i][j]=dc_pointers[i][j];

				}
		}
	}


}
else if((received_digit=((received_voltage%100-received_voltage%10)/10)>0) | (digits >-1)){
digits++; 
	for (int j=digits*8+8;j<(digits*8+16);j++){
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				dc_pointers_to_send[i][j]=DCCOLOR;
				}
			else{
				dc_pointers_to_send[i][j]=dc_pointers[i][j];

				}
		}
	}
}
else {

received_digit=received_voltage%10;
digits++; 

	for (int j=digits*8+8;j<(digits*8+16);j++){
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				dc_pointers_to_send[i][j]=DCCOLOR;
				}
			else{
				dc_pointers_to_send[i][j]=dc_pointers[i][j];

				}
		}
	}
}

	for (int j=24;j<32;j++){
		for(int i=0;i<8;i++){
			if((font_bits[11][j]&(1<<i))>0){
				dc_pointers_to_send[i][j]=DCCOLOR;
				}
			else{
				dc_pointers_to_send[i][j]=dc_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_dc, sizeof(array_of_commands_ISR_AC), true);


for (int i=0; i<8;i++){
spi_transmit_isr(spi,false, (uint8_t*)dc_pointers_to_send[i], (DCCASETH-DCCASETL)*16, true);
 }

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
    timeD1_pointers_to_send[i] = (uint16_t*)heap_caps_malloc(D1COLTIME * sizeof(uint16_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (timeD1_pointers_to_send[i] == NULL) {
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
    timeD2_pointers_to_send[i] = (uint16_t*)heap_caps_malloc(D2COLTIME * sizeof(uint16_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (timeD2_pointers_to_send[i] == NULL) {
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

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[11][j]&(1<<i))>0){
				timeD1_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeD1_pointers_to_send[i][j]=D1_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_timeD1, sizeof(array_of_commands_ISR_timeD1), true);

for (int i =0; i<8; i++){
spi_transmit_isr(spi,false, (uint8_t*) timeD1_pointers_to_send[i], D1TCASETH*D1TCASETL+16, true);
}

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[11][j]&(1<<i))>0){
				timeD2_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeD2_pointers_to_send[i][j]=D1_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_timeD2, sizeof(array_of_commands_ISR_timeD2), true);

for (int i =0; i<8; i++){
spi_transmit_isr(spi,false, (uint8_t*) timeD2_pointers_to_send[i], D2TCASETH*D2TCASETL+16, true);
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
				timeH1_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeH1_pointers_to_send[i][j]=H1_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_timeH1, sizeof(array_of_commands_ISR_timeH1), true);

for (int i =0; i<8; i++){
spi_transmit_isr(spi,false, (uint8_t*) timeH1_pointers_to_send[i], H1TCASETH*H1TCASETL+16, true);
}

prev_H1 = received_digit;

}
	
received_digit=*received_time[2]%10;

if (prev_H2!= received_digit){

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				timeH2_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeH2_pointers_to_send[i][j]=H2_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_timeH2, sizeof(array_of_commands_ISR_timeH2), true);

for (int i =0; i<8; i++){
spi_transmit_isr(spi,false, (uint8_t*) timeH2_pointers_to_send[i], H2TCASETH*H2TCASETL+16, true);
}

prev_H2 = received_digit;

}


received_digit=*received_time[1]-*received_time[1]%10/10;

if (prev_M1!= received_digit){


	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				timeM1_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeM1_pointers_to_send[i][j]=M1_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_timeM1, sizeof(array_of_commands_ISR_timeM1), true);

for (int i =0; i<8; i++){
spi_transmit_isr(spi,false, (uint8_t*) timeM1_pointers_to_send[i], M1TCASETH*M1TCASETL+16, true);
}

prev_M1 = received_digit;

}

received_digit=*received_time[1]%10;

if (prev_M2!= received_digit){

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				timeM2_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeM2_pointers_to_send[i][j]=M2_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_timeM2, sizeof(array_of_commands_ISR_timeM2), true);

for (int i =0; i<8; i++){
spi_transmit_isr(spi,false, (uint8_t*) timeM2_pointers_to_send[i], M2TCASETH*M2TCASETL+16, true);
}

prev_M2 = received_digit;

}

received_digit=*received_time[0]-*received_time[0]%10/10;

if (prev_S1!= received_digit){


	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				timeS1_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeS1_pointers_to_send[i][j]=S1_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_timeS1, sizeof(array_of_commands_ISR_timeS1), true);

for (int i =0; i<8; i++){
spi_transmit_isr(spi,false, (uint8_t*) timeS1_pointers_to_send[i], S1TCASETH*S1TCASETL+16, true);
}

prev_S1 = received_digit;

}

received_digit=*received_time[0]%10;

if (prev_S2!= received_digit){

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[received_digit][j]&(1<<i))>0){
				timeS2_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeS2_pointers_to_send[i][j]=S2_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_timeS2, sizeof(array_of_commands_ISR_timeS2), true);

for (int i =0; i<8; i++){
spi_transmit_isr(spi,false, (uint8_t*) timeS2_pointers_to_send[i], S2TCASETH*S2TCASETL+16, true);
}

prev_M2 = received_digit;

}

vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));

}
}


void display_update_SET_SCHEDULER_TIME(void){

int key;

int h1=-1;

//time[0]=seconds time[1]=minutes time[0]=seconds

uint8_t time[2];

for(;;){


spi_transmit_isr(spi,true,pointer_to_commands_isr_BANNERSCH, sizeof(array_of_commands_ISR_BANNERSCH), true);
for (int i=0; i < (STRASETH-STRASETL); i++){
spi_transmit_isr(spi,false, (uint8_t*) seton_time1_bkg_pointers[i],(STCASETH-STCASETL)*16 , true);
}

key=-1;

while ((key< 0)){

taskENTER_CRITICAL(0);
		key=pressed_key(MCPA0,1);
		mcp23017_set_pins_PortA_high(MCPA0);
		ets_delay_us(100);

  		if (key != -1) {
			continue;} 

		key=pressed_key(MCPA1,1);

		mcp23017_set_pins_PortA_high(MCPA1);
		ets_delay_us(100);

  		if (key != -1){
 			continue; }

		key=pressed_key(MCPA2,1);
		mcp23017_set_pins_PortA_high(MCPA2);
		ets_delay_us(100);

  			if (key != -1){ 
			continue;}

taskEXIT_CRITICAL(0);

		mcp23017_set_pins_PortA_high(MCPA0|MCPA1|MCPA2);
		key=pressed_key(-1,-1);
		mcp23017_set_pins_PortA_high(0);
		ets_delay_us(100);

}

if(key==12){

xTaskNotifyGive(xtaskHandleReset_BKG_Time);
mcp23017_set_pins_PortA_high(MCPA0);
taskYIELD();
continue;
}


if(key==11){

spi_transmit_isr(spi,true,pointer_to_commands_isr_BANNERSCH, sizeof(array_of_commands_ISR_BANNERSCH), true);

for (int i=0; i < (STRASETH-STRASETL); i++){
spi_transmit_isr(spi,false, (uint8_t*) scheduleroff_bkg_pointers[i],(STCASETH-STCASETL)*16 , true);
}

alarm_OFF();
vTaskDelay(pdTICKS_TO_MS(1000));
xTaskNotifyGive(xtaskHandleReset_BKG_Time);
taskYIELD();
continue;
}



	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[11][j]&(1<<i))>0){
				timeD1_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeD1_pointers_to_send[i][j]=D1_time_SCH_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_SCH_timeD1, sizeof(array_of_commands_ISR_SCH_timeD1), true);


for(int i=0; i<(SCHTIMERASETH-SCHTIMERASETL); i++){
spi_transmit_isr(spi,false,(uint8_t*) timeD1_pointers_to_send[i], (SCHD1TCASETH-SCHD1TCASETL)*16, true);
}

key=-1;

while ((key!=12)&&(key!=1)&&(key!=2) ){

taskENTER_CRITICAL(0);
		key=pressed_key(MCPA0,1);
		mcp23017_set_pins_PortA_high(MCPA0);
		ets_delay_us(100);

  		if (key != -1) {
			continue;} 

		key=pressed_key(MCPA1,1);

		mcp23017_set_pins_PortA_high(MCPA1);
		ets_delay_us(100);

  		if (key != -1){
 			continue; }

		key=pressed_key(MCPA2,1);
		mcp23017_set_pins_PortA_high(MCPA2);
		ets_delay_us(100);

  			if (key != -1){ 
			continue;}

taskEXIT_CRITICAL(0);

		mcp23017_set_pins_PortA_high(MCPA0|MCPA1|MCPA2);
		key=pressed_key(-1,-1);
		mcp23017_set_pins_PortA_high(0);
		ets_delay_us(100);

}

if(key==1){

key=-1;

while ((key!=12)&&(key>2)&&(key<0) ){

taskENTER_CRITICAL(0);
		key=pressed_key(MCPA0,1);
		mcp23017_set_pins_PortA_high(MCPA0);
		ets_delay_us(100);

  		if (key != -1) {
			continue;} 

		key=pressed_key(MCPA1,1);

		mcp23017_set_pins_PortA_high(MCPA1);
		ets_delay_us(100);

  		if (key != -1){
 			continue; }

		key=pressed_key(MCPA2,1);
		mcp23017_set_pins_PortA_high(MCPA2);
		ets_delay_us(100);

  			if (key != -1){ 
			continue;}

taskEXIT_CRITICAL(0);

		mcp23017_set_pins_PortA_high(MCPA0|MCPA1|MCPA2);
		key=pressed_key(-1,-1);
		mcp23017_set_pins_PortA_high(0);
		ets_delay_us(100);

}

if(key==12){

xTaskNotifyGive(xtaskHandleReset_BKG_Time);
mcp23017_set_pins_PortA_high(MCPA0);
taskYIELD();
continue;

}

else if((key>0)&&(key<3)){

time[1]= key*10;


	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[key][j]&(1<<i))>0){
				timeH1_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeH1_pointers_to_send[i][j]=H1_time_SCH_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_SCH_timeH1, sizeof(array_of_commands_ISR_SCH_timeH1), true);

for(int i=0; i<(SCHTIMERASETH-SCHTIMERASETL); i++){
spi_transmit_isr(spi,false,(uint8_t*) timeH1_pointers_to_send[i], (SCHD1TCASETH-SCHD1TCASETL)*16, true);
}

h1=key;
}
else if (key==0){

spi_transmit_isr(spi,true,pointer_to_commands_isr_SCH_timeH1, sizeof(array_of_commands_ISR_SCH_timeH1), true);

for(int i=0; i<(SCHTIMERASETH-SCHTIMERASETL); i++){
spi_transmit_isr(spi,false,(uint8_t*) H1_time_SCH_pointers[i], (SCHD1TCASETH-SCHD1TCASETL)*16, true);
}

}

key=-1;

while ((key!=12) && !(h1==2 && key<4) && !((h1<2) && (key>=0)) ){

taskENTER_CRITICAL(0);
		key=pressed_key(MCPA0,1);
		mcp23017_set_pins_PortA_high(MCPA0);
		ets_delay_us(100);

  		if (key != -1) {
			continue;} 

		key=pressed_key(MCPA1,1);

		mcp23017_set_pins_PortA_high(MCPA1);
		ets_delay_us(100);

  		if (key != -1){
 			continue; }

		key=pressed_key(MCPA2,1);
		mcp23017_set_pins_PortA_high(MCPA2);
		ets_delay_us(100);

  			if (key != -1){ 
			continue;}

taskEXIT_CRITICAL(0);

		mcp23017_set_pins_PortA_high(MCPA0|MCPA1|MCPA2);
		key=pressed_key(-1,-1);
		mcp23017_set_pins_PortA_high(0);
		ets_delay_us(100);

}


if(key==12){

xTaskNotifyGive(xtaskHandleReset_BKG_Time);
mcp23017_set_pins_PortA_high(MCPA0);
taskYIELD();
continue;

}

time[1]+= key;

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[key][j]&(1<<i))>0){
				timeH2_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeH2_pointers_to_send[i][j]=H2_time_SCH_pointers[i][j];

				}
		}

spi_transmit_isr(spi,true,pointer_to_commands_isr_SCH_timeH2, sizeof(array_of_commands_ISR_SCH_timeH2), true);

for(int i=0; i<(SCHTIMERASETH-SCHTIMERASETL); i++){
spi_transmit_isr(spi,false,(uint8_t*) H2_time_SCH_pointers[i], (SCHD1TCASETH-SCHD1TCASETL)*16, true);
}

}
key=-1;
 
while ((key!=12)&& (key>6) && (key<0)) {

taskENTER_CRITICAL(0);
		key=pressed_key(MCPA0,1);
		mcp23017_set_pins_PortA_high(MCPA0);
		ets_delay_us(100);

  		if (key != -1) {
			continue;} 

		key=pressed_key(MCPA1,1);

		mcp23017_set_pins_PortA_high(MCPA1);
		ets_delay_us(100);

  		if (key != -1){
 			continue; }

		key=pressed_key(MCPA2,1);
		mcp23017_set_pins_PortA_high(MCPA2);
		ets_delay_us(100);

  			if (key != -1){ 
			continue;}

taskEXIT_CRITICAL(0);

		mcp23017_set_pins_PortA_high(MCPA0|MCPA1|MCPA2);
		key=pressed_key(-1,-1);
		mcp23017_set_pins_PortA_high(0);
		ets_delay_us(100);

}


if(key==12){

xTaskNotifyGive(xtaskHandleReset_BKG_Time);
mcp23017_set_pins_PortA_high(MCPA0);
taskYIELD();
continue;

}


time[0]= key*10;

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[key][j]&(1<<i))>0){
				timeM1_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeM1_pointers_to_send[i][j]=M1_time_SCH_pointers[i][j];;

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_SCH_timeM1, sizeof(array_of_commands_ISR_SCH_timeM1), true);

for(int i=0; i<(SCHTIMERASETH-SCHTIMERASETL); i++){
spi_transmit_isr(spi,false,(uint8_t*) M1_time_SCH_pointers[i], (SCHD1TCASETH-SCHD1TCASETL)*16, true);
}


key=-1;

while ((key!=12) && (key>6) && (key<0)) {

taskENTER_CRITICAL(0);
		key=pressed_key(MCPA0,1);
		mcp23017_set_pins_PortA_high(MCPA0);
		ets_delay_us(100);

  		if (key != -1) {
			continue;} 

		key=pressed_key(MCPA1,1);

		mcp23017_set_pins_PortA_high(MCPA1);
		ets_delay_us(100);

  		if (key != -1){
 			continue; }

		key=pressed_key(MCPA2,1);
		mcp23017_set_pins_PortA_high(MCPA2);
		ets_delay_us(100);

  			if (key != -1){ 
			continue;}

taskEXIT_CRITICAL(0);

		mcp23017_set_pins_PortA_high(MCPA0|MCPA1|MCPA2);
		key=pressed_key(-1,-1);
		mcp23017_set_pins_PortA_high(0);
		ets_delay_us(100);

}


if(key==12){

xTaskNotifyGive(xtaskHandleReset_BKG_Time);
mcp23017_set_pins_PortA_high(MCPA0);
taskYIELD();
continue;
}

time[0]+= key;

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[key][j]&(1<<i))>0){
				timeM2_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeM2_pointers_to_send[i][j]=M2_time_SCH_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_SCH_timeM2, sizeof(array_of_commands_ISR_SCH_timeM2), true);

for(int i=0; i<(SCHTIMERASETH-SCHTIMERASETL); i++){
spi_transmit_isr(spi,false,(uint8_t*) M2_time_SCH_pointers[i], (SCHD1TCASETH-SCHD1TCASETL)*16, true);
}

ic2_setup_alarm1(time[0], time[1]);

vTaskDelay(pdMS_TO_TICKS(500));


int key;

int h1=-1;

//time[0]=seconds time[1]=minutes time[0]=seconds

uint8_t time[2];



spi_transmit_isr(spi,true,pointer_to_commands_isr_BANNERSCH, sizeof(array_of_commands_ISR_BANNERSCH), true);


for (int i=0; i < (STRASETH-STRASETL); i++){
spi_transmit_isr(spi,false, (uint8_t*) setoff_time_bkg_pointers[i],(STCASETH-STCASETL)*16 , true);
}

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[11][j]&(1<<i))>0){
				timeD1_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeD1_pointers_to_send[i][j]=H1_time_SCH_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_SCH_timeD1, sizeof(array_of_commands_ISR_SCH_timeD1), true);


for(int i=0; i<(SCHD1TCASETH-SCHD1TCASETL); i++){
spi_transmit_isr(spi,false,(uint8_t*) timeD1_pointers_to_send[i], (SCHTIMERASETH-SCHTIMERASETL)*16, true);
}

key=-1;

while ((key!=12)&&(key!=1)&&(key!=2)){

taskENTER_CRITICAL(0);
		key=pressed_key(MCPA0,1);
		mcp23017_set_pins_PortA_high(MCPA0);
		ets_delay_us(100);

  		if (key != -1) {
			continue;} 

		key=pressed_key(MCPA1,1);

		mcp23017_set_pins_PortA_high(MCPA1);
		ets_delay_us(100);

  		if (key != -1){
 			continue; }

		key=pressed_key(MCPA2,1);
		mcp23017_set_pins_PortA_high(MCPA2);
		ets_delay_us(100);

  			if (key != -1){ 
			continue;}

taskEXIT_CRITICAL(0);

		mcp23017_set_pins_PortA_high(MCPA0|MCPA1|MCPA2);
		key=pressed_key(-1,-1);
		mcp23017_set_pins_PortA_high(0);
		ets_delay_us(100);

}

if(key==1){

key=-1;

while ((key!=12)&&((key>2)||(key<0))){

taskENTER_CRITICAL(0);
		key=pressed_key(MCPA0,1);
		mcp23017_set_pins_PortA_high(MCPA0);
		ets_delay_us(100);

  		if (key != -1) {
			continue;} 

		key=pressed_key(MCPA1,1);

		mcp23017_set_pins_PortA_high(MCPA1);
		ets_delay_us(100);

  		if (key != -1){
 			continue; }

		key=pressed_key(MCPA2,1);
		mcp23017_set_pins_PortA_high(MCPA2);
		ets_delay_us(100);

  			if (key != -1){ 
			continue;}

taskEXIT_CRITICAL(0);

		mcp23017_set_pins_PortA_high(MCPA0|MCPA1|MCPA2);
		key=pressed_key(-1,-1);
		mcp23017_set_pins_PortA_high(0);
		ets_delay_us(100);

}

if(key==12){

xTaskNotifyGive(xtaskHandleReset_BKG_Time);
mcp23017_set_pins_PortA_high(MCPA0);
taskYIELD();
continue;
}

else if((key>0)&&(key<3)){

time[1]= key*10;


	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[key][j]&(1<<i))>0){
				timeH1_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeH1_pointers_to_send[i][j]=H1_time_SCH_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_SCH_timeH1, sizeof(array_of_commands_ISR_SCH_timeH1), true);

for(int i=0; i<(SCHD1TCASETH-SCHD1TCASETL); i++){
spi_transmit_isr(spi,false,(uint8_t*) timeH1_pointers_to_send[i], (SCHTIMERASETH-SCHTIMERASETL)*16, true);
}


h1=key;
}
else if (key==0){

spi_transmit_isr(spi,true,pointer_to_commands_isr_SCH_timeH1, sizeof(array_of_commands_ISR_SCH_timeH1), true);

for(int i=0; i<(SCHD1TCASETH-SCHD1TCASETL); i++){
spi_transmit_isr(spi,false,(uint8_t*) H1_time_SCH_pointers[i], (SCHTIMERASETH-SCHTIMERASETL)*16, true);
}

}


key=-1;

while ((key!=12) && !(h1==2 && key<4) && !((h1<2) &&  (key>=0)) ){

taskENTER_CRITICAL(0);
		key=pressed_key(MCPA0,1);
		mcp23017_set_pins_PortA_high(MCPA0);
		ets_delay_us(100);

  		if (key != -1) {
			continue;} 

		key=pressed_key(MCPA1,1);

		mcp23017_set_pins_PortA_high(MCPA1);
		ets_delay_us(100);

  		if (key != -1){
 			continue; }

		key=pressed_key(MCPA2,1);
		mcp23017_set_pins_PortA_high(MCPA2);
		ets_delay_us(100);

  			if (key != -1){ 
			continue;}

taskEXIT_CRITICAL(0);

		mcp23017_set_pins_PortA_high(MCPA0|MCPA1|MCPA2);
		key=pressed_key(-1,-1);
		mcp23017_set_pins_PortA_high(0);
		ets_delay_us(100);

}


if(key==12){

xTaskNotifyGive(xtaskHandleReset_BKG_Time);
mcp23017_set_pins_PortA_high(MCPA0);
taskYIELD();
continue;

}

time[1]+= key;

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[key][j]&(1<<i))>0){
				timeH2_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeH2_pointers_to_send[i][j]=H2_time_SCH_pointers[i][j];

				}
		}

spi_transmit_isr(spi,true,pointer_to_commands_isr_SCH_timeH2, sizeof(array_of_commands_ISR_SCH_timeH2), true);

for(int i=0; i<(SCHD1TCASETH-SCHD1TCASETL); i++){
spi_transmit_isr(spi,false,(uint8_t*) timeH2_pointers_to_send[i], (SCHTIMERASETH-SCHTIMERASETL)*16, true);
}


}
 
key=-1;

while ((key!=12)&&(key>6) && (key<0)) {

taskENTER_CRITICAL(0);
		key=pressed_key(MCPA0,1);
		mcp23017_set_pins_PortA_high(MCPA0);
		ets_delay_us(100);

  		if (key != -1) {
			continue;} 

		key=pressed_key(MCPA1,1);

		mcp23017_set_pins_PortA_high(MCPA1);
		ets_delay_us(100);

  		if (key != -1){
 			continue; }

		key=pressed_key(MCPA2,1);
		mcp23017_set_pins_PortA_high(MCPA2);
		ets_delay_us(100);

  			if (key != -1){ 
			continue;}

taskEXIT_CRITICAL(0);

		mcp23017_set_pins_PortA_high(MCPA0|MCPA1|MCPA2);
		key=pressed_key(-1,-1);
		mcp23017_set_pins_PortA_high(0);
		ets_delay_us(100);

}


if(key==12){

xTaskNotifyGive(xtaskHandleReset_BKG_Time);
mcp23017_set_pins_PortA_high(MCPA0);
taskYIELD();
continue;
}


time[0]= key*10;

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[key][j]&(1<<i))>0){
				timeM1_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeM1_pointers_to_send[i][j]=M1_time_SCH_pointers[i][j];;

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_SCH_timeM1, sizeof(array_of_commands_ISR_SCH_timeM1), true);

for(int i=0; i<(SCHD1TCASETH-SCHD1TCASETL); i++){
spi_transmit_isr(spi,false,(uint8_t*) timeM1_pointers_to_send[i], (SCHTIMERASETH-SCHTIMERASETL)*16, true);
}

key=-1;

while ((key!=12)&&(key>9) &&(key<0)) {

taskENTER_CRITICAL(0);
		key=pressed_key(MCPA0,1);
		mcp23017_set_pins_PortA_high(MCPA0);
		ets_delay_us(100);

  		if (key != -1) {
			continue;} 

		key=pressed_key(MCPA1,1);

		mcp23017_set_pins_PortA_high(MCPA1);
		ets_delay_us(100);

  		if (key != -1){
 			continue; }

		key=pressed_key(MCPA2,1);
		mcp23017_set_pins_PortA_high(MCPA2);
		ets_delay_us(100);

  			if (key != -1){ 
			continue;}

taskEXIT_CRITICAL(0);

		mcp23017_set_pins_PortA_high(MCPA0|MCPA1|MCPA2);
		key=pressed_key(-1,-1);
		mcp23017_set_pins_PortA_high(0);
		ets_delay_us(100);

}


if(key==12){

xTaskNotifyGive(xtaskHandleReset_BKG_Time);
mcp23017_set_pins_PortA_high(MCPA0);
taskYIELD();
continue;
}

time[0]+= key;

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[key][j]&(1<<i))>0){
				timeM2_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeM2_pointers_to_send[i][j]=M2_time_SCH_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_SCH_timeM2, sizeof(array_of_commands_ISR_SCH_timeM2), true);

for(int i=0; i<(SCHD1TCASETH-SCHD1TCASETL); i++){
spi_transmit_isr(spi,false,(uint8_t*) timeM2_pointers_to_send[i], (SCHTIMERASETH-SCHTIMERASETL)*16, true);
}

ic2_setup_alarm2(time[0], time[1]);

alarm_ON();


vTaskDelay(pdMS_TO_TICKS(500));

spi_transmit_isr(spi,true,pointer_to_commands_isr_BANNERSCH, sizeof(array_of_commands_ISR_BANNERSCH), true);

for (int i=0; i < (STRASETH-STRASETL); i++){
spi_transmit_isr(spi,false, (uint8_t*) scheduler_bkg_pointers[i],(STCASETH-STCASETL)*16 , true);
}


vTaskDelay(pdMS_TO_TICKS(1000));


xTaskNotifyGive(xtaskHandleReset_BKG_Time);
mcp23017_set_pins_PortA_high(MCPA0);
taskYIELD();
continue;

}

else{

mcp23017_set_pins_PortA_high(MCPA0);
taskYIELD();
continue;
}

}

else{


mcp23017_set_pins_PortA_high(MCPA0);
taskYIELD();
continue;
}

}
}


void display_update_RESET_BKG_TIME(void * pvparameters){

for(;;){

ulTaskNotifyTake(pdTRUE,portMAX_DELAY);

spi_transmit_isr(spi,true,pointer_to_commands_isr_BANNERST, sizeof(array_of_commands_ISR_BANNERST), true);


for(int i=0; i<(STRASETH-STRASETL); i++){
spi_transmit_isr(spi,false,(uint8_t*) set_time_pointers[i], (STCASETH-STCASETL)*16, true);
}


}

}


void display_update_SET_TIME(void * pvparameters){

int key;

int h1=-1;

//time[0]=seconds time[1]=minutes time[0]=seconds

uint8_t time[3];

for (;;){

key=pressed_key(MCPA0,-1);
if (key!=11){
continue;

}

spi_transmit_isr(spi,true,pointer_to_commands_isr_BANNERST, sizeof(array_of_commands_ISR_BANNERST), true);


for(int i=0; i<(STRASETH-STRASETL); i++){
spi_transmit_isr(spi,false,(uint8_t*) setup_time_bkg_pointers[i], (STCASETH-STCASETL)*16, true);
}


key=-1;

while ((key!=12)&&(key!=1)&&(key!=2) ){

taskENTER_CRITICAL(0);
		key=pressed_key(MCPA0,1);
		mcp23017_set_pins_PortA_high(MCPA0);
		ets_delay_us(100);

  		if (key != -1) {
			continue;} 

		key=pressed_key(MCPA1,1);

		mcp23017_set_pins_PortA_high(MCPA1);
		ets_delay_us(100);

  		if (key != -1){
 			continue; }

		key=pressed_key(MCPA2,1);
		mcp23017_set_pins_PortA_high(MCPA2);
		ets_delay_us(100);

  			if (key != -1){ 
			continue;}

taskEXIT_CRITICAL(0);

		mcp23017_set_pins_PortA_high(MCPA0|MCPA1|MCPA2);
		key=pressed_key(-1,-1);
		mcp23017_set_pins_PortA_high(0);
		ets_delay_us(100);

}

if(key==1){

key=-1;

while ((key!=12)&&(( key>2)||(key<0 ))){

taskENTER_CRITICAL(0);
		key=pressed_key(MCPA0,1);
		mcp23017_set_pins_PortA_high(MCPA0);
		ets_delay_us(100);

  		if (key != -1) {
			continue;} 

		key=pressed_key(MCPA1,1);

		mcp23017_set_pins_PortA_high(MCPA1);
		ets_delay_us(100);

  		if (key != -1){
 			continue; }

		key=pressed_key(MCPA2,1);
		mcp23017_set_pins_PortA_high(MCPA2);
		ets_delay_us(100);

  			if (key != -1){ 
			continue;}

taskEXIT_CRITICAL(0);

		mcp23017_set_pins_PortA_high(MCPA0|MCPA1|MCPA2);
		key=pressed_key(-1,-1);
		mcp23017_set_pins_PortA_high(0);
		ets_delay_us(100);
}

if((key>0)&(key<3)){

time[2]= key*10;


	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[key][j]&(1<<i))>0){
				timeH1_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeH1_pointers_to_send[i][j]=H1_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_timeH1, sizeof(array_of_commands_ISR_timeH1), true);


for(int i=0; i<(TIMERASETH-TIMERASETL); i++){
spi_transmit_isr(spi,false,(uint8_t*) timeH1_pointers_to_send[i], (H1TCASETH-H1TCASETL)*16, true);
}



h1=key;
}
else if (key==0){

spi_transmit_isr(spi,true,pointer_to_commands_isr_timeH1, sizeof(array_of_commands_ISR_timeH1), true);

for(int i=0; i<(TIMERASETH-TIMERASETL); i++){
spi_transmit_isr(spi,false,(uint8_t*) H1_time_pointers[i], (H1TCASETH-H1TCASETL)*16, true);
}

}

key=-1;

while ((key!=12) && !(h1==2 && key<4) && !((h1<2) && (key>=0)) ){

taskENTER_CRITICAL(0);
		key=pressed_key(MCPA0,1);
		mcp23017_set_pins_PortA_high(MCPA0);
		ets_delay_us(100);

  		if (key != -1) {
			continue;} 

		key=pressed_key(MCPA1,1);

		mcp23017_set_pins_PortA_high(MCPA1);
		ets_delay_us(100);

  		if (key != -1){
 			continue; }

		key=pressed_key(MCPA2,1);
		mcp23017_set_pins_PortA_high(MCPA2);
		ets_delay_us(100);

  			if (key != -1){ 
			continue;}

taskEXIT_CRITICAL(0);

		mcp23017_set_pins_PortA_high(MCPA0|MCPA1|MCPA2);
		key=pressed_key(-1,-1);
		mcp23017_set_pins_PortA_high(0);
		ets_delay_us(100);

}


if(key==12){

xTaskNotifyGive(xtaskHandleReset_BKG_Time);
mcp23017_set_pins_PortA_high(MCPA0);

taskYIELD();
continue;
}

time[2]+= key;

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[key][j]&(1<<i))>0){
				timeH2_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeH2_pointers_to_send[i][j]=H2_time_pointers[i][j];

				}
		}

spi_transmit_isr(spi,true,pointer_to_commands_isr_timeH2, sizeof(array_of_commands_ISR_timeH2), true);

for(int i=0; i<(TIMERASETH-TIMERASETL); i++){
spi_transmit_isr(spi,false,(uint8_t*) timeH2_pointers_to_send[i], (H1TCASETH-H1TCASETL)*16, true);
}

}

key=-1; 

while ((key!=12) && (key>6) && (key<0)) {

taskENTER_CRITICAL(0);
		key=pressed_key(MCPA0,1);
		mcp23017_set_pins_PortA_high(MCPA0);
		ets_delay_us(100);

  		if (key != -1) {
			continue;} 

		key=pressed_key(MCPA1,1);

		mcp23017_set_pins_PortA_high(MCPA1);
		ets_delay_us(100);

  		if (key != -1){
 			continue; }

		key=pressed_key(MCPA2,1);
		mcp23017_set_pins_PortA_high(MCPA2);
		ets_delay_us(100);

  			if (key != -1){ 
			continue;}

taskEXIT_CRITICAL(0);

		mcp23017_set_pins_PortA_high(MCPA0|MCPA1|MCPA2);
		key=pressed_key(-1,-1);
		mcp23017_set_pins_PortA_high(0);
		ets_delay_us(100);

}


if(key==12){

xTaskNotifyGive(xtaskHandleReset_BKG_Time);
mcp23017_set_pins_PortA_high(MCPA0);
taskYIELD();
continue;
}


time[1]= key * 10;

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[key][j]&(1<<i))>0){
				timeM1_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeM1_pointers_to_send[i][j]=M1_time_pointers[i][j];;

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_timeM1, sizeof(array_of_commands_ISR_timeM1), true);

for(int i=0; i<(TIMERASETH-TIMERASETL); i++){
spi_transmit_isr(spi,false,(uint8_t*) timeM1_pointers_to_send[i], (H1TCASETH-H1TCASETL)*16, true);
}

key=-1;

while ((key!=12) && (key>6) && (key<0)) {

taskENTER_CRITICAL(0);
		key=pressed_key(MCPA0,1);
		mcp23017_set_pins_PortA_high(MCPA0);
		ets_delay_us(100);

  		if (key != -1) {
			continue;} 

		key=pressed_key(MCPA1,1);

		mcp23017_set_pins_PortA_high(MCPA1);
		ets_delay_us(100);

  		if (key != -1){
 			continue; }

		key=pressed_key(MCPA2,1);
		mcp23017_set_pins_PortA_high(MCPA2);
		ets_delay_us(100);

  			if (key != -1){ 
			continue;}

taskEXIT_CRITICAL(0);

		mcp23017_set_pins_PortA_high(MCPA0|MCPA1|MCPA2);
		key=pressed_key(-1,-1);
		mcp23017_set_pins_PortA_high(0);
		ets_delay_us(100);

}


if(key==12){

xTaskNotifyGive(xtaskHandleReset_BKG_Time);
mcp23017_set_pins_PortA_high(MCPA0);
taskYIELD();
continue;
}

time[1]+= key;

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[key][j]&(1<<i))>0){
				timeM2_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeM2_pointers_to_send[i][j]=M2_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_timeM2, sizeof(array_of_commands_ISR_timeM2), true);

for(int i=0; i<(TIMERASETH-TIMERASETL); i++){
spi_transmit_isr(spi,false,(uint8_t*) timeM2_pointers_to_send[i], (H1TCASETH-H1TCASETL)*16, true);
}


key=-1;

while ((key!=12) && (key>6) && (key<0)) {

taskENTER_CRITICAL(0);
		key=pressed_key(MCPA0,1);
		mcp23017_set_pins_PortA_high(MCPA0);
		ets_delay_us(100);

  		if (key != -1) {
			continue;} 

		key=pressed_key(MCPA1,1);

		mcp23017_set_pins_PortA_high(MCPA1);
		ets_delay_us(100);

  		if (key != -1){
 			continue; }

		key=pressed_key(MCPA2,1);
		mcp23017_set_pins_PortA_high(MCPA2);
		ets_delay_us(100);

  			if (key != -1){ 
			continue;}

taskEXIT_CRITICAL(0);

		mcp23017_set_pins_PortA_high(MCPA0|MCPA1|MCPA2);
		key=pressed_key(-1,-1);
		mcp23017_set_pins_PortA_high(0);
		ets_delay_us(100);

}

if(key==12){

xTaskNotifyGive(xtaskHandleReset_BKG_Time);
mcp23017_set_pins_PortA_high(MCPA0);
taskYIELD();
continue;
}

time[0]= key*10;


	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[key][j]&(1<<i))>0){
				timeS1_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeS1_pointers_to_send[i][j]=S1_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_timeS1, sizeof(array_of_commands_ISR_timeS1), true);

for(int i=0; i<(TIMERASETH-TIMERASETL); i++){
spi_transmit_isr(spi,false,(uint8_t*) timeS1_pointers_to_send[i], (H1TCASETH-H1TCASETL)*16, true);
}


key=-1;

while ((key!=12) && (key>6) && (key<0)) {

taskENTER_CRITICAL(0);
		key=pressed_key(MCPA0,1);
		mcp23017_set_pins_PortA_high(MCPA0);
		ets_delay_us(100);

  		if (key != -1) {
			continue;} 

		key=pressed_key(MCPA1,1);

		mcp23017_set_pins_PortA_high(MCPA1);
		ets_delay_us(100);

  		if (key != -1){
 			continue; }

		key=pressed_key(MCPA2,1);
		mcp23017_set_pins_PortA_high(MCPA2);
		ets_delay_us(100);

  			if (key != -1){ 
			continue;}

taskEXIT_CRITICAL(0);

		mcp23017_set_pins_PortA_high(MCPA0|MCPA1|MCPA2);
		key=pressed_key(-1,-1);
		mcp23017_set_pins_PortA_high(0);
		ets_delay_us(100);

}


if(key==12){

xTaskNotifyGive(xtaskHandleReset_BKG_Time);
mcp23017_set_pins_PortA_high(MCPA0);
taskYIELD();
continue;
}

else if(key>0 || key==0){

time[0]+= key;

	for (int j=0;j<8;j++){
		
		for(int i=0;i<8;i++){
			if((font_bits[key][j]&(1<<i))>0){
				timeS2_pointers_to_send[i][j]=TIMECOLOR;
				}
			else{
				timeS2_pointers_to_send[i][j]=S2_time_pointers[i][j];

				}
		}
	}

spi_transmit_isr(spi,true,pointer_to_commands_isr_timeS2, sizeof(array_of_commands_ISR_timeS2), true);

for(int i=0; i<(TIMERASETH-TIMERASETL); i++){
spi_transmit_isr(spi,false,(uint8_t*) timeS2_pointers_to_send[i], (H1TCASETH-H1TCASETL)*16, true);
}

}

ic2_setup_time(time[0], time[1], time[2]);

}
else if(key==2){

display_update_SET_SCHEDULER_TIME();

}
else{
mcp23017_set_pins_PortA_high(MCPA0);
taskYIELD();
continue;
}
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

static void devices_scheduler(void *pvparameters){

for(;;){

// CHECK EACH MINUTE
vTaskDelay(pdTICKS_TO_MS(60000));

// THE SCHEDULER FLAG IS 1 AND MUST READ VOLTAGE AND CHECK THEN IF FLOW TURN OFF DEVICE 3
if((*ptrflagAlarm1) & !(flag_photoresistor)){

//TURN OFF CHARGER TO MEASURE BATTERY
//suspend task pwm because gonna turn on again generators
vTaskSuspend(dc_pwm_control_task);

for (int i=0;i<3;i++){

mcpwm_generator_set_force_level(generators_DC_control[i][0] , 1, true);

	}
// WAIT DISCHARGE OF CAPACITOR PRIORITY IS HIGH SO WILL GET UNBLOCK AFTER TIME PASS
vTaskDelay(pdTICKS_TO_MS(500));

if(*adc_dc_voltage_pointers[3]<BATLOWCHARGE){

mcpwm_generator_set_force_level(generators_DC_control[2][0] , 1, true);
mcpwm_generator_set_force_level(generators_DC_control[2][1] , 0, true);


flag_device3out=1;

mcpwm_generator_set_force_level(generators_DC_control[0][0] , -1, true);

vTaskResume(dc_pwm_control_task);

alarm_reset();

	}

else{

flag_device3out=0;

mcpwm_generator_set_force_level(generators_DC_control[0][0] , -1, true);
mcpwm_generator_set_force_level(generators_DC_control[2][0] , -1, true);


vTaskResume(dc_pwm_control_task);

alarm_reset();

}

}
else if(*ptrflagAlarm1)
{
//TURN OFF CHARGER TO MEASURE BATTERY
//suspend task pwm because gonna turn on again generators
vTaskSuspend(dc_pwm_control_task);

mcpwm_generator_set_force_level(generators_DC_control[0][0] , 1, true);
mcpwm_generator_set_force_level(generators_DC_control[2][0] , 1, true);

// WAIT DISCHARGE OF CAPACITOR PRIORITY IS HIGH SO WILL GET UNBLOCK AFTER TIME PASS
vTaskDelay(pdTICKS_TO_MS(500));

if(*adc_dc_voltage_pointers[3]<BATLOWCHARGE){

mcpwm_generator_set_force_level(generators_DC_control[2][0] , 1, true);
mcpwm_generator_set_force_level(generators_DC_control[2][1] , 0, true);


flag_device3out=1;

mcpwm_generator_set_force_level(generators_DC_control[0][0] , -1, true);
mcpwm_generator_set_force_level(generators_DC_control[1][0] , -1, true);


vTaskResume(dc_pwm_control_task);

alarm_reset();

	}

else{

flag_device3out=0;

mcpwm_generator_set_force_level(generators_DC_control[0][0] , -1, true);
mcpwm_generator_set_force_level(generators_DC_control[1][0] , -1, true);
mcpwm_generator_set_force_level(generators_DC_control[2][0] , -1, true);


vTaskResume(dc_pwm_control_task);

alarm_reset();



}



}else if(!(flag_photoresistor)){

mcpwm_generator_set_force_level(generators_DC_control[1][0] , 1, true);

}

if(*ptrflagAlarm2){


mcpwm_generator_set_force_level(generators_DC_control[2][0] , -1, true);
mcpwm_generator_set_force_level(generators_DC_control[2][1] , -1, true);


flag_device3out=0;

alarm_reset();

	}

if((flag_photoresistor)){

mcpwm_generator_set_force_level(generators_DC_control[1][0] , -1, true);

}




}

}
