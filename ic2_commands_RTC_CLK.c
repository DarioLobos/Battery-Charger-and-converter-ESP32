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
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "portmacro.h"
#include "sdkconfig.h"
#include "config_main.h"
#include <stdint.h>
#include "mcp23017.h"

uint8_t *received_time[3];
uint8_t *received_date[4];

static volatile uint8_t flagAlarm1=0;
volatile uint8_t * ptrflagAlarm1= &flagAlarm1;
static volatile uint8_t flagAlarm2=0;
volatile uint8_t * ptrflagAlarm2= &flagAlarm2;



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
	uint8_t address=0;
    uint8_t *ptraddress=&address;


    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLV_DS3231ADDR << 1) | WRITE_BIT, true);
    i2c_master_write(cmd, ptraddress, 1, true);
    i2c_master_write(cmd, ptrdata, sizeof(data), true);
	i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, portMAX_DELAY);

return ret;

}

static esp_err_t ic2_read_time(){

    *received_time = heap_caps_malloc(sizeof(received_time), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
	uint8_t address=0;
    uint8_t *ptraddress=&address;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLV_DS3231ADDR << 1) | READ_BIT, true);
    i2c_master_write(cmd, ptraddress, 1, true);
    i2c_master_read(cmd, *received_time, sizeof(*received_time), false);
	i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, portMAX_DELAY);

return ret;

}



static esp_err_t ic2_setup_date(uint8_t day_week, uint8_t day, uint8_t month,uint8_t year ){

    uint8_t data[4]={ day_week, day,month,year };
    void *ptrdata=&data;
	uint8_t address=(uint8_t)ADDRDAY;
    uint8_t *ptraddress=&address;
	

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLV_DS3231ADDR << 1) | WRITE_BIT, true);
    i2c_master_write(cmd,ptraddress , 1, true);
    i2c_master_write(cmd, ptrdata, sizeof(data), true);
	i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, portMAX_DELAY);

return ret;

}

static esp_err_t ic2_read_date(){

    *received_date = heap_caps_malloc(sizeof(received_date), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
	uint8_t address=(uint8_t)ADDRDAY;
    uint8_t *ptraddress=&address;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLV_DS3231ADDR << 1) | READ_BIT, true);
    i2c_master_write(cmd, ptraddress, 1, true);
    i2c_master_read(cmd, *received_date, sizeof(*received_time), false);
	i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, portMAX_DELAY);

return ret;

}

// alarm 1 is for start device 3 at certain time if voltage in battery is not enough
static esp_err_t ic2_setup_alarm1( uint8_t minutes, uint8_t hour){

    uint8_t data[2]={ minutes,hour};
    void *ptrdata=&data;
	uint8_t address= (uint8_t) ADDRALARM1;
    uint8_t *ptraddress=&address;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLV_DS3231ADDR << 1) | WRITE_BIT, true);
    i2c_master_write(cmd, ptraddress, 1, true);
    i2c_master_write(cmd, ptrdata, sizeof(data), true);
	i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, portMAX_DELAY);

return ret;

}

// alarm 1 is for stop device 3 at certain time if voltage in battery is not enough
static esp_err_t ic2_setup_alarm2( uint8_t minutes, uint8_t hour){

    uint8_t data[2]={ minutes,hour};
    void *ptrdata=&data;
	uint8_t address= (uint8_t) ADDRALARM2;
    uint8_t *ptraddress=&address;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLV_DS3231ADDR << 1) | WRITE_BIT, true);
    i2c_master_write(cmd, ptraddress, 1, true);
    i2c_master_write(cmd, ptrdata, sizeof(data), true);
	i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, portMAX_DELAY);

return ret;

}

static esp_err_t alarm_ON(){

    uint8_t data=(uint8_t) ADDRALARMONBITS;
    void *ptrdata=&data;
	uint8_t address= (uint8_t) ADDRALARMON;
    uint8_t *ptraddress=&address;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLV_DS3231ADDR << 1) | WRITE_BIT, true);
    i2c_master_write(cmd, ptraddress, 1, true);
    i2c_master_write(cmd, ptrdata, sizeof(data), true);
	i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, portMAX_DELAY);

