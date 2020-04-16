/*
ADNS2610.cpp - mouse chip library by Leoneq, April 2020
(insert github here)
Based on code by Conor Peterson (robotrobot@gmail.com): https://conorpeterson.wordpress.com/2010/06/04/optical-mouse-hacking-part-1/
*/
#include "Arduino.h"
#include "ADNS2610.h"

/*
 * This creates object of the mouse. You need to define data pin,
 * clock pin, and custom LED pin.
 */
ADNS2610::ADNS2610(int dataPin, int clockPin, int ledPin)
{	
    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    rLedPin_ = ledPin;
    gLedPin_ = ledPin;
    bLedPin_ = ledPin;
    ledPin_ = ledPin;
    dataPin_ = dataPin;
    clockPin_ = clockPin;
}

/*
 * This creates object of the mouse, but with RGB LED.
 * You need to define data pin, clock pin, and custom LEDs pin.
 */
ADNS2610::ADNS2610(int dataPin, int clockPin, int rLedPin, int gLedPin, int bLedPin)
{	
    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(rLedPin, OUTPUT);
    pinMode(gLedPin, OUTPUT);
    pinMode(bLedPin, OUTPUT);
    digitalWrite(rLedPin, LOW);
    digitalWrite(gLedPin, LOW);
    digitalWrite(bLedPin, LOW);
    rLedPin_ = rLedPin;
    gLedPin_ = gLedPin;
    bLedPin_ = bLedPin;
    dataPin_ = dataPin;
    clockPin_ = clockPin;
}

/*
 * This function awakens the mouse. If the argyment is true,
 * the mouse will be forced to always stay awake.
 */
void ADNS2610::begin(boolean mode)
{
    digitalWrite(clockPin_, HIGH);
    delayMicroseconds(5);
    digitalWrite(clockPin_, LOW);
    delayMicroseconds(1);
    digitalWrite(clockPin_, HIGH);
    delay(1025);
    if(mode) writeRegister(regConfig_, maskNoSleep_);
}

void ADNS2610::writeRegister(byte addr, byte data)
{
  byte i;

  addr |= 0x80; //Setting MSB high indicates a write operation.

  //Write the address
  pinMode (dataPin_, OUTPUT);
  for (i = 8; i != 0; i--)
  {
    digitalWrite (clockPin_, LOW);
    digitalWrite (dataPin_, addr & (1 << (i-1) ));
    digitalWrite (clockPin_, HIGH);
  }

  //Write the data    
  for (i = 8; i != 0; i--)
  {
    digitalWrite (clockPin_, LOW);
    digitalWrite (dataPin_, data & (1 << (i-1) ));
    digitalWrite (clockPin_, HIGH);
  }
}

/*
 * This function prints some info about the mouse via Serial.
 */
void ADNS2610::debug()
{
  unsigned int val;

  val = readRegister(regStatus_);

  Serial.print("Mouse ID: ");
  Serial.println((unsigned int)((val & maskPID_) >> 5));
  Serial.println("Ready.");
  Serial.flush();
}

/*
 * This function just blink your LED(s).
 */
void ADNS2610::blinkLED()
{
  if(ledPin_ != 0) 
  {
    digitalWrite(ledPin_, HIGH);
    delay(120);
    digitalWrite(ledPin_, LOW);
    delay(80);
  } else 
  {
    digitalWrite(rLedPin_, HIGH);
    delay(120);
    digitalWrite(rLedPin_, LOW);
    delay(80);
    digitalWrite(gLedPin_, HIGH);
    delay(120);
    digitalWrite(gLedPin_, LOW);
    delay(80);
    digitalWrite(bLedPin_, HIGH);
    delay(120);
    digitalWrite(bLedPin_, LOW);
    delay(80);
  }
}

/*
 * This function fills frame from first argument, with color defined in second argument.
 * The second argument should be 'r', 'g', or 'b'.
 */
