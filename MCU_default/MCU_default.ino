//  **********
//
//  MCU_default
//
//  Provides initial startup and basic operation capabilities for the MCU bryk
//
//  **********

#include "i2c_defs.h"
#include "Wire.h"


void setup()
{
  Wire.begin();
  

  //  <<<<<<<<<<<<<<<<<<<<<
  //  RGB LED Setup Section
  //  >>>>>>>>>>>>>>>>>>>>>

  //  check for RGB LED bryk
  delay(10);  //  wait for startup
  
  Wire.beginTransmission(ADDR_TI59116_0);
  
  //  start configuring 
  Wire.write(byte(AUTO_INCREMENT_ALL_REG));  //  tells device to increment register for consectutive writes
  //  mode 1
  Wire.write(byte(0));  //  all sub-address responses and OCSCILLATOR set to off
  //  mode 2
  Wire.write(byte(0));  //  set dimming on, enable error status flag
  //  LED brightness, only have 8 LEDs but for ease of auto-increment configure all 16
  for (unsigned i = 0x0; i < 16; i++)
    Wire.write(byte(0xff));
  //  group duty cycle
  Wire.write(byte(0xff));
  //  group freq (blinking)
  Wire.write(byte(0xff));
  //  LED output state, set to off
  for (unsigned j = 0; j < 4; j++)
    Wire.write(byte(0x0));
    
  //  remainder need not be initialized at startup
  
  Wire.endTransmission();

  //  start with power-on self-test...
  post();

}


void loop()
{
  
}


void post()
{
  unsigned inc = 0;
  int LEDOUT = 0;
  
  //  no status leds on MCU, so for now we have to use the RGB LED bryk...
  Wire.beginTransmission(ADDR_TI59116_0);
  
  //  turn on each of our leds
  Wire.write(byte(AUTO_INCREMENT_ALL_REG + 0x14));  //  auto-increment and start at reg LEDOUT0
  
  for ( unsigned i = 0; i < 16; i++)
  {
    //  check to see if LED N is configured in our system
    if ((BRYX_LEDS_EN >> inc) & 0x01)
    {
      LEDOUT = LEDOUT | (0x01 << (inc * 2));  //  turn each connected LED on immediately
    }
    
    inc++;
  }
  
  //  now that the LEDOUT status words are configured, send to the device
  for (unsigned j = 0; j < 4; j++)
  {
    Wire.write(byte(LEDOUT & 0xFF));
    LEDOUT >>= 8;
  }  
  
  
  Wire.endTransmission();
}
