//  **********
//
//  MCU_default
//
//  Provides initial startup and basic operation capabilities for the MCU bryk
//
//  **********

#include "Wire.h"
#include "Arduino.h"
#include "i2c_defs.h"

//  what capability are we building?
//#define MCU_MOTOR
#define MCU_BLUE
//#define MCU_LED
//#define DEBUG

#ifdef MCU_BLUE
//#define LED_BLUE  //  subdefine
#define CAR_BLUE  //  subdefine
#endif


#define PWM1 9
#define PWM2 10

//  operational vars
long timeStart = 0;
byte devScanErr = 0;
byte byte_read = 0;
byte num_bytes_read = 0;
int read_buffer[8];  //  resize as appropriate for our application
byte stop_byte = 0xFF;
byte drv_forward = 0;
byte drv_reverse = 0;
byte drv_speed_val;

int doOnce = 0;

//  ********************
//  setup
//  
void setup()
{

  Serial.begin(115200);
  
  Wire.begin();

  //  configure pins here...
#ifdef MCU_MOTOR  
  //  motor driver
  pinMode(PWM1, OUTPUT);
  pinMode(PWM2, OUTPUT);
  //  appropriate the ananlog pins for direction bit usage...
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  //  for drive testing w/MD08a, use the I2C pins as digital outs...
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
#elif defined (MCU_BLUE) && defined (LED_BLUE)
  //  have to setup led/i2c stuff here as well...
    //  check for RGB LED bryk
  delay(10);  //  wait for startup
  
  //  print life message
  Serial.println("Hello!!!!");
  
  Wire.beginTransmission(ADDR_TI59116_0);
  devScanErr = Wire.endTransmission();
  
  if ( devScanErr == 0)  //  found the device
  {
    Wire.beginTransmission(ADDR_TI59116_0);

    //  start configuring 
    Wire.write(byte(AUTO_INCREMENT_ALL_REG));  //  tells device to increment register for consectutive writes
    //  mode 1
    Wire.write(byte(0));  //  all sub-address responses and OCSCILLATOR set to off
    //  mode 2
    Wire.write(byte(0));  //  set dimming on, enable error status flag
    //  LED brightness, only have 8 LEDs but for ease of auto-increment configure all 16
    for (unsigned i = 0x0; i < 16; i++)
      Wire.write(byte(0x0));
    //  group duty cycle
    Wire.write(byte(0xff));
    //  group freq (blinking)
    Wire.write(byte(0xff));
    //  LED output state, set to off
    for (unsigned j = 0; j < 4; j++)
      Wire.write(byte(0x0));
    //  remainder need not be initialized at startup
    
    Wire.endTransmission();
  }
  //  bluetooth comm
  //pinMode(PWM1, INPUT);
  //pinMode(PWM2, INPUT);
  //  <<<<<<<<<<<<<<<<<<<<<
  //  RGB LED Setup Section
  //  >>>>>>>>>>>>>>>>>>>>>
#elif defined (MCU_LED)
  //  check for RGB LED bryk
  delay(10);  //  wait for startup
  
  Wire.beginTransmission(ADDR_TI59116_0);
  devScanErr = Wire.endTransmission();
  
  if ( devScanErr == 0)  //  found the device
  {
    Wire.beginTransmission(ADDR_TI59116_0);

    //  start configuring 
    Wire.write(byte(AUTO_INCREMENT_ALL_REG));  //  tells device to increment register for consectutive writes
    //  mode 1
    Wire.write(byte(0));  //  all sub-address responses and OCSCILLATOR set to off
    //  mode 2
    Wire.write(byte(0));  //  set dimming on, enable error status flag
    //  LED brightness, only have 8 LEDs but for ease of auto-increment configure all 16
    for (unsigned i = 0x0; i < 16; i++)
      Wire.write(byte(0x0));
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
//    post();
  }
#endif  
  
  //  grab the current time
  timeStart = millis();
}


//  ********************
//  loop
//  
void loop()
{
  
 #ifdef MCU_LED  
 //  institute a polling system here; the list of bryx should probably live in a user-loadable file...
  if ( (millis() - timeStart) >= 1000 )  // 1 sec poll
  {
    for (int i = 0; i < NUM_BRYX_I2C; i++)
    {
      //  look for new devices here
      Wire.beginTransmission(bryx_dev_list[i].address);
      devScanErr = Wire.endTransmission();
      
      if (devScanErr == 0);
      {
        bryx_dev_list[i].isConnected = 1;
        
        Serial.print("Device found at address: ");
        Serial.println(bryx_dev_list[i].address);
      }
    }
    
    //  set timeStart to current time
    timeStart = millis();
  }
  
  //  color check
  //post();
  
  //  *****
  //  cycle leds...
  blinkLEDs();

#elif defined (MCU_MOTOR)  
  //  *****
  //  test motor drive PWM...
  testDrive();

#elif defined (MCU_BLUE)
  //  *****
  //  check bluetooth module for data...
  readBlue();

#endif  
  
}


