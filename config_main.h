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
// FRAME SETUP TIME (MENU FRAME AND SUBMENUES)
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

# define D1TCASETL 46
# define D1TCASETH 53
# define D1COLTIME (D1TCASETH-D1TCASETL+1)

# define M1TCASETL 54
# define M1TCASETH 61
# define M1COLTIME (M1TCASETH-M1TCASETL+1)

# define M2TCASETL 62
# define M2TCASETH 69
# define M2COLTIME (M2TCASETH-M2TCASETL+1)

# define D2TCASETL 70
# define D2TCASETH 77
# define D2COLTIME (D1TCASETH-D1TCASETL+1)

# define S1TCASETL 78
# define S1TCASETH 85
# define S1COLTIME (S1TCASETH-S1TCASETL+1)

# define S2TCASETL 86
# define S2TCASETH 93
# define S2COLTIME (S2TCASETH-S2TCASETL+1)


// ------------------------------------
// SMALL FRAMES FOR EACH TIME DIGITE FOR SCHEDULER DEVICE 3
// ------------------------------------

# define SCHTIMERASETL 100 
# define SCHTIMERASETH 107
# define SCHROWTIME (SCHTIMERASETH-SCHTIMERASETL+1)


# define SCHH1TCASETL 45
# define SCHH1TCASETH 52
# define SCHH1COLTIME (H1TCASETH-H1TCASETL+1)


# define SCHH2TCASETL 53
# define SCHH2TCASETH 60
# define SCHH2COLTIME (H2TCASETH-H1TCASETL+1)

# define SCHD1TCASETL 61
# define SCHD1TCASETH 68
# define SCHD1COLTIME (D1TCASETH-D1TCASETL+1)



# define SCHM1TCASETL 69
# define SCHM1TCASETH 76
# define SCHM1COLTIME (M1TCASETH-M1TCASETL+1)

# define SCHM2TCASETL 77
# define SCHM2TCASETH 84
# define SCHM2COLTIME (M2TCASETH-M2TCASETL+1)


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
// MCPWM FOR AC SIGNAL
// ------------------------------------

// TIMER 0  FOR MOSFET AC HIGH,
// TIMER 1 FOR BOOSTER HIGH,
// TIMER 2 FOR MOSFET AC LOW AND
// TIMER 4 FOR  BOOSTER LOW 

#define TIMER0_RESOLUTION_HZ 1000000   
// 1MHz, 1us per tick
#define TIMER0_PERIOD        16667    
// 16,666ms

#define TIMER1_RESOLUTION_HZ 1000000  
// 1MHz, 1us per tick // THIS IS BIG BOOSTER 
#define TIMER1_PERIOD        50     
// 50 ticks, 50us  BOOSTER LOW POWER

#define TIMER2_RESOLUTION_HZ 1000000   
// 1MHz, 1us per tick
#define TIMER2_PERIOD        16667    
// 16,666ms

#define TIMER3_RESOLUTION_HZ 1000000  
// 1MHz,us per tick  // THIS IS SMALL BOOSTER
#define TIMER3_PERIOD        50     
// 50 ticks, 50Us  BOOSTER HIGH POWER

#define DELAYTIMEAC 333


#define COMP_VALUE_MOSFET    8333     
// 8.333 ms 

// THIS IS THE MINIMUN COMPARATOR
#define COMP_BOOSTER_LOW     4     
//  4 us 
#define COMP_BOOSTER_HIGH    36      
//36 us 


#define GEN_GPIO_AC_MOS1_H     14
#define GEN_GPIO_AC_MOS2_H     12
#define GEN_GPIO_BOOS_H  	   13
#define GEN_GPIO_AC_MOS1_L     5
#define GEN_GPIO_AC_MOS2_L     19
#define GEN_GPIO_BOOS_L  	   18

// ------------------------------------
// MCPWM FOR DC BUCK BOOSTER SIGNAL
// ------------------------------------


#define TIMERDC_RESOLUTION_HZ 10000000  
// 10MHz, 0.11us per tick
#define TIMERDC_PERIOD     500    
// 50us

#define COMP_VALUE_DC	   220     
// 22us 

#define DELAYTIMEDC 30
// 3u


// THIS IS THE MINIMUN COMPARATOR [THIS VALUE IS PENDINF DO SIMULATION]

#define COMP_MIN_DC1     40     
//  4 us 

#define COMP_MIN_DC2     40     
//  4 us 

#define COMP_MIN_DC3     40     
//  4 us 


#define GEN_GPIO_DC1P    33
#define GEN_GPIO_DC1N    25

#define GEN_GPIO_DC2P    26
#define GEN_GPIO_DC2N    27