void ADNS2610::readFrame(byte* arr, char color)
{
  switch(color)
  {
    case 'r':
      digitalWrite(rLedPin_, HIGH);
      digitalWrite(gLedPin_, LOW);
      digitalWrite(bLedPin_, LOW);
      break;
    case 'g':
      digitalWrite(rLedPin_, LOW);
      digitalWrite(gLedPin_, HIGH);
      digitalWrite(bLedPin_, LOW);
      break;
    case 'b':
      digitalWrite(rLedPin_, LOW);
      digitalWrite(gLedPin_, LOW);
      digitalWrite(bLedPin_, HIGH);
      break;
  }
  delay(100);
  byte *pos;
  byte *uBound;
  unsigned long timeout;
  byte val;

  //Ask for a frame dump
  writeRegister(regPixelData_, 0x2A);

  val = 0;
  pos = arr;
  uBound = arr + 325;

  timeout = millis() + 1000;

  //There are three terminating conditions from the following loop:
  //1. Receive the start-of-field indicator after reading in some data (Success!)
  //2. Pos overflows the upper bound of the array (Bad! Might happen if we miss the start-of-field marker for some reason.)
  //3. The loop runs for more than one second (Really bad! We're not talking to the chip properly.)
  while( millis() < timeout && pos < uBound)
  {
    val = readRegister(regPixelData_);
    //Only bother with the next bit if the pixel data is valid.
    if( !(val & 64) ) continue;

    //If we encounter a start-of-field indicator, and the cursor isn't at the first pixel,
    //then stop. ('Cause the last pixel was the end of the frame.)
    if((val & 128) &&  (pos != arr)) break;

    *pos = val & 63;
    pos++;
  }

  digitalWrite(rLedPin_, LOW);
  digitalWrite(gLedPin_, LOW);
  digitalWrite(bLedPin_, LOW);
}

/*
 * This function fills frame from the first argument.
 */
void ADNS2610::readFrame(byte* arr)
{
  digitalWrite(ledPin_, HIGH);
  delay(100);
  byte *pos;
  byte *uBound;
  unsigned long timeout;
  byte val;

  //Ask for a frame dump
  writeRegister(regPixelData_, 0x2A);

  val = 0;
  pos = arr;
  uBound = arr + 325;

  timeout = millis() + 1000;

  //There are three terminating conditions from the following loop:
  //1. Receive the start-of-field indicator after reading in some data (Success!)
  //2. Pos overflows the upper bound of the array (Bad! Might happen if we miss the start-of-field marker for some reason.)
  //3. The loop runs for more than one second (Really bad! We're not talking to the chip properly.)
  while( millis() < timeout && pos < uBound)
  {
    val = readRegister(regPixelData_);
    //Only bother with the next bit if the pixel data is valid.
    if( !(val & 64) ) continue;

    //If we encounter a start-of-field indicator, and the cursor isn't at the first pixel,
    //then stop. ('Cause the last pixel was the end of the frame.)
    if((val & 128) &&  (pos != arr)) break;

    *pos = val & 63;
    pos++;
  }

  digitalWrite(ledPin_, LOW);
}

byte ADNS2610::readRegister(byte addr)
{
  byte i;
  byte r = 0;

  //Write the address
  pinMode (dataPin_, OUTPUT);
  for (i = 8; i != 0; i--)
  {
    digitalWrite (clockPin_, LOW);
    digitalWrite (dataPin_, addr & (1 << (i-1) ));
    digitalWrite (clockPin_, HIGH);
  }

  pinMode (dataPin_, INPUT);  //Switch the dataline from output to input
  delayMicroseconds(110);  //Wait (per the datasheet, the chip needs a minimum of 100 µsec to prepare the data)

  //Clock the data back in
  for (i = 8; i != 0; i--)
  {                             
    digitalWrite (clockPin_, LOW);
    digitalWrite (clockPin_, HIGH);
    r |= (digitalRead (dataPin_) << (i-1) );
  }

  delayMicroseconds(110);  //Tailing delay guarantees >100 µsec before next transaction

  return r;
}

/*
 * This variable stores the relative delta X. Always
 * remember, to use it as int8_t.
 */
int8_t ADNS2610::getDeltaX()
{
  char x = readRegister(regDeltaX_);
  return x;
}

