/*
ADNS2610.h  - ADNS2610 Arduino Library - by Leoneq ;3, April 2020
(insert github here)
Based on code by Conor Peterson (robotrobot@gmail.com): https://conorpeterson.wordpress.com/2010/06/04/optical-mouse-hacking-part-1/
*/

#ifndef ADNS2610_h
#define ADNS2610_h

#include "Arduino.h"

class ADNS2610  
{
    public:
        ADNS2610(int dataPin, int clockPin, int ledPin);
        ADNS2610(int dataPin, int clockPin, int rLedPin, int gLedPin, int bLedPin);
        int8_t getDeltaX();
        int8_t getDeltaY();
        int getAvgPixel(char kolor);
        int getMinPixel(char kolor);
        int getMaxPixel(char kolor);
        int getAvgPixel();
        int getMinPixel();
        int getMaxPixel();
        void readFrame(byte* frame);
        void readFrame(byte* frame, char color);
		void begin(boolean mode);
        void blinkLED();
        void debug();
        void backlight(boolean state);
    private:
        byte dataPin_;
        byte clockPin_;
		byte ledPin_;
        byte rLedPin_;
        byte gLedPin_;
        byte bLedPin_;
        byte deltaX_;
        byte deltaY_;
        boolean mode_;
        void writeRegister(byte addr, byte data);
        byte readRegister(byte addr);
        const byte regConfig_    = 0x00; //change to your mouse chip, according to your datasheet
        const byte regStatus_    = 0x01;
        const byte regMinPixel_  = 0x06;
        const byte regDeltaX_    = 0x03;
        const byte regDeltaY_    = 0x02;
        const byte regMaxPixel_  = 0x05;
        const byte regAvgPixel_  = 0x07;
        const byte regPixelData_ = 0x08;
        const byte maskNoSleep_  = 0x01;
        const byte maskPID_      = 0xE0;
};

#endif