/*
 * ic2_commands_RTC_CLK.c
 *
 *  Created on: Jan 22, 2026
 *      Author: dario
 */
// 

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "portmacro.h"
#include "sdkconfig.h"
#include "main.h"
#include <stdint.h>

uint8_t *received_time[3];



static esp_err_t i2c_master_init(void)
{

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = SDL_PIN,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,

        // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
    };

    esp_err_t err = i2c_param_config(I2C_NUM_0, &conf);
    if (err != ESP_OK) {
        return err;
    }

esp_err_t ret=i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);

return ret;

}

static esp_err_t ic2_setup_time(uint8_t seconds, uint8_t minutes, uint8_t hour){

    uint8_t data[3]={ seconds, minutes,hour};
    void *ptrdata=&data;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLV_DS3231ADDR << 1) | WRITE_BIT, true);
    i2c_master_write(cmd, ptrdata, sizeof(data), true);
	i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, portMAX_DELAY);

return ret;

}

static esp_err_t ic2_read_time(){

    *received_time = heap_caps_malloc(COLDC * sizeof(received_time), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLV_DS3231ADDR << 1) | READ_BIT, true);
    i2c_master_read(cmd, *received_time, sizeof(*received_time), false);
	i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, portMAX_DELAY);

return ret;

}


