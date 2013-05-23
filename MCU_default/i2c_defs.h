//
//
//  static 12c address and data belong here
//
//

#ifndef I2C_DEFS_H
#define I2C_DEFS_H


//  ******************************************
//  >>>>>  TI LED Driver P/N : TLC59116  <<<<< 
//  ******************************************

//  addresses 
#define TI59116_ALLCALL 0b1101000
#define ADDR_TI59116_0 0b1100000
#define ADDR_TI59116_1 0b1100001
#define ADDR_TI59116_2 0b1100010
#define ADDR_TI59116_3 0b1100011
#define RESET_ADDRESS 0b1101011

//  registers
#define NO_AUTO_INCREMENT 0b00000000
#define AUTO_INCREMENT_ALL_REG 0b10000000
#define AUTO_INCREMENT_BRIGHTNESS 0b10100000
#define AUTO_INCREMENT_CONTROL 0b11000000
#define AUTO_INCREMENT_IDV_GLOB 0b11100000

//  general defines
#define BRYX_LEDS_EN 0x3c36 //  (RGB LED BRYK has 8 leds connected: 1,2,4,5,10,11,12,13)





#endif

