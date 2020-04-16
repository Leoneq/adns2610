/*
mouse_arduino.ino  - ADNS2610 Arduino Library - by Leoneq ;3, April 2020
(insert github here)
Based on code by Conor Peterson (robotrobot@gmail.com): https://conorpeterson.wordpress.com/2010/06/04/optical-mouse-hacking-part-1/
This example is for ADNS2610 - and if you want, just add RGB LED for color image!
Just change your pins, define proper ADNS2610 object, and open processing sketch (it's in the library folder).
If you don't want to get the image, just open serial monitor and type 'c', or 'x'.
For further documentation visit the cpp file.
*/

#include <Arduino.h>
#include "ADNS2610.h"

//I used NodeMCU, change as you wish
#define SDIO D1
#define SCLK D2
#define LEDG D6
#define LEDR D5
#define LEDB D7

#define FRAMELENGTH 324 //don't change. 18x18px frame
byte frame[FRAMELENGTH];
int avgR, avgG, avgB, deltaX, sum = 0;

ADNS2610 adns(SDIO, SCLK, LEDR, LEDG, LEDB);
//ADNS2610 adns(SDIO, SCLK, LEDR); //In case you want to use normal, red LED.

void setup()
{
  adns.begin(true); //'True' will force the mouse to always stay on.
  Serial.begin(115200);
  adns.blinkLED(); //To test your LED, or RGB LED.
  adns.debug(); //Does nothing, but prints your mouse ID.
  adns.backlight(true); //It turns on stock LED, or just red from RGB LED.
}

void loop()
{
  char input;  

  if( Serial.available() )
  {
    input = Serial.read();
    switch( input )
    {
    case 'r':
      adns.readFrame(frame, 'r');
      for(int s = 0; s < (FRAMELENGTH); s++ ) Serial.write((byte) frame[s]); //This loop sends frame to processing. 
      Serial.print((byte)127);
      break;
    case 'g':
      adns.readFrame(frame, 'g');
      for(int s = 0; s < (FRAMELENGTH); s++ ) Serial.write((byte) frame[s]); //Always remember - use write function, to send data (not text).
      Serial.print((byte)127);
      break;
    case 'b':
      adns.readFrame(frame, 'b');
      for(int s = 0; s < (FRAMELENGTH); s++ ) Serial.write((byte) frame[s]); //After frame, we send "127" at the end.
      Serial.print((byte)127);
      break;
    case 'c':
      Serial.print("R: "); Serial.print(adns.getAvgPixel('r'));
      Serial.print("G: "); Serial.print(adns.getAvgPixel('g'));
      Serial.print("B: "); Serial.println(adns.getAvgPixel('b')); //Just like color sensor.
      //Serial.print("I: "); Serial.println(adns.getAvgPixel()); //Use this one, if you have stock LED.
      break;
    case 'x':
      Serial.print("Delta X: "); Serial.println((int8_t)adns.getDeltaX()); //We need to convert byte to int8_t - delta is from -127 to 127, not 0-255.
      Serial.print("Delta Y: "); Serial.println((int8_t)adns.getDeltaY());
      Serial.print("Min pixel: "); Serial.println(adns.getMinPixel());
      Serial.print("Max pixel: "); Serial.println(adns.getMaxPixel());
      break;
    }
    Serial.flush();
  }
  /*
  deltaX = adns.getDeltaX(); //very, very simple gesture control. You need camera lenses for that
  while(deltaX != 0)
  {
    deltaX = adns.getDeltaX(); 
    sum += deltaX; //the output from sensor may not be stable.
    delay(5);
  }
  if(sum > 0) adns.backlight(false); //let's control led with gestures!
  else if(sum < 0) adns.backlight(true); 
  sum = 0;*/
}