#define GEN_GPIO_DC3P    1
#define GEN_GPIO_DC3N    3



// ------------------------------------
//ADC1 Channels
// ------------------------------------

// 
// GPIO 39
#define ADC1_AC          ADC_CHANNEL_3

// GPIO32
#define ADC1_DC1          ADC_CHANNEL_4

// GPIO33
#define ADC1_DC2          ADC_CHANNEL_0

// GPIO34
#define ADC1_DC3          ADC_CHANNEL_6

// GPIO 35
#define ADC1_BAT          ADC_CHANNEL_7


#define ADC_ATTEN           ADC_ATTEN_DB_12

// buffer for continuous mode

#define ADC_BUFFER_SIZE		128
#define ADC_FRAME_SIZE		64



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

// READING LIMITS FOR DC  RESISTOR BRIDGE R2 10K R1 56K 
// 14V EQUAL TO 2121 mV measured
// 16v EQUAl TO 2420 mV measured
// 9v EQUAL TO 1363 mV measured

#define MAX_DC_VIN 2424 
#define MAX_DC_VOUT 2121 
#define NON_DC_VOUT 2061 
#define MIN_DC_VOUT 1818
#define MIN_DC_VIN 1363

// duty cycles 
#define DC_MIN_D_BOOSTER 2
#define DC_MAX_D_BOOSTER 20

#define DC_MIN_D_BUCK 2
#define DC_MAX_D_BUCK 40

#define DC_MIN_D_BUCK_BOOST 19
#define DC_MAX_D_BUCK_BOOST 23


// THIS DEFINES RATE OF CHANGE ACOORDING DC LOAD AND MUST BE TESTED
#define GRADIENT_DC_HIGH 	300
#define GRADIENT_DC_MID 	200
#define GRADIENT_DC_LOW 	100



	 
// ------------------------------------
//GPIO FOR BOOSTER DC INPUT TO IDENTIFY USED BOOSTER WITH INTERRUPT
// ------------------------------------

#define ESP_INTR_FLAG_DEFAULT 0

#define GPIO_INPUT_BOOSTER 	13

// MASK FOR SELECTION

#define INPUT_BOOSTER_MASK (1ULL<<GPIO_INPUT_BOOSTER)

// THIS DEFINES RATE OF CHANGE ACOORDING AC LOAD AND MUST BE TESTED
#define GRADIENT_BOOST_hIGH 300
#define GRADIENT_BOOST_MID 200
#define GRADIENT_BOOST_LOW 100


// ------------------------------------
//GPIOS FOR KEYPAD 4x3 (what i have)
// ------------------------------------

// THE THIRD COLUMN AND THE THIRD ROW ARE SPECIAL FUNCTIONS THAT ENABLE THE OTHERS,
// SO WILL USE INTERRUOTION TO DONT NEED TO SCAN UNTIL USER CALL MENU.
// BE SURE THAT SPECIAL KEYS ARE ATTACHED TO THE THIRD COLUMN AND ROW
// COLUMNS ARE DEFINED AS OUTPUT AND ROWS AS INPUT

#define GPIO_KEYPADROW0 	17
#define GPIO_KEYPADROW1 	16
#define GPIO_KEYPADROW2 	4
#define GPIO_KEYPADROW3 	2

#define GPIO_KEYPADCOL0 	16
#define GPIO_KEYPADCOL1 	15
#define GPIO_KEYPADCOL2 	0

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
// THIS ADDRESS IS THE STARTING OF BYTE DAY, MONTH, YEAR
#define ADDRDAY	0X03


// THIS ADDRESS IS THE STARTING OF BYTE ALAMM1 MINUTES, HOUR
// ALARM 1 IF SET WILL START DEVICE 3 AT CERTAIN TIME
#define ADDRALARM1	0X08

// THIS ADDRESS IS THE STARTING OF BYTE ALAMM2 MINUTES, HOUR (ALARM 2 DONT HAVE SECONDS)
// ALARM 1 IF SET WILL STOP DEVICE 3 AT CERTAIN TIME
#define ADDRALARM2	0X0B

#define ADDRALARMON	0X0E

#define ADDRALARCHK	0X0F

#define ADDRALARMONBITS	3

#define ADDRALARM1IRQBITS	1
#define ADDRALARM2IRQBITS	2

// ------------------------------------
// BATTERY CHARGE LEVEL FOR TURN OFF THE DEVICE3
// ------------------------------------

// BATTERY IS ABOUT 50% LEAD OR AGM LifEPO4 MUST BE 13000

#define BATLOWCHARGE	12100 

// RESISTOR FOR VOLTAGE DIVIDER FROM 12V TO ADC LEVER (R2/(R1+R2))

#define R1	56
#define R2	10
