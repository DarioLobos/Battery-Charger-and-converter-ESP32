/*
 * mcpwm_bat_charge.c
 *
 *  Created on: Jan 26, 2026
 *      Author: dario
 */



#include "mcpwm_init.c"

static mcpwm_timer_handle_t timers_DC_control[3];

static mcpwm_oper_handle_t operators_DC_control[3];

static mcpwm_cmpr_handle_t comparators_DC_control[3];

static mcpwm_gen_handle_t generators_DC_control[3][3];


static void mosfet_signal_DC(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t compa){


// M3[0]]c          D
// |                |
// |______COil______|
// |                |
// D               M1[1]a



ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(gena,
MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP,
MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(gena,
MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, compa,
MCPWM_GEN_ACTION_LOW)));

mcpwm_dead_time_config_t dead_time_config = {
.posedge_delay_ticks = 0,
.negedge_delay_ticks = 0
};

dead_time_config.flags.invert_output = false;


ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(gena, genb, &dead_time_config));


}

static void timer_setup_DC (void){

	const char *TAG = "error/message:";
    ESP_LOGI(TAG, "Create timers for DC chargers");
	mcpwm_timer_config_t timer_config;
    
    timer_config.clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT;
    timer_config.group_id = 0;
    timer_config.resolution_hz = TIMERDC_RESOLUTION_HZ;
    timer_config.period_ticks = TIMERDC_PERIOD;
    timer_config.count_mode = MCPWM_TIMER_COUNT_MODE_UP;
    
    

   for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timers_DC_control[i]));
    }

    ESP_LOGI(TAG, "Create operators");
    mcpwm_operator_config_t operator_config= {
        .group_id = 1, // operator should be in the same group of the above timers
    };

    for (int i = 0; i < 3; ++i) {
        ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &operators_DC_control[i]));
	    ESP_LOGI(TAG, "Connect timers and operators with each other");
        ESP_ERROR_CHECK(mcpwm_operator_connect_timer(operators_DC_control[i], timers_DC_control[i]));
    }


    ESP_LOGI(TAG, "Create comparators");
    mcpwm_comparator_config_t compare_config = {
        .flags.update_cmp_on_tep = true,
    };

// MOSFET SIGNAL USE ONLY ONE COMPARATORM, THE SECOND AND THIRD ONE, FOLLOWS AND ARE LATCHED BY THE FIRST ONE

    for (int i = 0; i < 3; ++i) {
        ESP_ERROR_CHECK(mcpwm_new_comparator(operators_DC_control[i], &compare_config, &comparators_DC_control[i]));
        // init compare for each comparator
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparators_DC_control[i], COMP_VALUE_DC));
};

// Each operator have 2 generators

    ESP_LOGI(TAG, "Create generators");
    const int gen_gpios[3][2] = {
{GEN_GPIO_DC1P,GEN_GPIO_DC1N},
{GEN_GPIO_DC2P,GEN_GPIO_DC2N},
{GEN_GPIO_DC3P,GEN_GPIO_DC3N}};


    mcpwm_generator_config_t gen_config = {};

    for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++){
        	gen_config.gen_gpio_num = gen_gpios[i][j];
        ESP_ERROR_CHECK(mcpwm_new_generator(operators_DC_control[i], &gen_config, &generators_DC_control[i][j]));
    }
}
    
    for (int i = 0; i < 3; ++i) {
 	mosfet_signal_DC(generators_DC_control[i][0],generators_DC_control[i][1],comparators_DC_control[i] );

}

}
