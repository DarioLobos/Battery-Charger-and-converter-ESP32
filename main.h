/*Configuration and Macros*/

// TASK AND CORES
#define TASK_PRIO_4         4
#define TASK_PRIO_3         3
#define TASK_PRIO_2         2
#define TASK_PRIO_1         1
#define TASK_PRIO_0         0

#define CORE0               0
#define CORE1               1

// MCPWM
// TIMER 0 FOR BOOSTER TIMER 1 FOR MOSFET
#define TIMER0_RESOLUTION_HZ 1000000  // 1MHz, 1us per tick
#define TIMER0_PERIOD        16667    // 16,666ms
#define TIMER1_RESOLUTION_HZ 1000000  // 1MHz, 1us per tick

// BOOSTER NEEDS 2 TIMERS TO CAN START ONE OR THE OTHER ACCORDING SIGNAL 
#define TIMER1_PERIOD        50     // 50 ticks, 50us  BOOSTER LOW POWER
#define TIMER2_RESOLUTION_HZ 1000000  // 1MHz, 1us per tick
#define TIMER2_PERIOD        50     // 50 ticks, 50Us  BOOSTER HIGH POWER



#define COMP_VALUE_MOSFET    8333     // 8.333 ms 

// THIS IS THE MINIMUN COMPARATOR
#define COMP_BOOSTER_LOW     4     //  4 us 
#define COMP_BOOSTER_HIGH    36     // 36 us 


#define GEN_GPIO0           0
#define GEN_GPIO1           2
#define GEN_GPIO2           4
#define GEN_GPIO3	        5

//ADC1 Channels
#define ADC1_CHAN1          ADC_CHANNEL_1
#define ADC1_CHAN2          ADC_CHANNEL_2
#define ADC1_CHAN3          ADC_CHANNEL_3
#define ADC1_CHAN6          ADC_CHANNEL_6
#define ADC1_CHAN7          ADC_CHANNEL_7



#define ADC_ATTEN           ADC_ATTEN_DB_12