//  ********************
//  post
//
void post()
{
  unsigned inc = 0;
  long LEDOUT = 0;
  
  //  no status leds on MCU, so for now we have to use the RGB LED bryk...
  Wire.beginTransmission(ADDR_TI59116_0);
  
  //  turn on each of our leds
  Wire.write(byte(AUTO_INCREMENT_ALL_REG + 0x14));  //  auto-increment and start at reg LEDOUT0

  Serial.print("LEDOUT: ");
  Serial.println(LEDOUT, BIN);
  
  for ( unsigned i = 0; i < 16; i++)
  {
    //  check all leds for connectivity...
    // LEDOUT = 0x5500514;//LEDOUT | (long(0x1) << (i * 2));
    
    //  check to see if LED N is configured in our system
    if ((BRYX_LEDS_EN >> inc) & 0x01)
    {
      Serial.print("LED: ");
      Serial.print(i);
      Serial.print(" enabled.\n");
//      Serial.print("Shift test: ");
//      Serial.println((long(0x1) << (inc * 2)),BIN);
      LEDOUT = LEDOUT | (long(0x1) << (inc * 2));  //  turn each connected LED on immediately
      Serial.print("LEDOUT: ");
      Serial.println(LEDOUT, BIN);
    }
    
    inc++;
  }
  
//  LEDOUT = 89130260;
//  Serial.print("Test LEDOUT as BIN: ");
//  Serial.println(LEDOUT, BIN);
//  Serial.print("Size of LEDOUT: ");
//  Serial.print(sizeof(LEDOUT));
//  Serial.print("\n");
  Serial.print("LEDOUT Final: ");
  Serial.print(LEDOUT, BIN);
  Serial.print("\n");
  
  //  now that the LEDOUT status words are configured, send to the device
  for (unsigned j = 0; j < 4; j++)
  {
    Wire.write(byte(LEDOUT & 0xFF));
    LEDOUT >>= 8;
  }  
  
  Wire.endTransmission();
 
}


//  ********************
//  blinkLEDs
//
void blinkLEDs()
{
  //  pick a function here
  
  //  copCarLED()
  
//  for (int i = 0; i < NUM_LEDS; i++)
//    fadeLEDs(ledConfigNums[i].number);
  
  setLEDColor(1, 200);
}


//  ********************
//  readBlue
//
void readBlue()
{
  byte header_num_bytes;
  int led_num, brt_val;
  static int car_cont;
  
#ifdef LED_BLUE 
  //  TEST
  delay(100);
  setLEDColor(10, 200);
  
  //  grab bluetooth data, going through the entire serial buffer until empty...
  if (Serial.available() > 0)
  {
    
      setLEDColor(10, 0);

    //  header, number of bytes to expect in this message
//    header_num_bytes = Serial.parseInt();

    
    led_num = Serial.parseInt();
    brt_val = Serial.parseInt();

#ifdef DEBUG
    Serial.print("Set LED num: ");
    Serial.print(led_num);
    Serial.print("to bright value: ");
    Serial.println(brt_val);
#endif

//    for (int j = 0; j < header_num_bytes; j++)  //  store the byte
//    {
//      read_buffer[num_bytes_read] = Serial.parseInt();
//      num_bytes_read++;
//    }
  }
    //  for LED control via bluetooth, will really only have two bytes of interest, assume byte 1 is 
    //  the numerical led value of interest, and byte 2 is the brightness value...
//    for (int i = 0; i < num_bytes_read; i += 2)
//    {
      setLEDColor(byte(led_num), byte(brt_val));
//    }
    
//    num_bytes_read = 0;
#elif defined (CAR_BLUE)
  if (Serial.available() > 0)
  {
    car_cont = Serial.parseInt();
    
    if ((car_cont - 100) < 0)  // 000 - 099 == no turn, just speed command 
    {
      if (car_cont > 9)
      {
        drv_forward = 1;  //  second digit = 1 = forward
        drv_reverse = 0;
        drv_speed_val = constrain((car_cont - 10), 0, 10);
      }
      else  //  must be reverse
      {
        drv_forward = 0;
        drv_reverse = 1;
        drv_speed_val = constrain(car_cont, 0, 10);
      }
    }
    else if ((car_cont - 200) < 0)  //  100 - 199 == left turn
    {
      if ((car_cont - 100) > 9)
      {
        drv_forward = 1;  //  second digit = 1 = forward
        drv_reverse = 0;
        drv_speed_val = constrain((car_cont - 110), 0, 10);
      }
      else  //  must be reverse
      {
        drv_forward = 0;
        drv_reverse = 1;
        drv_speed_val = constrain((car_cont - 100), 0, 10);
      } 
    }
    else if ((car_cont - 300) < 0)  //  200 - 299 == right turn
    {
      if ((car_cont - 200) > 9)
      {
        drv_forward = 1;  //  second digit = 1 = forward
        drv_reverse = 0;
        drv_speed_val = constrain((car_cont - 210), 0, 10);
      }
      else  //  must be reverse
      {
        drv_forward = 0;
        drv_reverse = 1;
        drv_speed_val = constrain((car_cont - 200), 0, 10);
      } 
    }
    
  }


#endif
  
}