/*
 * This variable stores the relative delta Y. Always
 * remember, to use it as int8_t.
 */
int8_t ADNS2610::getDeltaY()
{
  char x = readRegister(regDeltaY_);
  return x;
}

/*
 * This simple function just toggles the backlight on and off.
 */
void ADNS2610::backlight(boolean state)
{
  if(state)
  {
    if(ledPin_) digitalWrite(ledPin_, HIGH);
    else digitalWrite(rLedPin_, HIGH);
  } else
  {
    digitalWrite(ledPin_, LOW);
    digitalWrite(rLedPin_, LOW);
  }
}

/*
 * This integer value stores the average pixel, with the defined color:
 * 'r', 'g' or 'b'. Best use for the color sensor.
 */
int ADNS2610::getAvgPixel(char kolor)
{
  switch(kolor)
  {
    case 'r':
      digitalWrite(rLedPin_, HIGH);
      digitalWrite(gLedPin_, LOW);
      digitalWrite(bLedPin_, LOW);
      break;
    case 'g':
      digitalWrite(rLedPin_, LOW);
      digitalWrite(gLedPin_, HIGH);
      digitalWrite(bLedPin_, LOW);
      break;
    case 'b':
      digitalWrite(rLedPin_, LOW);
      digitalWrite(gLedPin_, LOW);
      digitalWrite(bLedPin_, HIGH);
      break;
  }
  delay(10);
  int x = readRegister(regAvgPixel_) * 128 / 324;
  digitalWrite(rLedPin_, LOW);
  digitalWrite(gLedPin_, LOW);
  digitalWrite(bLedPin_, LOW);
  return x;
}

/*
 * This variable stores the maximum registered value.
 * You need to define the color: 'r', 'g' or 'b'.
 */
int ADNS2610::getMaxPixel(char kolor)
{
  switch(kolor)
  {
    case 'r':
      digitalWrite(rLedPin_, HIGH);
      digitalWrite(gLedPin_, LOW);
      digitalWrite(bLedPin_, LOW);
      break;
    case 'g':
      digitalWrite(rLedPin_, LOW);
      digitalWrite(gLedPin_, HIGH);
      digitalWrite(bLedPin_, LOW);
      break;
    case 'b':
      digitalWrite(rLedPin_, LOW);
      digitalWrite(gLedPin_, LOW);
      digitalWrite(bLedPin_, HIGH);
      break;
  }
  delay(10);
  int x = readRegister(regMaxPixel_);
  digitalWrite(rLedPin_, LOW);
  digitalWrite(gLedPin_, LOW);
  digitalWrite(bLedPin_, LOW);
  return x;
}

/*
 * This variable stores the minimum registered value.
 * You need to define the color: 'r', 'g' or 'b'.
 */
int ADNS2610::getMinPixel(char kolor)
{
  switch(kolor)
  {
    case 'r':
      digitalWrite(rLedPin_, HIGH);
      digitalWrite(gLedPin_, LOW);
      digitalWrite(bLedPin_, LOW);
      break;
    case 'g':
      digitalWrite(rLedPin_, LOW);
      digitalWrite(gLedPin_, HIGH);
      digitalWrite(bLedPin_, LOW);
      break;
    case 'b':
      digitalWrite(rLedPin_, LOW);
      digitalWrite(gLedPin_, LOW);
      digitalWrite(bLedPin_, HIGH);
      break;
  }
  delay(10);
  int x = readRegister(regMinPixel_);
  digitalWrite(rLedPin_, LOW);
  digitalWrite(gLedPin_, LOW);
  digitalWrite(bLedPin_, LOW);
  return x;
}

/*
 * This variable stores the average pixel value.
 */
int ADNS2610::getAvgPixel()
{
  int x = readRegister(regAvgPixel_) * 128 / 324;
  return x;
}

/*
 * This variable stores the maximum pixel value.
 */
int ADNS2610::getMaxPixel()
{
  int x = readRegister(regMaxPixel_);
  return x;
}

/*
 * This variable stores the minimum pixel value.
 */
int ADNS2610::getMinPixel()
{
  int x = readRegister(regMinPixel_);
  return x;
}