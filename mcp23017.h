/*
 * mcp23017.h
 *
 *  Created on: Feb 2, 2026
 *      Author: dario
 */

#define IODIRA 		0X00
#define IPOLA 		0X01
#define GPINTENA 	0X02
#define DEFVALA 	0X03
#define INTCONA 	0X04
#define IOCON1	 	0X05
#define GPPUA	 	0X06
#define INTFA	 	0X07
#define INTCAPA	 	0X08
#define GPIOA	 	0X09
#define OLATA	 	0X0A

#define IODIRB 		0X10
#define IPOLB 		0X11
#define GPINTENB 	0X12
#define DEFVALB 	0X13
#define INTCONB 	0X14
#define IOCON2	 	0X15
#define GPPUB	 	0X16
#define INTFB	 	0X17
#define INTCAPB	 	0X18
#define GPIOB	 	0X19
#define OLATB	 	0X1A

// TWO PORT 8 BIT SEPATATED
#define BANK1		(1<<7)

//DISABLED ADDRESS POINTER INCREMENT 
#define SEQOPDIS	(1<<5)

// ENABLE ADDRESS IN IC2 IS REDUNDANT 
#define HAEN		(1<<3)

#define IOCONCONFIG	( BANK1 | SEQOPDIS | HAEN)



