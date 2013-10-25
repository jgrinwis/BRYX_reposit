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
long timeNow = 0;
long timePrev = 0;
byte devScanErr = 0;
byte byte_read = 0;
byte num_bytes_read = 0;
int read_buffer[8];  //  resize as appropriate for our application
byte stop_byte = 0xFF;
byte drv_forward = 0;
byte drv_reverse = 0;
byte drv_speed_val;
enum TURN_DIRECTION {STRAIGHT, LEFT, RIGHT};
enum DRIVE_DIRECTION {FORWARD, REVERSE};
boolean have_leds = false;
unsigned btle_comms_counter = 0;
byte localRand = 0;

int doOnce = 0;

//  ********************
//  setup
//  
void setup()
{

//  Serial.begin(19200);
  Serial.begin(115200);
//  Serial.begin(9600);
  Serial.setTimeout(50);
  Wire.begin();
  
  Wire.beginTransmission(ADDR_TI59116_0);
  devScanErr = Wire.endTransmission();

  if ( devScanErr == 0)  //  found the device
  {
    have_leds = true;
    pinMode(A0, OUTPUT);
    digitalWrite(A0, HIGH);
    
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
  else  //  no leds, must be motor/sound transducer
  {
    have_leds = false;
    
    //  configure pins here...
    //  motor driver
    pinMode(PWM1, OUTPUT);
    pinMode(PWM2, OUTPUT);
    //  appropriate the ananlog pins for direction bit usage...
    pinMode(A0, OUTPUT);
    pinMode(A1, OUTPUT);
    //  for drive testing w/MD08a, use the I2C pins as digital outs...
    pinMode(A4, OUTPUT);
    pinMode(A5, OUTPUT);
    
    digitalWrite(A0, HIGH);
  }
 
  //  have to setup led/i2c stuff here as well...
    //  check for RGB LED bryk
  delay(10);  //  wait for startup
  
  //  print life message
  Serial.println("Hello!!!!");
  
  //  grab the current time
  timeStart = timePrev = millis();
}


//  ********************
//  loop
//  
void loop()
{
  
  if (readBlue())
  {
    //  reset btle comms counter
    btle_comms_counter = 0;
  }
  else if (have_leds)
  {
    btle_comms_counter++;
    
    if (btle_comms_counter >= 5000)  //  go into standby if no btle comm after 50 cycles
    {
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
      }
      
      //  color check
      //post();
      
      //  *****
      //  cycle leds...
      delay(50);

//  TEMP TEST
//playNotes();
smellsLikeSchoolSpirit();
//playScale();


      //  disable status lighting for demo...
//      blinkLEDs();

      timeNow = millis();
      
      #ifdef DEBUG
      if ((timeNow - timePrev) >= 1000)
      {
        Serial.println(".");
        timePrev = timeNow;
      }
      #endif
    }
  }
//  else  //  poll for i2c devices
//  TEST - always poll
  {
      if ( (millis() - timePrev) >= 1000 )  // 1 sec poll
      {
        //  set timeStart to current time
        timePrev = millis();
        Serial.println("Heartbeat");
        
//        for (int i = 0; i < NUM_BRYX_I2C; i++)
        for (int i = 0; i < 127; i++)
        {
          Wire.beginTransmission(i);
          devScanErr = Wire.endTransmission();
          
          if (devScanErr == 0)
          {
            if (i == ADDR_TI59116_0)
              have_leds = true;
            
            Serial.print("Device found at address: ");
            Serial.println(i);
            
            //  loop-back testing
            //  write data to found device...
            Wire.beginTransmission(i);
            Wire.write(localRand = random(0,255));
            Serial.print("Value written to device: ");
            Serial.println(localRand);
            Wire.endTransmission();
            
            //  request some data for checkout...
            Wire.requestFrom(i, 1);
            while (Wire.available() == 0)
            {
              //  noop, wait till there's something to read.
              //  obviously if the device is found but non-responsive to request, this loop will
              //  infinite itself, so consider this TEST code only.
            }
             
            Serial.print("Data response from the device: ");
            Serial.println(Wire.read());
          }

        }

//          //  look for new devices here
//          Wire.beginTransmission(bryx_dev_list[i].address);
//          devScanErr = Wire.endTransmission();
//          
//          if (devScanErr == 0);
//          {
//            bryx_dev_list[i].isConnected = 1;
//            if (i == ADDR_TI59116_0)
//              have_leds = true;
//            
//            Serial.print("Device found at address: ");
//            Serial.println(bryx_dev_list[i].address);
//          }
        
      }  
    
  //  playNotes();
  }
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
  static unsigned led_counter = 0;
  static boolean reset_counter       = false;
  
  #ifdef DEBUG
//  Serial.print("led_counter: ");
//  Serial.println(led_counter);
//  Serial.print("reset_counter: ");
//  Serial.println(int(reset_counter));
  #endif
  
  if (reset_counter)
  {
    setLEDColor(ledConfigNums[--led_counter].number, 0);
    
    if (led_counter == 0)
      reset_counter = false;
  }
  else
  {
    setLEDColor(ledConfigNums[led_counter++].number, 25);
    
    if (led_counter >= 8)
      reset_counter = true;
  }
  
  
  //  pick a function here
  
  //  copCarLED()
  
//  for (int i = 0; i < NUM_LEDS; i++)
//    fadeLEDs(ledConfigNums[i].number);

//  setLEDColor(1, 200);
}


