/*
 * display_commands.h
 *
 *  Created on: Jan 16, 2026
 *      Author: dario
 * COMMAND LIST FOR ST7735S
 */
#define SWRESET		0X01
#define RDDID		0X04  
#define RDDST 		0X09
#define RDDPM   	0X0A
#define RDDMADCTL 	0X0B
#define RDDCOLMOD	0X0C
#define RDDIM		0X0D
#define RDDSM		0X0E
#define RDDSDR		0X0E
#define SLPIN		0X10
#define SLPOUT		0X11
#define PTLON		0X12
#define NORON		0X13
#define INVOFF 		0X20
#define INVON		0X21
#define GAMSET		0X26
#define DISPOFF		0X28
#define DISPON		0X29
#define CASET		0X2A
#define RASET		0X2B
#define RAMWR		0X2C
#define RGBSET		0X2D
#define RAMRD		0X2E
#define PTLAR		0X30
#define SCRLAR		0X33
#define TEOFF		0X34
#define TEON		0X35
#define MADCTL		0X36
#define VSCSAD		0X37
#define IDMOFF		0X38
#define IDMON		0X39
#define COLMOD		0X3A
#define RDID1		0XDA
#define RDID2		0XDB
#define RDID3		0XDC

/*  NORON = NORMAL MODE ON no parameters
 *  DISPON = DISPLAY ON no parameters
 *  CASET= COLUMN ADDRESS SET parameters define read/write area
 *  RASET= ROW ADDRESS SET parameters define read/write area
 *  RAMWR= MEMORY WRITE
 *  RAMRD= MEMORY READ
 *  MADTCL= MEMORY DATA ACCESS CONTROL (DEFAULT USED)
 *  COLMOD= INTERFACE PIXEL FORMAT 
 */

// THESE DEFINE SOME CHOSEN PARAMETERS

#define PCOLMOD 0X05

// DEFINE PINS

#define LCD_HOST    SPI2_HOST

#define PIN_NUM_MISO 12
#define PIN_NUM_MOSI 13
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   15

#define PIN_NUM_DC   21
#define PIN_NUM_RST  18
#define PIN_NUM_BCKL 5