//  ********************
//  testDrive
//
void testDrive()
{
  //----------
  //  steering
  //  full lock, sense???
  //  MD80 specific
  digitalWrite(A0, LOW);
  digitalWrite(A1, HIGH);
  delay(100);
  analogWrite(PWM1, 200);
  delay(100);
  
  //-------
  //  drive
  //  again, sense w/MD80???
  digitalWrite(A4, LOW);
  digitalWrite(A5, HIGH);  
  delay(100);
  analogWrite(PWM2, 150);
  delay(500);

  //  turn
  digitalWrite(A0, HIGH);
  digitalWrite(A1, LOW);
  delay(100);
  analogWrite(PWM1, 200);

  delay(1000);

}


//  ********************
//  setLEDVal
//
void setLEDVal(byte led_num, byte val)
{
  unsigned reg_on_off = 0;
  byte     on_off_val = 0;
  unsigned reg_brightness = 0;
  byte     brightness_val = 0;
  
  //  determine register vals of the requested LED w/correct LED output state settings...
  switch (led_num)
  {
    case 1:
      reg_on_off = 0x14;
      on_off_val = 0x8;
      reg_brightness = 0x3;
      break;
    case 2:
      reg_on_off = 0x14;
      on_off_val = 0x20;
      reg_brightness = 0x4;
      break;
    case 4:
      reg_on_off = 0x15;
      on_off_val = 0x2;
      reg_brightness = 0x6;
      break;
    case 5:
      reg_on_off = 0x15;
      on_off_val = 0x8;
      reg_brightness = 0x7;
      break;
    case 10:
      reg_on_off = 0x16;
      on_off_val = 0x20;
      reg_brightness = 0xC;
      break;
    case 11:
      reg_on_off = 0x16;
      on_off_val = 0x80;
      reg_brightness = 0xD;
      break;
    case 12:
      reg_on_off = 0x17;
      on_off_val = 0x2;
      reg_brightness = 0xE;
      break;
    case 13:
      reg_on_off = 0x17;
      on_off_val = 0x8;
      reg_brightness = 0xF;
      break;
    default:
      //  do nothing unless we have serial access, then print error    
      byte err_out;
  }
  
  //  start comm w/LED driver (once for every register/value pair write)...
  Wire.beginTransmission(ADDR_TI59116_0);
  
  //  first, set the brightness of the requested LED...
  Wire.write(byte(NO_AUTO_INCREMENT + reg_brightness));
  Wire.write(brightness_val);
  Wire.endTransmission();
  
  //  start comm w/LED driver...
  Wire.beginTransmission(ADDR_TI59116_0);

  //  then turn on requested LED...
  Wire.write(byte(NO_AUTO_INCREMENT + reg_on_off));
  Wire.write(on_off_val);  //  start at reg of LED to turn on
  Wire.endTransmission();

}


