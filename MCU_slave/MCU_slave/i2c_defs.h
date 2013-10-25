//
//
//  static 12c address and data belong here
//
//

#ifndef I2C_DEFS_H
#define I2C_DEFS_H


//  we will live wihtout the Binay consts B0 and B1...
#undef B0
#undef B1

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

#define NUM_BRYX_I2C 2
#define MCU_SLAVE_1_ADDR 0b1100100

struct bryx_dev_data
{
  int address;
  bool isConnected;
};

bryx_dev_data bryx_dev_list[NUM_BRYX_I2C] = {{ADDR_TI59116_0, 0}, {MCU_SLAVE_1_ADDR, 0}};


//  ************************
//  >>>>>  Note Chart  <<<<< 
//  ************************
namespace NOTECHART {  //  scientific note notation, rounded to nearest int (tone() limitation)
  enum NOTES {
    C0  = 16,
    C0S = 17,
    D0F = 17,
    D0  = 18,
    D0S = 19,
    E0F = 19,
    E0  = 21,
    F0  = 22,
    F0S = 23,
    G0F = 23,
    G0  = 25,
    G0S = 26,
    A0F = 26,
    A0  = 28,
    A0S = 29,
    B0F = 29,
    B0  = 31,
    C1  = 33,
    C1S = 35,
    D1F = 35,
    D1  = 36,
    D1S = 39,
    E1F = 39,
    E1  = 41,
    F1  = 44,
    F1S = 46,
    G1F = 46,
    G1  = 49,
    G1S = 52,
    A1F = 52,
    A1  = 55,
    A1S = 58,
    B1F = 58,
    B1  = 62,
    C2  = 65,
    C2S = 69,
    D2F = 69,
    D2  = 73,
    D2S = 78,
    E2F = 78,
    E2  = 82,
    F2  = 87,
    F2S = 93,
    G2F = 93,
    G2  = 98,
    G2S = 104,
    A2F = 104,
    A2  = 110,
    A2S = 117,
    B2F = 117,
    B2  = 123,
    C3  = 131,
    C3S = 139,
    D3F = 139,
    D3  = 147,
    D3S = 156,
    E3F = 156,
    E3  = 165,
    F3  = 175,
    F3S = 185,
    G3F = 185,
    G3  = 196,
    G3S = 208,
    A3F = 208,
    A3  = 220,
    A3S = 233,
    B3F = 233,
    B3  = 247,
    C4  = 262,
    C4S = 277,
    D4F = 277,
    D4  = 294,
    D4S = 311,
    E4F = 311,
    E4  = 330,
    F4  = 349,
    F4S = 370,
    G4F = 370,
    G4  = 392,
    G4S = 415,
    A4F = 415,
    A4  = 440,
    A4S = 466,
    B4F = 466,
    B4  = 494,
    C5  = 523,
    C5S = 554,
    D5F = 554,
    D5  = 587,
    D5S = 622,
    E5F = 633,
    E5  = 659,
    F5  = 698,
    F5S = 740,
    G5F = 740,
    G5  = 784,
    G5S = 831,
    A5F = 831,
    A5  = 880,
    A5S = 932,
    B5F = 932,
    B5  = 989,
    C6  = 1047,
    C6S = 1109,
    D6F = 1109,
    D6  = 1175,
    D6S = 1245,
    E6F = 1245,
    E6  = 1319,
    F6  = 1397,
    F6S = 1480,
    G6F = 1480,
    G6  = 1568,
    G6S = 1661,
    A6F = 1661,
    A6  = 1760,
    A6S = 1865,
    B6F = 1865,
    B6  = 1976,
    C7  = 2093,
    C7S = 2217,
    D7F = 2217,
    D7  = 2349,
    D7S = 2489,
    E7F = 2489,
    E7  = 2637,
    F7  = 2794,
    F7S = 2960,
    G7F = 2960,
    G7  = 3136,
    G7S = 3322,
    A7F = 3322,
    A7  = 3520,
    A7S = 3729,
    B7F = 3729,
    B7  = 3951,
    C8  = 4186,
    C8S = 4435,
    D8F = 4435,
    D8  = 4699,
    D8S = 4978,
    E8F = 4978
  };
};

#endif



