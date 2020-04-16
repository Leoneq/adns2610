/* mouse_processing.pde - by Leoneq ;3, April 2020
 * This sketch allow you to see images from your mouse.
 * Remember to change serial port, frame period, and two things in draw() function. Read the comments.
 * (insert github here)
 * Based on code by Conor Peterson (robotrobot@gmail.com): https://conorpeterson.wordpress.com/2010/06/04/optical-mouse-hacking-part-1/
 * Have fun with your mouse cam!
 */
 
import processing.serial.*;

final int rate = 115200;
final int pixelSize = 25;
final int frameX = 18; //don't change. The frame is 18x18px
final int frameY = 18;
final int frameLen = frameX * frameY;

Serial port;
int[] frameR;
int[] frameG;
int[] frameB;
int serialCounter;

int nextFrameTime;
int framePeriod = 500;
/* change that for your setup:
 * 5 - 50FPS, best for color sensor/delta sensor
 * 300 - 5FPS, for grayscale image
 * 500 - 1FPS, for color image (each color frame takes 3 separated frames)
 */
char kolor = 'r';

void setup()
{
  size(450, 450); 

  frameR = new int[frameLen];
  frameG = new int[frameLen];
  frameB = new int[frameLen];

  initSerial();

  noStroke(); //we don't want grid
  //noSmooth();
  nextFrameTime = millis();
}

void draw()
{
  serialHandler();
  
  if ( millis() >= nextFrameTime )
  {   
    nextFrameTime = millis() + framePeriod;
    switch(kolor)
    {
    case 'r':
      requestFrame('g');
      break;
    case 'g':
      requestFrame('b');
      break;
    case 'b':
      requestFrame('r');
      break;
    }
    //requestFrame('r'); //comment the entire switch, and uncomment this line for grayscale image.
  } //simple routine for getting 3 frames.

  for ( int i = 0; i < frameLen; i++ ) //Let's draw 324 pixels!
  {
    fill(map(frameR[i], 0, 63, 0, 255), map(frameG[i], 0, 63, 0, 255), map(frameB[i], 0, 63, 0, 255)); //This simply converts values, and then paints pixel.
    //fill(map(frameR[i], 0, 63, 0, 255)); //If you want grayscale image, comment upper line and uncomment this one!
    rect(i % frameX * pixelSize, 
      i / frameY * pixelSize, 
      pixelSize, pixelSize); //This thing creates our pixel.
  }
}

void keyPressed()
{
  if ( key  == 'r' ) //just in case
    requestFrame('r'); 
  if ( key  == 'g' )
    requestFrame('g'); 
  if ( key  == 'b' )
    requestFrame('b'); 
  if (key == 's')
    saveFrame("mouse-####.jpg"); //If you want to save your selfie.
}

void initSerial()
{
  String portName = Serial.list()[1]; //Change to your serial port.
  port = new Serial(this, portName, rate);
  println("Using " + portName + " as serial device.");
}

void requestFrame(char r) //we send a character to arduino, and wait for the frame.
{
  kolor = r;
  port.write(r);

  serialCounter = frameLen;
}

void serialHandler()
{
  int incoming;
  while ( port.available() != 0 )
  {
    incoming = port.read();
    //print(incoming + " "); //debug function
    if (kolor == 'r' || kolor == 'g' || kolor == 'b')
    {
      if ( serialCounter > 0 )
      {
        if ( incoming == 127 )
          serialCounter = 0;
        else
        {
          switch(kolor)
          {
          case 'r':
            frameR[serialCounter - 1] = incoming;
            break;
          case 'g':
            frameG[serialCounter - 1] = incoming;
            break;
          case 'b':
            frameB[serialCounter - 1] = incoming;
            break;
          }
          serialCounter--;
        }
      }
    }
  }
}