//  ********************
//  readBlue
//
boolean readBlue()
{
  byte header_num_bytes;
  int led_num, brt_val;
  static int car_cont;
  DRIVE_DIRECTION drv_direction;
  TURN_DIRECTION trn_direction;
  static int red_val, green_val, blue_val;
  
  drv_direction = FORWARD;
  trn_direction = STRAIGHT;
  
  //  TEST
//  delay(100);
//  setLEDColor(10, 200);
  
  if (have_leds)  //  control the leds
  {
    //  grab bluetooth data, going through the entire serial buffer until empty...
    if (Serial.available())
    {
//      delay(50);  // wait for serial to flush

      //  turn on individual leds...
//      led_num = Serial.parseInt();
//      brt_val = Serial.parseInt();

//      setLEDColor(byte(led_num), byte(brt_val));

      //  turn on by color...
     red_val    = Serial.parseInt();
     green_val  = Serial.parseInt();
     blue_val   = Serial.parseInt();
   
     //  red
     setLEDColor(4,red_val);
     setLEDColor(13,red_val);
     //  green
     setLEDColor(1,green_val);
     setLEDColor(12,green_val);
     //  blue
     setLEDColor(2,blue_val);
     setLEDColor(11,blue_val);

  #ifdef DEBUG
      delay(50);
      Serial.print("Set LED num: ");
      Serial.print(led_num, DEC);
      Serial.print(" to bright value: ");
      Serial.println(brt_val, DEC);
      delay(50);
  #endif
  
      //  try flushing serial buffer
      Serial.read();      
      
//      delay(50);  // wait for serial to flush
      return(true);
    }
    else
      return(false);
  }
  else  //  must be motor control
  {
    //    num_bytes_read = 0;
    if (Serial.available() > 0)
    {
      car_cont = Serial.parseInt();
      
      if ( 0 <= car_cont < 100 )  // 000 - 099 == no turn, just speed command 
      {
        if (car_cont > 9)
        {
          drv_direction = FORWARD;  //  second digit = 1 = forward
          trn_direction = STRAIGHT;
          drv_speed_val = constrain((car_cont - 10), 0, 10);
        }
        else  //  must be reverse
        {
          drv_direction = REVERSE;
          trn_direction = STRAIGHT;
          drv_speed_val = constrain(car_cont, 0, 10);
        }
      }
      else if ( 100 <= car_cont < 200 )  //  100 - 199 == left turn
      {
        if (110 <= car_cont)
        {
          drv_direction = FORWARD;  //  second digit = 1 = forward
          trn_direction = LEFT;
          drv_speed_val = constrain((car_cont - 110), 0, 10);
        }
        else  //  must be reverse
        {
          drv_direction = REVERSE;
          trn_direction = LEFT;
          drv_speed_val = constrain((car_cont - 100), 0, 10);
        } 
      }
      else if ( 200 <= car_cont < 300 )  //  200 - 299 == right turn
      {
        if (210 <= car_cont)
        {
          drv_direction = FORWARD;  //  second digit = 1 = forward
          trn_direction = RIGHT;
          drv_speed_val = constrain((car_cont - 210), 0, 10);
        }
        else  //  must be reverse
        {
          drv_direction = REVERSE;
          trn_direction = RIGHT;
          drv_speed_val = constrain((car_cont - 200), 0, 10);
        } 
      }
      
      testDrive(drv_direction, trn_direction, drv_speed_val);
      return(true);
    }
    else
      return(false);
    
  }
  
}


