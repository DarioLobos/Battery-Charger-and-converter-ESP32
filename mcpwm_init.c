/*
 * mcpwm_init.c
 *
 *  Created on: Jan 19, 2026
 *      Author: dario
 */

#include "driver/mcpwm_timer.h"
#include "config_main.h"
#include "esp_log.h"
#include "driver/mcpwm_prelude.h"
#include "driver/mcpwm_cmpr.h"
#include "driver/mcpwm_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"


static mcpwm_timer_handle_t timers[3];

static mcpwm_oper_handle_t operatorsBooster[2];

static mcpwm_oper_handle_t operatorsMosfet[2];

static mcpwm_cmpr_handle_t comparatorsBoosters[2];

static mcpwm_cmpr_handle_t comparatorsMosfets;

static mcpwm_gen_handle_t generatorsBoosters[2];

static mcpwm_gen_handle_t generatorsMosfets[2];



static void mosfet_signals(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t compa){

ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(gena,
MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP,
MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(gena,
MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, compa,
MCPWM_GEN_ACTION_LOW)));

mcpwm_dead_time_config_t dead_time_config = {
.posedge_delay_ticks = DELAYTIMEAC,
.negedge_delay_ticks = 0
};

ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(gena, gena, &dead_time_config));

dead_time_config.posedge_delay_ticks = 0;
dead_time_config.negedge_delay_ticks = DELAYTIMEAC;
dead_time_config.flags.invert_output = true;

ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(gena, genb, &dead_time_config));

}

static void timer_mosfet_start(void *pvparameter ){

    mcpwm_timer_handle_t timer=pvparameter;

  
     ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
     ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
     vTaskDelay(pdMS_TO_TICKS(10));
     vTaskDelete(NULL);

}

void booster_selection (void *pvparameter ){

int booster= gpio_get_level(GPIO_INPUT_BOOSTER);

for(;;) {
if (booster==0){
// SIGNAL ZERO MEAN THAT BIG BOOSTER IS USED

     ESP_ERROR_CHECK(mcpwm_timer_enable(timers[0]));
     ESP_ERROR_CHECK(mcpwm_timer_start_stop(timers[0], MCPWM_TIMER_START_NO_STOP));
     ESP_ERROR_CHECK(mcpwm_timer_disable(timers[1]));


}
else {
// SIGNAL 1 MEAN THAT SMALL BOOSTER IS USED
     ESP_ERROR_CHECK(mcpwm_timer_enable(timers[1]));
     ESP_ERROR_CHECK(mcpwm_timer_start_stop(timers[1], MCPWM_TIMER_START_NO_STOP));
     ESP_ERROR_CHECK(mcpwm_timer_disable(timers[0]));


}

ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

} 
}


static void timer_setup_AC (void){

	const char *TAG = "error/message:";
    ESP_LOGI(TAG, "Create timers");
	mcpwm_timer_config_t timer_config[3];
    
    timer_config[0].clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT;
    timer_config[0].group_id = 0;
    timer_config[0].resolution_hz = TIMER0_RESOLUTION_HZ;
    timer_config[0].period_ticks = TIMER0_PERIOD;
    timer_config[0].count_mode = MCPWM_TIMER_COUNT_MODE_UP;
    
    timer_config[1].clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT;
    timer_config[1].group_id = 0;
    timer_config[1].resolution_hz = TIMER1_RESOLUTION_HZ;
    timer_config[1].period_ticks = TIMER1_PERIOD;
    timer_config[1].count_mode = MCPWM_TIMER_COUNT_MODE_UP;

    timer_config[2].clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT;
    timer_config[2].group_id = 0;
    timer_config[2].resolution_hz = TIMER2_RESOLUTION_HZ;
    timer_config[2].period_ticks = TIMER2_PERIOD;
    timer_config[2].count_mode = MCPWM_TIMER_COUNT_MODE_UP;


   for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config[i], &timers[i]));
    }

    ESP_LOGI(TAG, "Create operators");
    mcpwm_operator_config_t operator_config_booster = {
        .group_id = 0, // operator should be in the same group of the above timers
    };
    for (int i = 0; i < 2; ++i) {
        ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config_booster, &operatorsBooster[i]));
    }

    mcpwm_operator_config_t operator_config_mosfet = {
        .group_id = 0, // operator should be in the same group of the above timers
    };
    for (int i = 0; i < 2; ++i) {
        ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config_mosfet, &operatorsMosfet[i]));
    }


    ESP_LOGI(TAG, "Connect timers and operators with each other");
        ESP_ERROR_CHECK(mcpwm_operator_connect_timer(operatorsMosfet[0], timers[0]));
        ESP_ERROR_CHECK(mcpwm_operator_connect_timer(operatorsMosfet[1], timers[0]));
        ESP_ERROR_CHECK(mcpwm_operator_connect_timer(operatorsBooster[0], timers[1]));
        ESP_ERROR_CHECK(mcpwm_operator_connect_timer(operatorsBooster[1], timers[2]));


    ESP_LOGI(TAG, "Create comparators");
    mcpwm_comparator_config_t compare_config_boosters = {
        .flags.update_cmp_on_tep = true,
    };
        ESP_ERROR_CHECK(mcpwm_new_comparator(operatorsBooster[0], &compare_config_boosters, &comparatorsBoosters[0]));
        // init compare for each comparator
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparatorsBoosters[0], COMP_BOOSTER_LOW));

        ESP_ERROR_CHECK(mcpwm_new_comparator(operatorsBooster[1], &compare_config_boosters, &comparatorsBoosters[1]));
        // init compare for each comparator
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparatorsBoosters[1], COMP_BOOSTER_HIGH));



// MOSFET SIGNAL USE ONLY ONE COMPARATORM THE SECOND FOLLOW AND IS LATCHED BY THE FIRST ONE
    mcpwm_comparator_config_t compare_config_mosfets = {
        .flags.update_cmp_on_tep = true,
    };
        ESP_ERROR_CHECK(mcpwm_new_comparator(operatorsMosfet[0], &compare_config_mosfets, &comparatorsMosfets));
        // init compare for each comparator
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparatorsMosfets, COMP_VALUE_MOSFET));

    ESP_LOGI(TAG, "Create generators");
    const int gen_gpios_boosters[2] = {GEN_GPIO0, GEN_GPIO1};
    mcpwm_generator_config_t gen_config_booster = {};
    for (int i = 0; i < 2; i++) {
        gen_config_booster.gen_gpio_num = gen_gpios_boosters[i];
        ESP_ERROR_CHECK(mcpwm_new_generator(operatorsBooster[i], &gen_config_booster, &generatorsBoosters[i]));
    }
    
    const int gen_gpios_mosfets[2] = {GEN_GPIO2, GEN_GPIO3};
    mcpwm_generator_config_t gen_config_mosfet = {};
    for (int i = 0; i < 2; i++) {
        gen_config_mosfet.gen_gpio_num = gen_gpios_mosfets[i];
        ESP_ERROR_CHECK(mcpwm_new_generator(operatorsBooster[i], &gen_config_mosfet, &generatorsBoosters[i]));
    }

	mosfet_signals(generatorsMosfets[0], generatorsMosfets[1], comparatorsMosfets);

}

