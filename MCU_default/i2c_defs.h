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

//  RGB LED BRYK has 8 leds connected: 
//  1(G2), 2(B2), 4(R2), 5(W2), 10(W1), 11(B1), 12(G1), 13(R1)
#define NUM_LEDS 8
//#define BRYX_LEDS_EN 0x3c36  //  all on
#define BRYX_LEDS_EN 0x2004  //  blue 2 and red 1 (cop car)
#define BRYX_LEDS_BLUE2 0x2000  //  blue 2  (cop car)
#define BRYX_LEDS_RED1 0x0004  //  red 1 (cop car)
//  list of numeric leds in array for looping
struct led_state
{
  byte number;
  byte on;
};

led_state ledConfigNums[NUM_LEDS] = {{1,0},{2,0},{4,0},{5,0},{10,0},{11,0},{12,0},{13,0}};


//  ************************************************
//  >>>>>  All BRYX I2C addresses for polling  <<<<< 
//  ************************************************

#define NUM_BRYX_I2C 1

struct bryx_dev_data
{
  int address;
  bool isConnected;
};

bryx_dev_data bryx_dev_list[NUM_BRYX_I2C] = {{ADDR_TI59116_0, 0}};

#endif