return ret;

}

static esp_err_t alarm_OFF(){

    uint8_t data=(uint8_t) 0;
    void *ptrdata=&data;
	uint8_t address= (uint8_t) ADDRALARMON;
    uint8_t *ptraddress=&address;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLV_DS3231ADDR << 1) | WRITE_BIT, true);
    i2c_master_write(cmd, ptraddress, 1, true);
    i2c_master_write(cmd, ptrdata, sizeof(data), true);
	i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, portMAX_DELAY);

return ret;

}

static esp_err_t read_alarm2_flag(){

	uint8_t address=ADDRALARCHK;
    uint8_t *ptraddress=&address;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLV_DS3231ADDR << 1) | READ_BIT, true);
    i2c_master_write(cmd, ptraddress, 1, true);
    i2c_master_read(cmd, (uint8_t*) ptrflagAlarm2 
, sizeof(flagAlarm1), false);
	i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, portMAX_DELAY);

*ptrflagAlarm1=(uint8_t) ((*ptrflagAlarm2 & ADDRALARM2IRQBITS)>>1);
return ret;

}

static esp_err_t read_alarm1_flag(){

	uint8_t address=ADDRALARCHK;
    uint8_t *ptraddress=&address;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLV_DS3231ADDR << 1) | READ_BIT, true);
    i2c_master_write(cmd, ptraddress, 1, true);
    i2c_master_read(cmd, (uint8_t*) ptrflagAlarm1 
, sizeof(flagAlarm1), false);
	i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, portMAX_DELAY);

*ptrflagAlarm1=(uint8_t) (*ptrflagAlarm1 & ADDRALARM1IRQBITS);
return ret;

}

static esp_err_t alarm_reset(){

    uint8_t data=(uint8_t) 0 ;
    void *ptrdata=&data;
	uint8_t address= (uint8_t) ADDRALARCHK;
    uint8_t *ptraddress=&address;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLV_DS3231ADDR << 1) | WRITE_BIT, true);
    i2c_master_write(cmd, ptraddress, 1, true);
    i2c_master_write(cmd, ptrdata, sizeof(data), true);
	i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, portMAX_DELAY);

return ret;
 
}


// this change all values of port A one time
static esp_err_t mcp23017_set_pins_PortA_high(uint8_t mask){


    uint8_t data= mask ;
    void *ptrdata=&data;
	uint8_t address= (uint8_t) GPIOA;
    uint8_t *ptraddress=&address;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLV_MCP23017 << 1) | WRITE_BIT, true);
    i2c_master_write(cmd, ptraddress, 1, true);
    i2c_master_write(cmd, ptrdata, sizeof(data), true);
	i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, portMAX_DELAY);

return ret;

}

// this change all values of port B one time
static esp_err_t mcp23017_set_pins_PortB_high(uint8_t mask){


    uint8_t data= mask ;
    void *ptrdata=&data;
	uint8_t address= (uint8_t) GPIOB;
    uint8_t *ptraddress=&address;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLV_MCP23017 << 1) | WRITE_BIT, true);
    i2c_master_write(cmd, ptraddress, 1, true);
    i2c_master_write(cmd, ptrdata, sizeof(data), true);
	i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, portMAX_DELAY);

return ret;

}

static esp_err_t mcp23017_config(){


    uint8_t data=(uint8_t) IOCONCONFIG ;
    void *ptrdata=&data;
	uint8_t address= (uint8_t) IOCON1;
    uint8_t *ptraddress=&address;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLV_MCP23017 << 1) | WRITE_BIT, true);
    i2c_master_write(cmd, ptraddress, 1, true);
    i2c_master_write(cmd, ptrdata, sizeof(data), true);
	i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, portMAX_DELAY);


return ret;

}


// later can be done more functions for MCP23017