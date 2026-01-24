/*Configuration and Macros*/

// TASK AND CORES
#define TASK_PRIO_4         4
#define TASK_PRIO_3         3
#define TASK_PRIO_2         2
#define TASK_PRIO_1         1
#define TASK_PRIO_0         0

#define CORE0               0
#define CORE1               1

// ------------------------------------
// Height and width of display
// ------------------------------------

# define ROWARRAY 128
# define COLARRAY 160

// ------------------------------------
// FRAMES THAT CHANGE IN THE DISPLAY
// ------------------------------------

// ------------------------------------
// FRAME SETUP TIME (MENU FRAME)
// ------------------------------------

# define STRASETL 25
# define STRASETH 124
# define STCASETL 25
# define STCASETH 104

# define STROWARRAY (STRASETH-STRASETL+1)
# define STCOLARRAY (STCASETH-STCASETL+1)


// ------------------------------------
// FRAME CLOCK 
// ------------------------------------

# define TIMERASETL 15 
# define TIMERASETH 22
# define TIMECOLOR 0xfdf7
# define ROWTIME (TIMERASETH-TIMERASETL+1)
# define COLTIME (H1TCASETL-S2TCASETH+1)

// ------------------------------------
// SMALL FRAMES FOR EACH TIME DIGITE
// ------------------------------------

# define H1TCASETL 30
# define H1TCASETH 37
# define H1COLTIME (H1TCASETH-H1TCASETL+1)


# define H2TCASETL 38
# define H2TCASETH 45
# define H2COLTIME (H2TCASETH-H1TCASETL+1)

# define M1TCASETL 54
# define M1TCASETH 61
# define M1COLTIME (M1TCASETH-M1TCASETL+1)

# define M2TCASETL 62
# define M2TCASETH 69
# define M2COLTIME (M2TCASETH-M2TCASETL+1)

# define S1TCASETL 79
# define S1TCASETH 86
# define S1COLTIME (S1TCASETH-S1TCASETL+1)

# define S2TCASETL 87
# define S2TCASETH 94
# define S2COLTIME (S2TCASETH-S2TCASETL+1)




// ------------------------------------
// FRAME AC VOLTAGE (110V)
// ------------------------------------

# define ACRASETL 143
# define ACRASETH 150
# define ACCASETL 50
# define ACCASETH 97
# define ACCOLOR 0x17a7
# define ROWAC (ACRASETH-ACRASETL+1)
# define COLAC (ACCASETH-ACCASETL+1)

// ------------------------------------
// FRAME DC VOLTAGE (12v)
// ------------------------------------

# define DCRASETL 133
# define DCRASETH 140
# define DCCASETL 50
# define DCCASETH 89
# define DCCOLOR 0x1dee
# define ROWDC (DCRASETH-DCRASETL+1)
# define COLDC (DCCASETH-DCCASETL+1)

// ------------------------------------
// MCPWM
// ------------------------------------

// TIMER 0 FOR MOSFET TIMER 1 AND 2 FOR BOOSTER

#define TIMER0_RESOLUTION_HZ 1000000   
// 1MHz, 1us per tick
#define TIMER0_PERIOD        16667    
// 16,666ms

// BOOSTER NEEDS 2 TIMERS TO CAN START ONE OR THE OTHER ACCORDING SIGNAL 

#define TIMER1_RESOLUTION_HZ 1000000  
// 1MHz, 1us per tick // THIS IS BIG BOOSTER 
#define TIMER1_PERIOD        50     
// 50 ticks, 50us  BOOSTER LOW POWER
#define TIMER2_RESOLUTION_HZ 1000000  
// 1MHz, 1us per tick  // THIS IS SMALL BOOSTER
#define TIMER2_PERIOD        50     
// 50 ticks, 50Us  BOOSTER HIGH POWER



#define COMP_VALUE_MOSFET    8333     
// 8.333 ms 

// THIS IS THE MINIMUN COMPARATOR
#define COMP_BOOSTER_LOW     4     
//  4 us 
#define COMP_BOOSTER_HIGH    36      
//36 us 


