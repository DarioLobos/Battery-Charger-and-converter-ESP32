/*
 * display_functions.c
 *
 *  Created on: Jan 19, 2026
 *      Author: dario Lobos
 */

#include "main.h"
#include "display_commands.h"
#include "display_commands.c"
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

TaskHandle_t xtaskHandleDisplay= NULL;
TaskHandle_t xtaskHandleFrame = NULL;


static void display_init(void *pvparameter){

spi_device_handle_t spi= pvparameter;

display_allocation();

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