//  ********************
//  fadeLEDs
//  NOTE:  this func does not check for led_num validity
//
void fadeLEDs(byte led_num)
{
  unsigned inc = 0;
  long     LEDOUT = 0;
  unsigned reg_on_off = 0;
  byte     on_off_val = 0;
  unsigned reg_brightness = 0;
  byte     brightness_val = 0;


  Wire.beginTransmission(ADDR_TI59116_0);
  
  //  ensure all leds are off before starting
  Wire.write(byte(AUTO_INCREMENT_ALL_REG + 0x14));  //  auto-increment and start at reg LEDOUT0
  
  for ( unsigned i = 0; i < 16; i++)
  {
    //  check to see if LED N is configured in our system
    if ((BRYX_LEDS_EN >> inc) & 0x01)
    {
      LEDOUT = LEDOUT | (long(0x00) << (inc * 2));  //  turn each connected LED off immediately
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
  inc = 0;
  LEDOUT = 0;
  
  //  wait a bit...
  delay(100);

  //  ON
  //  turn requested led on and fade its brightness from 0->255->0
  //  determine register vals of the requested LED w/correct LED output state settings...
  switch (led_num)
  {
    case 1:
      reg_on_off = 0x14;
      on_off_val = 0x8;
      reg_brightness = 0x3;
      break;
    case 2:
      reg_on_off = 0x14;
      on_off_val = 0x20;
      reg_brightness = 0x4;
      break;
    case 4:
      reg_on_off = 0x15;
      on_off_val = 0x2;
      reg_brightness = 0x6;
      break;
    case 5:
      reg_on_off = 0x15;
      on_off_val = 0x8;
      reg_brightness = 0x7;
      break;
    case 10:
      reg_on_off = 0x16;
      on_off_val = 0x20;
      reg_brightness = 0xC;
      break;
    case 11:
      reg_on_off = 0x16;
      on_off_val = 0x80;
      reg_brightness = 0xD;
      break;
    case 12:
      reg_on_off = 0x17;
      on_off_val = 0x2;
      reg_brightness = 0xE;
      break;
    case 13:
      reg_on_off = 0x17;
      on_off_val = 0x8;
      reg_brightness = 0xF;
      break;
    default:
      //  do nothing unless we have serial access, then print error    
      byte err_out;
  }
  
  //  first, set the brightness of the requested LED to 0...
  Wire.beginTransmission(ADDR_TI59116_0);
  Wire.write(byte(NO_AUTO_INCREMENT + reg_brightness));
  Wire.write(byte(0));
  Wire.endTransmission();

  //  then turn on requested LED...
  Wire.beginTransmission(ADDR_TI59116_0);
  Wire.write(byte(NO_AUTO_INCREMENT + reg_on_off));
  Wire.write(on_off_val);  //  start at reg of LED to turn on
  Wire.endTransmission();

  //  begin fade up loop
  for (byte i = 0; i < 255; i++)
  {
    //  start comm w/LED driver (once for every register/value pair write)...
    Wire.beginTransmission(ADDR_TI59116_0);
    
    //  set the brightness of the requested LED...
    Wire.write(byte(NO_AUTO_INCREMENT + reg_brightness));
    Wire.write(i);
    Wire.endTransmission();
    
    //  delay a bit?  not sure how fast this will loop...
    delay(5);
  }

  //  begin fade down loop
  for (byte i = 255; i > 0; i--)
  {
    //  start comm w/LED driver (once for every register/value pair write)...
    Wire.beginTransmission(ADDR_TI59116_0);
    
    //  set the brightness of the requested LED...
    Wire.write(byte(NO_AUTO_INCREMENT + reg_brightness));
    Wire.write(i);
    Wire.endTransmission();
    
    //  delay a bit?  not sure how fast this will loop...
    delay(5);
  }
  
}


//  ******************
//  setLEDColor
//
void setLEDColor(byte led_num, byte brt_val)
{
  //  all brightnesses should default to 0 in setup...
//  setLEDBrightVal(10, 0); setLEDBrightVal(13, 0); setLEDBrightVal(12, 0); setLEDBrightVal(11, 0);
//  setLEDBrightVal(5, 0);  setLEDBrightVal(4, 0);  setLEDBrightVal(1, 0);  setLEDBrightVal(2, 0);

  //  check which leds are on
  for (int l = 0; l < NUM_LEDS; l++)
  {
    if (ledConfigNums[l].on == byte(0))
    {
      turnOnLED(ledConfigNums[l].number);
    }
  }
  
  //  then set the appropriate led's brightness...
  setLEDBrightVal(led_num, brt_val);

  //  brightness is the color mix method
  //  try looping thru color wheel
//  for (int i = 0; i < 255; i += 10)
//  {
//    for (int j = 0; j < 255; j += 10)
//    {
//      for (int k = 0; k < 255; k += 10)
//      {
//        //   WHITE                    RED                       GREEN                    BLUE
//        setLEDBrightVal(10, 0); setLEDBrightVal(13, i); setLEDBrightVal(12, j); setLEDBrightVal(11, k);
//        setLEDBrightVal(5, 0);  setLEDBrightVal(4, i);  setLEDBrightVal(1, j);  setLEDBrightVal(2, k);
//      }
//    }
//  }
}


//  ******************
//  turnOnLED
//
void turnOnLED(byte led_num)
{
  
  unsigned     reg_on_off = 0;
  static byte  on_off_val_grp1 = 0;
  static byte  on_off_val_grp2 = 0;
  static byte  on_off_val_grp3 = 0;
  static byte  on_off_val_grp4 = 0;

  switch (led_num)
  {
    case 1:
      reg_on_off = 0x14;
      on_off_val_grp1 += 0x8;
      Wire.beginTransmission(ADDR_TI59116_0);
      Wire.write(byte(NO_AUTO_INCREMENT + reg_on_off));
      Wire.write(on_off_val_grp1);  //  start at reg of LED to turn on
      Wire.endTransmission();
      break;
    case 2:
      reg_on_off = 0x14;
      on_off_val_grp1 += 0x20;
      Wire.beginTransmission(ADDR_TI59116_0);
      Wire.write(byte(NO_AUTO_INCREMENT + reg_on_off));
      Wire.write(on_off_val_grp1);  //  start at reg of LED to turn on
      Wire.endTransmission();
      break;
    case 4:
      reg_on_off = 0x15;
      on_off_val_grp2 += 0x2;
      Wire.beginTransmission(ADDR_TI59116_0);
      Wire.write(byte(NO_AUTO_INCREMENT + reg_on_off));
      Wire.write(on_off_val_grp2);  //  start at reg of LED to turn on
      Wire.endTransmission();
      break;
    case 5:
      reg_on_off = 0x15;
      on_off_val_grp2 += 0x8;
      Wire.beginTransmission(ADDR_TI59116_0);
      Wire.write(byte(NO_AUTO_INCREMENT + reg_on_off));
      Wire.write(on_off_val_grp2);  //  start at reg of LED to turn on
      Wire.endTransmission();
      break;
    case 10:
      reg_on_off = 0x16;
      on_off_val_grp3 += 0x20;
      Wire.beginTransmission(ADDR_TI59116_0);
      Wire.write(byte(NO_AUTO_INCREMENT + reg_on_off));
      Wire.write(on_off_val_grp3);  //  start at reg of LED to turn on
      Wire.endTransmission();
      break;
    case 11:
      reg_on_off = 0x16;
      on_off_val_grp3 += 0x80;
      Wire.beginTransmission(ADDR_TI59116_0);
      Wire.write(byte(NO_AUTO_INCREMENT + reg_on_off));
      Wire.write(on_off_val_grp3);  //  start at reg of LED to turn on
      Wire.endTransmission();
      break;
    case 12:
      reg_on_off = 0x17;
      on_off_val_grp4 += 0x2;
      Wire.beginTransmission(ADDR_TI59116_0);
      Wire.write(byte(NO_AUTO_INCREMENT + reg_on_off));
      Wire.write(on_off_val_grp4);  //  start at reg of LED to turn on
      Wire.endTransmission();
      break;
    case 13:
      reg_on_off = 0x17;
      on_off_val_grp4 += 0x8;
      Wire.beginTransmission(ADDR_TI59116_0);
      Wire.write(byte(NO_AUTO_INCREMENT + reg_on_off));
      Wire.write(on_off_val_grp4);  //  start at reg of LED to turn on
      Wire.endTransmission();
      break;
    default:
      //  do nothing unless we have serial access, then print error    
      byte err_out;
  }
  
  
  for (int i = 0; i < NUM_LEDS; i++)
  {
    if (ledConfigNums[i].number == led_num)
      ledConfigNums[i].on = 1;
  }

}


//  ******************
//  setLEDBrightVal
//
void setLEDBrightVal(byte led_num, byte brt_val)
{
  unsigned reg_brightness = 0;

  switch (led_num)
  {
    case 1:
      reg_brightness = 0x3;
      break;
    case 2:
      reg_brightness = 0x4;
      break;
    case 4:
      reg_brightness = 0x6;
      break;
    case 5:
      reg_brightness = 0x7;
      break;
    case 10:
      reg_brightness = 0xC;
      break;
    case 11:
      reg_brightness = 0xD;
      break;
    case 12:
      reg_brightness = 0xE;
      break;
    case 13:
      reg_brightness = 0xF;
      break;
    default:
      //  do nothing unless we have serial access, then print error    
      byte err_out;
  }
  
  Wire.beginTransmission(ADDR_TI59116_0);
  Wire.write(byte(NO_AUTO_INCREMENT + reg_brightness));
  Wire.write(brt_val);
  Wire.endTransmission();

}