#define GEN_GPIO0           0
#define GEN_GPIO1           2
#define GEN_GPIO2           4
#define GEN_GPIO3	        5

// ------------------------------------
//ADC1 Channels
// ------------------------------------

// 
// GPIO 39
#define ADC1_CHAN3          ADC_CHANNEL_3

// GPIO32
#define ADC1_CHAN4          ADC_CHANNEL_4

// GPIO33
#define ADC1_CHAN5          ADC_CHANNEL_5

// GPIO34
#define ADC1_CHAN6          ADC_CHANNEL_6

// GPIO 35
#define ADC1_CHAN7          ADC_CHANNEL_7


#define ADC_ATTEN           ADC_ATTEN_DB_12

// READING LIMITS IF IS NOT USED AN ESP32 S MUST BE CHANGED UPPER LIMIT SEE USED SPEC

#define VMINAC		1790  
//EQUAL TO 100V

#define VNOMAC		2350
//EQUAL T0 110V

#define VMAXAC		2570 
//EQUA TO 130

// COMPARATOR LIMITS MEASURE IS IN TICK SO DUTY CYCLE WILL DEPEND ON RESOLUTION 

#define MAX_COMP_H	40	 
#define MIN_COMP_H	36

#define MAX_COMP_L	34	 
#define MIN_COMP_L	4	 

	 
// ------------------------------------
//GPIO FOR BOOSTER INPUT TO IDENTIFY USED BOOSTER WITH INTERRUPT
// ------------------------------------

#define ESP_INTR_FLAG_DEFAULT 0

#define GPIO_INPUT_BOOSTER 	27

// MASK FOR SELECTION

#define INPUT_BOOSTER_MASK (1ULL<<GPIO_INPUT_BOOSTER)

// ------------------------------------
//GPIOS FOR KEYPAD 4x3 (what i have)
// ------------------------------------

// THE THIRD COLUMN AND THE THIRD ROW ARE SPECIAL FUNCTIONS THAT ENABLE THE OTHERS,
// SO WILL USE INTERRUOTION TO DONT NEED TO SCAN UNTIL USER CALL MENU.
// BE SURE THAT SPECIAL KEYS ARE ATTACHED TO THE THIRD COLUMN AND ROW
// COLUMNS ARE DEFINED AS OUTPUT AND ROWS AS INPUT

#define GPIO_KEYPADROW0 	19
#define GPIO_KEYPADROW1 	18
#define GPIO_KEYPADROW2 	5
#define GPIO_KEYPADROW3 	17

#define GPIO_KEYPADCOL0 	16
#define GPIO_KEYPADCOL1 	4
#define GPIO_KEYPADCOL2 	2

#define INPUT_ROW_NUMBERS_MASK (1ULL<<GPIO_KEYPADROW0 | 1ULL<<GPIO_KEYPADROW1 | 1ULL<<GPIO_KEYPADROW2 | 1ULL<<GPIO_KEYPADROW3 )

#define OUTPUT_COL_NUMBERS_MASK (1ULL<<GPIO_KEYPADCOL0 | 1ULL<<GPIO_KEYPADCOL1 | 1ULL<<GPIO_KEYPADCOL2)



// ------------------------------------
// IC2 CONFIG FOR CLOCK OR OTHER SENSORS LATER ON
// ------------------------------------

// GPIO 21
#define SDA_PIN   21

// GPIO 22
#define SDL_PIN   22

// frequency is to handle fast mode of RTC Clock DS3231 [CLOCK HAVE BATTERY AND IS INDEPENDENT, CHEAP]
#define I2C_MASTER_FREQ_HZ 400000

// address for DS3231

#define SLV_DS3231ADDR	0X68
 
#define WRITE_BIT I2C_MASTER_WRITE       
#define READ_BIT I2C_MASTER_READ       

// IN DS3231 ADDDRESS OF TIME START IN ZERO, AND ARE ALIGNED ADDRESS OF DATE START 0X03 AND IS ALIGNED TOO
#define ADDRDAY	0X03