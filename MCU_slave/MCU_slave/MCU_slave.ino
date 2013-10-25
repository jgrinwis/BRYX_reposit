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
byte read_buffer[8];  //  resize as appropriate for our application
byte stop_byte = 0xFF;
byte idx = 0;
byte numEchoBytes = 0;

int doOnce = 0;

//  temp checkout vars
byte responseData = 0xAD;

//  ********************
//  setup
//  
void setup()
{

//  Serial.begin(19200);
  Serial.begin(115200);
//  Serial.begin(9600);
  Serial.setTimeout(50);
  
  //  join the I2C bus as a slave...
  Wire.begin(bryx_dev_list[1].address);
  
  //  set the request handler...
  Wire.onRequest(reqResponder);
  
  //  set the reception handler...
  Wire.onReceive(recptResponder);
  
  //  configure pins here...
  //  motor driver
//  pinMode(PWM1, OUTPUT);
//  pinMode(PWM2, OUTPUT);
//  //  appropriate the ananlog pins for direction bit usage...
//  pinMode(A0, OUTPUT);
//  pinMode(A1, OUTPUT);
//  //  for drive testing w/MD08a, use the I2C pins as digital outs...
//  pinMode(A4, OUTPUT);
//  pinMode(A5, OUTPUT);
//    
//  digitalWrite(A0, HIGH);
 
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

//    if ( (millis() - timePrev) >= 1000 )  // 1 sec poll
//    {
//      //  set timeStart to current time
//      timePrev = millis();
//      //Serial.println("Heartbeat");
//      
//      for (int i = 0; i < 127; i++)
//      {
//        Wire.beginTransmission(i);
//        devScanErr = Wire.endTransmission();
//        
//        if (devScanErr == 0)
//        {
//          if (i == ADDR_TI59116_0)
//            have_leds = true;
//          
//          Serial.print("Device found at address: ");
//          Serial.println(i);
//        }
//      }
//      
//    }

}


void reqResponder()
{
//  Serial.print("Received data request, sending byte: ");
//  Serial.println(responseData);
  for (int i = 0; i < numEchoBytes; i++)
    Wire.write(read_buffer[i]);
}


void recptResponder(int numBytes)
{
  idx = 0;
  
  for (int i = 0; i < numBytes; i++)
  {
    read_buffer[i] = Wire.read();
    idx++;
  }
  
  numEchoBytes = idx;
}