//  ********************
//  testDrive
//
void testDrive(byte drv_direction, byte trn_direction, byte speed_val)
{
  //----------
  //  steering
  //  full lock, sense???
  //  MD80 specific
  if (trn_direction == STRAIGHT)
  {
    digitalWrite(A0, LOW);
    digitalWrite(A1, LOW);
  }
  else if (trn_direction == LEFT)
  {
    digitalWrite(A0, HIGH);
    digitalWrite(A1, LOW);
  }
  else if (trn_direction == RIGHT)
  {
    digitalWrite(A0, LOW);
    digitalWrite(A1, HIGH);
  }

//  delay(100);
  analogWrite(PWM1, 200);
  delay(100);
  
  //-------
  //  drive
  //  again, sense w/MD80???
  if (drv_direction == FORWARD)
  {
    digitalWrite(A4, HIGH);
    digitalWrite(A5, LOW);  
  }
  else if (drv_direction == REVERSE)
  {
    digitalWrite(A4, LOW);
    digitalWrite(A5, HIGH);  
  }

//  delay(100);
  analogWrite(PWM2, speed_val * 10);  //  speed vals from BTLE are going to be 0-9

  //  turn
//  digitalWrite(A0, HIGH);
//  digitalWrite(A1, LOW);
//  delay(100);
//  analogWrite(PWM1, 200);
//
//  delay(1000);

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


void darkLEDs()
{
  for (int i = 0; i < NUM_LEDS; i++)
    setLEDColor(ledConfigNums[i].number, 0);

}


//  ******************
//  playNotes
//
void playNotes()
{
    static byte buzzerPin = 9;
  
//    //  FUR ELISE
//    //
//    noTone(9);
//    delay(500);
//      setLEDColor(1, 255);
//    tone(9,659);
//    delay(125);
//    darkLEDs();
//      setLEDColor(2, 255);
//    tone(9,622);
//    delay(125);
//    darkLEDs();
//      setLEDColor(1, 255);
//    tone(9,659);
//    delay(125);
//    darkLEDs();
//      setLEDColor(2, 255);
//    tone(9,622);
//    delay(125);
//    darkLEDs();
//      setLEDColor(1, 255);
//    tone(9,659);
//    delay(125);
//    darkLEDs();
//      setLEDColor(10, 255);
//    tone(9,494);
//    delay(125);
//    darkLEDs();
//      setLEDColor(13, 255);
//    tone(9,587);
//    delay(125);
//    darkLEDs();
//      setLEDColor(11, 255);
//    tone(9,523);
//    delay(125);
//    darkLEDs();
//     setLEDColor(12, 255);
//    tone(9,440);
//    delay(500);
//
//    darkLEDs();

    
//  RANDOM
//
    tone(9, random(200, 1250));
    setLEDColor(random(10,13),255);
    delay(random(1,75));
    darkLEDs();

//    //  WARNING SIREN
//    //
//    for (int i = 800; i < 1000; i++)
//    {
//      tone(9, i);
//      delay(5);
//    }
//    delay(500);
   
//COP CAR

//    for (int i = 900; i < 1000; i++)
//    {
//      setLEDColor(2, 255);
//      tone(9, i);
//      delay(5);
//    }
//    
//    darkLEDs();
//    
//    for (int j = 750; j > 650; j--)
//    {
//      setLEDColor(13, 255);
//      tone(9, j);
//      delay(5);
//    }
//
//    darkLEDs();

////DRILL
////
//    tone(buzzerPin,2200); // then buzz by going high
//    tone(buzzerPin,1000);
//    tone(buzzerPin,500);
//    tone(buzzerPin,200);
//    tone(buzzerPin,500);
//    delayMicroseconds(10000);    // waiting
//    noTone(buzzerPin);  // going low
//    delayMicroseconds(10000);    // and waiting more
//    tone(buzzerPin,2200); 
//    tone(buzzerPin,1000);
//    delayMicroseconds(10000);    // waiting
//    noTone(buzzerPin);  // going low
//    delayMicroseconds(10000);    // and waiting more
//    tone(buzzerPin,100); 
//    delayMicroseconds(10000);    // waiting
//    noTone(buzzerPin);  // going low
//    delayMicroseconds(10000);    // and waiting more
//    tone(buzzerPin,100); 
//    delayMicroseconds(10000);    // waiting
//    noTone(buzzerPin);  // going low
//    delayMicroseconds(10000);    // and waiting more
//    noTone(9);
  
  
//  play CETK welcome...
//
//  tone(9, NOTECHART::G4);  //  Middle G (orange)
//  setLEDColor(4, 255);
//  setLEDColor(13, 255);
//  setLEDColor(1, 150);  
//  setLEDColor(12, 150);
//  delay(750);
//  darkLEDs();
//  tone(9, NOTECHART::A4);  //  Middle A (yellow)
//  setLEDColor(4, 255);
//  setLEDColor(13, 255);
//  setLEDColor(1, 255);
//  setLEDColor(12, 255);
//  delay(750);
//  darkLEDs();
//  tone(9, NOTECHART::F4);  //  Middle F (red)
//  setLEDColor(4, 255);
//  setLEDColor(13, 255);
//  delay(750);
//  darkLEDs();
//  tone(9, NOTECHART::F3);  //  F, down an octave (green/chartreuse)
//  setLEDColor(4, 150);
//  setLEDColor(13, 150);
//  setLEDColor(1, 255);
//  setLEDColor(12, 255);
//  delay(750);
//  darkLEDs();
//  tone(9, NOTECHART::C4);  //  Middle C (white)
//  setLEDColor(4, 255);
//  setLEDColor(13, 255);
//  setLEDColor(1, 255);
//  setLEDColor(12, 255);
//  setLEDColor(2, 255);
//  setLEDColor(11, 255);
//  delay(750);
//
//  noTone(9);  
//  darkLEDs();
//  delay(500);  
}


void smellsLikeSchoolSpirit()
{
  setLEDColor(1,255);
  setLEDColor(4,255);
  setLEDColor(11,255);
  tone(9, NOTECHART::C5);  //  hail
  delay(1000);
//  darkLEDs();
  setLEDColor(1,100);
  setLEDColor(4,100);
  setLEDColor(11,100);
  tone(9, NOTECHART::A4);  //  to
  delay(500);
  tone(9, NOTECHART::B4);  //  the
  delay(500);
  tone(9, NOTECHART::C5);  //  vic-
  delay(500);
  tone(9, NOTECHART::A4);  //  tors
  delay(500);
  tone(9, NOTECHART::B4);  //  val-
  delay(500);
  tone(9, NOTECHART::C5);  //  iant
  delay(500);
  tone(9, NOTECHART::D5);  //  hail
  setLEDColor(1,255);
  setLEDColor(4,255);
  setLEDColor(11,255);
  delay(1000);
  setLEDColor(1,100);
  setLEDColor(4,100);
  setLEDColor(11,100);
  tone(9, NOTECHART::B4);  //  to
  delay(500);
  tone(9, NOTECHART::C5);  //  the
  delay(500);
  tone(9, NOTECHART::D5);  //  con-
  delay(500);
  tone(9, NOTECHART::B4);  //  q'ring
  delay(500);
  tone(9, NOTECHART::C5);  //  he-
  delay(500);
  tone(9, NOTECHART::D5);  //  roes
  delay(500);
  tone(9, NOTECHART::E5);  //  hail
  setLEDColor(1,255);
  setLEDColor(4,255);
  setLEDColor(11,255);
  delay(500);
  setLEDColor(1,100);
  setLEDColor(4,100);
  setLEDColor(11,100);
  delay(500);
  tone(9, NOTECHART::F5);  //  hail
  setLEDColor(1,255);
  setLEDColor(4,255);
  setLEDColor(11,255);
  delay(750);
  setLEDColor(1,100);
  setLEDColor(4,100);
  setLEDColor(11,100);
  tone(9, NOTECHART::C5);  //  to
  delay(105);
  tone(9, NOTECHART::C5);  //  mich-
  delay(500);
  tone(9, NOTECHART::D5);  //  i-
  delay(500);
  tone(9, NOTECHART::A4);  //  gan
  delay(500);
  tone(9, NOTECHART::B4);  //  the
  delay(500);
  tone(9, NOTECHART::C5);  //  lead-
  delay(1000);
  tone(9, NOTECHART::B4);  //  ers
  delay(500);
  tone(9, NOTECHART::A4);  //  and
  delay(500);
  tone(9, NOTECHART::E5);  //  best
  delay(1125);
  
  noTone(9);
  
  delay(1000);

  darkLEDs();
}


void playScale()
{
  tone(9, NOTECHART::E4);  //  Middle G (orange)
  delay(1000);
  tone(9, NOTECHART::F4);  //  Middle G (orange)
  delay(1000);
  tone(9, NOTECHART::G4);  //  Middle G (orange)
  delay(1000);
  tone(9, NOTECHART::A4);  //  Middle G (orange)
  delay(1000);
  tone(9, NOTECHART::B4);  //  Middle G (orange)
  delay(1000);
  tone(9, NOTECHART::C4);  //  Middle G (orange)
  delay(1000);
  tone(9, NOTECHART::D4);  //  Middle G (orange)
  delay(1000);
  tone(9, NOTECHART::E4);  //  Middle G (orange)
  delay(1000);
  tone(9, NOTECHART::F4);  //  Middle G (orange)
  delay(1000);
  noTone(9);
  
  delay (2000);
  
}
