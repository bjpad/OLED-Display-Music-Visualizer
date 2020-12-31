# OLED-Display-Music-Visualizer
# This code is meant for an Arduino nano communicating via I2C with a 128x32 OLED display. Additionally, DC analog voltages are read from a Frequency to Voltage Converter, and digital values from a button to change FSM states.

//For arduino nano choose the (Old Bootloader) 
//This OLED only has white pixels
//This display from RadioShack is a 32x128

//For bit maps
#include <splash.h>

//Arduino library for I2C
#include <Wire.h>

//Include Adafruit OLED & Graphics (GFX) libraries
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//OLED Data
#define OLED_DEVICEADDRESS 0x3C  //I2C device address

//Freq. to Volt Converter Input
#define FREQ_TO_VOLT_PIN A1

//Button Input Pin
#define BUTTON_PIN 2

#define NUM_STATES 4;


//Bit Map Stuff
#define KE_HEIGHT   32 //32
#define KE_WIDTH    48 //48

#define BE_HEIGHT   32 //32
#define BE_WIDTH    48 //48

#define HEART_HEIGHT 6
#define HEART_WIDTH 8

#define VOLTBAR_HEIGHT 1
#define VOLTBAR_WIDTH 16

#define STAR_HEIGHT 7
#define STAR_WIDTH 8

#define H20MAN_HEIGHT 21
#define H20MAN_WIDTH 32

#define BUBBLESMALL_HEIGHT 3
#define BUBBLESMALL_WIDTH 8

#define BUBBLELARGE_HEIGHT 4
#define BUBBLELARGE_WIDTH 8

#define CHEMBEAKER_HEIGHT 27
#define CHEMBEAKER_WIDTH 56

//Input Analog Voltage mapping
#define HALF_VOLT 102
#define ONE_VOLT 204
#define TWO_VOLTS 409
#define THREE_VOLTS 613
#define FOUR_VOLTS 818.4

//BeatIncreaseWait Threshold for beat to increase
#define BeatIncreaseWait_THRESHOLD 6     //This at 21 is just over a 100mV threshold: 0.1V / (5V / 2^10), 1 is for 4.8mV

//Reset pin not used, but needed for library to create "display" object
// for the OLED display. This objects represents our OLED display
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(OLED_RESET);

static const unsigned char PROGMEM Katie_ClosedEye[] =
{ B00000000, B00000000, B00000000, B00011110, B00000000, B00000000, //1
  B00000000, B00000000, B00011100, B01100001, B10000000, B00000000, //2
  B00000000, B00000000, B00100011, B10000000, B01000000, B00000000, //3
  B00000000, B00000000, B11011111, B11100000, B01000000, B00000000, //4
  B00000000, B00000000, B11100000, B00011100, B01110000, B00000000, //5
  B00000000, B00000111, B00011111, B11100011, B10110000, B00000000, //6
  B00000000, B00001000, B11100111, B10011100, B01110000, B00000000, //7
  B00000000, B00001001, B00000000, B00000010, B01110000, B00000000, //8
  B00000000, B00110001, B00000000, B00000010, B00110000, B00000000, //9
  B00000000, B00110110, B00000000, B00000001, B10110000, B00000000, //10
  B00000000, B01000110, B11100000, B00011101, B10001000, B00000000, //11
  B00000000, B01001001, B00011000, B01100010, B01001000, B00000000, //12
  B00000001, B10001000, B00000000, B00000000, B01000110, B00000000, //13
  B00000001, B10110001, B11100000, B00011110, B00110110, B00000000, //14
  B00000000, B01110000, B00111000, B01110000, B00111000, B00000000, //15
  B00000000, B00101000, B00000000, B00000000, B01111000, B00000000, //16
  B00000000, B00101000, B00000000, B00000000, B01111110, B00000000, //17
  B00000000, B00101000, B00000011, B00000000, B01111001, B00000000, //18
  B00000000, B00011110, B00000100, B10000001, B11111001, B00000000, //19
  B00000000, B00000110, B00000000, B00000001, B11001001, B00000000, //20
  B00000000, B00000110, B00000000, B00100001, B11001001, B00000000, 
  B00000000, B00000001, B00001111, B11000011, B10110000, B11000000, //22
  B00000000, B00000000, B11000000, B00001110, B00001000, B11000000,
  B00000000, B00000001, B00100000, B00010011, B10001001, B00000000, //24
  B00000000, B00000001, B00011000, B01100011, B11001001, B00000000,
  B00000000, B00000111, B00000111, B10000011, B11110110, B00000000, //26
  B00000000, B01111001, B00000000, B00000011, B10111111, B00000000,
  B00011111, B10001001, B00000000, B00000011, B10110000, B11111100, //28
  B11100000, B00001000, B11000000, B00001100, B01110000, B00000011,
  B00000000, B00000110, B11000000, B00001100, B01000000, B00000000, //30
  B00000000, B00000001, B00100000, B00010011, B10000000, B00000000,
  B00000000, B00000000, B11100000, B00011100, B00000000, B00000000, }; //32

static const unsigned char PROGMEM Katie_OpenEye[] =
{ B00000000, B00000000, B00000000, B00011110, B00000000, B00000000, //1.
  B00000000, B00000000, B00011100, B01100001, B10000000, B00000000, //2.
  B00000000, B00000000, B00100011, B10000000, B01000000, B00000000, //3
  B00000000, B00000000, B11011111, B11100000, B01000000, B00000000, //4
  B00000000, B00000000, B11100000, B00011100, B01110000, B00000000, //5
  B00000000, B00000111, B00011111, B11100011, B10110000, B00000000, //6
  B00000000, B00001000, B11100111, B10011100, B01110000, B00000000, //7
  B00000000, B00001001, B00000000, B00000010, B01110000, B00000000, //8
  B00000000, B00110001, B00000000, B00000010, B00110000, B00000000, //9
  B00000000, B00110110, B00000000, B00000001, B10110000, B00000000, //10
  B00000000, B01000110, B11100000, B00011101, B10001000, B00000000, //11
  B00000000, B01001001, B00011000, B01100010, B01001000, B00000000, //12
  B00000001, B10001000, B00000000, B00000000, B01000110, B00000000, //13
  B00000001, B10110001, B11100000, B00011110, B00110110, B00000000, //14
  B00000000, B01110000, B10111000, B01110100, B00111000, B00000000, //15
  B00000000, B00101000, B01100000, B00011000, B01111000, B00000000, //16
  B00000000, B00101000, B00000000, B00000000, B01111110, B00000000, //17
  B00000000, B00101000, B00000011, B00000000, B01111001, B00000000, //18
  B00000000, B00011110, B00000100, B10000001, B11111001, B00000000, //19
  B00000000, B00000110, B00000000, B00000001, B11001001, B00000000, //20
  B00000000, B00000110, B00000000, B00000001, B11001001, B00000000, 
  B00000000, B00000001, B00000111, B10000011, B10110000, B11000000, //22
  B00000000, B00000000, B11000000, B00001110, B00001000, B11000000,
  B00000000, B00000001, B00100000, B00010011, B10001001, B00000000, //24
  B00000000, B00000001, B00011000, B01100011, B11001001, B00000000,
  B00000000, B00000111, B00000111, B10000011, B11110110, B00000000, //26
  B00000000, B01111001, B00000000, B00000011, B10111111, B00000000,
  B00011111, B10001001, B00000000, B00000011, B10110000, B11111100, //28
  B11100000, B00001000, B11000000, B00001100, B01110000, B00000011,
  B00000000, B00000110, B11000000, B00001100, B01000000, B00000000, //30
  B00000000, B00000001, B00100000, B00010011, B10000000, B00000000,
  B00000000, B00000000, B11100000, B00011100, B00000000, B00000000, }; //32

  static const unsigned char PROGMEM Brian_ClosedEye[] =
{ B00000000, B00000000, B00000000, B00110000, B00000000, B00000000, //1.
  B00000000, B00000000, B01111111, B11010000, B00000000, B00000000, //2.
  B00000000, B00000000, B10000000, B00001000, B00000000, B00000000, //3
  B00000000, B00000001, B00000000, B00000100, B00000000, B00000000, //4
  B00000000, B00000010, B00001111, B11000010, B00000000, B00000000, //5
  B00000000, B00000010, B01110000, B00111001, B00000000, B00000000, //6
  B00000000, B00000100, B10000000, B00000100, B10000000, B00000000, //7
  B00000000, B00000100, B10000000, B00000100, B01000000, B00000000, //8
  B00000000, B00001001, B00111000, B01110010, B01000000, B00000000, //9
  B00000000, B00001001, B01000000, B00001010, B01000000, B00000000, //10
  B00000000, B00001010, B00000000, B00000001, B01000000, B00000000, //11
  B00000000, B00001010, B01111000, B01111001, B11000000, B00000000, //12
  B00000000, B00001010, B00001000, B01000001, B11000000, B00000000, //13
  B00000000, B00001010, B00000000, B00000001, B10000000, B00000000, //14
  B00000000, B00000100, B00000001, B00000000, B01000000, B00000000, //15
  B00000000, B00001010, B00000010, B00000001, B01000000, B00000000, //16
  B00000000, B00001010, B00000100, B00000001, B10000000, B00000000, //17
  B00000000, B00000110, B00000111, B00000001, B00000000, B00000000, //18
  B00000000, B00000010, B00000000, B00000001, B00000000, B00000000, //19
  B00000000, B00000001, B00000000, B00000010, B00000000, B00000000, //20
  B00000000, B00000001, B00000000, B01100010, B00000000, B00000000, 
  B00000000, B00000000, B10000111, B11000100, B00000000, B00000000, //22
  B00000000, B00000001, B01000000, B00001010, B00000000, B00000000,
  B00000000, B00000001, B00100000, B00010010, B00000000, B00000000, //24
  B00000000, B00000001, B00010000, B00100010, B00000000, B00000000,
  B00000000, B00000111, B00001111, B11000011, B11000000, B00000000, //26
  B00000000, B01111000, B00000000, B00000000, B00111111, B00000000,
  B00011111, B10000000, B00000000, B00000000, B00100000, B11111100, //28
  B11100000, B10000000, B00000000, B00000000, B01100000, B00000011,
  B00000000, B10000000, B00000000, B00000000, B01000000, B00000000, //30
  B00000000, B01100000, B00000000, B00000011, B10000000, B00000000,
  B00000000, B00010000, B00000000, B00011100, B00000000, B00000000, }; //32

  static const unsigned char PROGMEM Brian_OpenEye[] =
{ B00000000, B00000000, B00000000, B00110000, B00000000, B00000000, //1.
  B00000000, B00000000, B01111111, B11010000, B00000000, B00000000, //2.
  B00000000, B00000000, B10000000, B00001000, B00000000, B00000000, //3
  B00000000, B00000001, B00000000, B00000100, B00000000, B00000000, //4
  B00000000, B00000010, B00001111, B11000010, B00000000, B00000000, //5
  B00000000, B00000010, B01110000, B00111001, B00000000, B00000000, //6
  B00000000, B00000100, B10000000, B00000100, B10000000, B00000000, //7
  B00000000, B00000100, B10000000, B00000100, B01000000, B00000000, //8
  B00000000, B00001001, B00111000, B01110010, B01000000, B00000000, //9
  B00000000, B00001001, B01000100, B10001010, B01000000, B00000000, //10
  B00000000, B00001010, B00000000, B00000001, B01000000, B00000000, //11
  B00000000, B00001010, B01111000, B01111001, B11000000, B00000000, //12
  B00000000, B00001010, B00101000, B01010001, B11000000, B00000000, //13
  B00000000, B00001010, B00010000, B00100001, B10000000, B00000000, //14
  B00000000, B00000100, B00000001, B00000000, B01000000, B00000000, //15
  B00000000, B00001010, B00000010, B00000001, B01000000, B00000000, //16
  B00000000, B00001010, B00000100, B00000001, B10000000, B00000000, //17
  B00000000, B00000110, B00000111, B00000001, B00000000, B00000000, //18
  B00000000, B00000010, B00000000, B00000001, B00000000, B00000000, //19
  B00000000, B00000001, B00000000, B00000010, B00000000, B00000000, //20
  B00000000, B00000001, B00000000, B00000010, B00000000, B00000000, 
  B00000000, B00000000, B10000011, B10000100, B00000000, B00000000, //22
  B00000000, B00000001, B01000000, B00001010, B00000000, B00000000,
  B00000000, B00000001, B00100000, B00010010, B00000000, B00000000, //24
  B00000000, B00000001, B00010000, B00100010, B00000000, B00000000,
  B00000000, B00000111, B00001111, B11000011, B11000000, B00000000, //26
  B00000000, B01111000, B00000000, B00000000, B00111111, B00000000,
  B00011111, B10000000, B00000000, B00000000, B00100000, B11111100, //28
  B11100000, B10000000, B00000000, B00000000, B01100000, B00000011,
  B00000000, B10000000, B00000000, B00000000, B01000000, B00000000, //30
  B00000000, B01100000, B00000000, B00000011, B10000000, B00000000,
  B00000000, B00010000, B00000000, B00011100, B00000000, B00000000, }; //32

  static const unsigned char PROGMEM Heart[] = 
  {B01100110,
   B10011001,
   B10000001,
   B01000010,
   B00100100,
   B00011000 };

  static const unsigned char PROGMEM Star[] = 
  {B00010000,
   B00010000,
   B00111000,
   B11111110,
   B00111000,
   B00010000,
   B00010000 };
  
  static const unsigned char PROGMEM VoltBar[] = 
  {B11111111, B11111111};

  static const unsigned char PROGMEM H20_Man_Standing[] =
{ B00000000, B11111111, B11111100, B00000000, //1.
  B00000001, B00000000, B00001100, B00000000, //2.
  B00000010, B00000000, B00010100, B00000000, //3
  B00000100, B00000000, B00100100, B01000000, //4
  B10001111, B11111111, B11000100, B10000000, //5
  B01001000, B00000000, B01000101, B00000000, //6
  B00101000, B00000000, B01000110, B00000000, //7
  B00011001, B10000110, B01001100, B00000000, //8
  B00001010, B01001001, B01001100, B00000000, //9
  B00001000, B00000000, B01001100, B00000000, //10
  B00001000, B00000000, B01011100, B00000000, //11
  B00001000, B00000000, B01011100, B00000000, //12
  B00001000, B00000000, B01111100, B00000000, //13
  B00001000, B00011100, B01111000, B00000000, //14
  B00001000, B00000000, B01110000, B00000000, //15
  B00001111, B11111111, B11100000, B00000000, //16
  B00000001, B00000001, B00000101, B01100010, //17
  B00000001, B00000001, B00000111, B00010101, //18
  B00000001, B00000001, B00000101, B00100101, //19
  B00000001, B00000001, B00000101, B01110010, //20
  B00000001, B00000001, B00000000, B00000000};//21

 static const unsigned char PROGMEM H20_Man_Squatting[] =
{ B00000000, B11111111, B11111100, B00000000, //1.
  B00000001, B00000000, B00001100, B00000000, //2.
  B00000010, B00000000, B00010100, B00000000, //3
  B00000100, B00000000, B00100100, B01000000, //4
  B10001111, B11111111, B11000100, B10000000, //5
  B01001000, B00000000, B01000101, B00000000, //6
  B00101000, B00000000, B01000110, B00000000, //7
  B00011001, B10000110, B01001100, B00000000, //8
  B00001010, B01001001, B01001100, B00000000, //9
  B00001000, B00000000, B01001100, B00000000, //10
  B00001000, B00000000, B01011100, B00000000, //11
  B00001000, B00000000, B01011100, B00000000, //12
  B00001000, B00000000, B01111100, B00000000, //13
  B00001000, B00011100, B01111000, B00000000, //14
  B00001000, B00000000, B01110000, B00000000, //15
  B00001111, B11111111, B11100000, B00000000, //16
  B00000001, B00000001, B00000101, B01100010, //17
  B00000010, B00000010, B00000111, B00010101, //18
  B00000010, B00000010, B00000101, B00100101, //19
  B00000001, B00000001, B00000101, B01110010, //20
  B00000000, B00000000, B00000000, B00000000};//21

  static const unsigned char PROGMEM BubbleLarge[] = 
  {B01100000,
   B10010000,
   B10010000,
   B01100000};

   static const unsigned char PROGMEM BubbleSmall[] = 
  {B01000000,
   B10100000,
   B01000000};

  static const unsigned char PROGMEM ChemBeaker1[] =
{ B00000000, B00111000, B00000000, B00000000, B00000000, B00000000, B00000000, //1.
  B00000000, B11000110, B00000000, B00000000, B00000000, B00000000, B00000000, //2.
  B00000000, B10111010, B00000000, B00000000, B00000000, B00000000, B00000000, //3
  B00000000, B10000010, B00000000, B00000000, B00000000, B00000000, B00000000, //4
  B00000000, B10000010, B00000000, B00000000, B00000000, B00000000, B00000000, //5
  B00000001, B00000001, B00000000, B00000000, B00000000, B00000000, B00000000, //6
  B00000001, B00000001, B00000000, B00000000, B00000000, B00000000, B00000000, //7
  B00000010, B00000000, B10000000, B00000000, B00000000, B00000000, B00000000, //8
  B00000010, B00000000, B10000000, B00000000, B00000000, B00000000, B00000000, //9
  B00000100, B00000000, B01000000, B00000000, B00000000, B00000000, B00000000, //10
  B00000100, B00000000, B01000000, B00000000, B00000000, B00000000, B00000000, //11
  B00001000, B00000000, B00100000, B00000000, B00000000, B00000000, B00000000, //12
  B00001000, B00000000, B00100000, B00000100, B00000000, B00000000, B00000000, //13
  B00011111, B11111111, B11110000, B00000000, B00000000, B00000000, B00000000, //14
  B00011111, B11111111, B11110000, B10000110, B00000000, B00000000, B00000000, //15
  B00111111, B11111111, B11111111, B11100010, B00001000, B00000000, B00000000, //16
  B00111111, B11111111, B11111111, B10011000, B00000000, B00000000, B00000000, //17
  B01111111, B11111111, B11111111, B10100110, B00000000, B00000000, B00000000, //18
  B01111111, B11111111, B11111111, B11110001, B10000000, B00000000, B00000000, //19
  B11111111, B11111111, B11111110, B10011110, B01000000, B00000000, B00000000, //20
  B11111111, B11111111, B11111110, B00001101, B00111110, B00000000, B00000000, //21
  B01111111, B11111111, B11111100, B10001110, B11111111, B00000000, B00000000, //22
  B00000000, B00000000, B00000000, B10001111, B11111111, B10000000, B00000000, //23
  B00000000, B00000000, B00000000, B00001111, B11111011, B00000000, B00000000, //24
  B00000000, B00000000, B00000000, B00000111, B11100000, B00000000, B00000000, //25
  B00000000, B00000000, B00000000, B00000000, B10000000, B00000000, B00000000, //26
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000};//27

static const unsigned char PROGMEM ChemBeaker2[] =
{ B00000000, B00111000, B00000000, B00000000, B00000000, B00000000, B00000000, //1.
  B00000000, B11000110, B00000000, B00000000, B00000000, B00000000, B00000000, //2.
  B00000000, B10111010, B00000000, B00000000, B00000000, B00000000, B00000000, //3
  B00000000, B10000010, B00000000, B00000000, B00000000, B00000000, B00000000, //4
  B00000000, B10000010, B00000000, B00000000, B00000000, B00000000, B00000000, //5
  B00000001, B00000001, B00000000, B00000000, B00000000, B00000000, B00000000, //6
  B00000001, B00000001, B00000000, B00000000, B00000000, B00000000, B00000000, //7
  B00000010, B00000000, B10000000, B00000000, B00000000, B00000000, B00000000, //8
  B00000010, B00000000, B10000000, B00000000, B00000000, B00000000, B00000000, //9
  B00000100, B00000000, B01000000, B00000000, B00000000, B00000000, B00000000, //10
  B00000100, B00000000, B01000000, B00000000, B00000000, B00000000, B00000000, //11
  B00001000, B00000000, B00100000, B00000000, B00000000, B00000000, B00000000, //12
  B00001000, B00000000, B00100000, B00000000, B00000000, B00000000, B00000000, //13
  B00010000, B00000000, B00010000, B00000000, B00000000, B00000000, B00000000, //14
  B00010000, B00000000, B00010000, B10000000, B00000000, B00000000, B00000000, //15
  B00111111, B11111111, B11111111, B11100000, B00000000, B00000000, B00000000, //16
  B00111111, B11111111, B11111111, B10011001, B10000000, B00000000, B00000000, //17
  B01111111, B11111111, B11111111, B10100110, B10000000, B00000000, B00000000, //18
  B01111111, B11111111, B11111111, B11110001, B10000000, B00000000, B00000000, //19
  B11111111, B11111111, B11111110, B10011110, B01000000, B00100000, B00000000, //20
  B11111111, B11111111, B11111110, B00001101, B00111111, B11100000, B00000000, //21
  B01111111, B11111111, B11111100, B00001110, B11111111, B11111110, B00000000, //22
  B00000000, B00000000, B00000000, B01001111, B11111111, B11111111, B00000000, //23
  B00000000, B00000000, B00000000, B01101111, B11111111, B11111111, B00000000, //24
  B00000000, B00000000, B00000000, B00000111, B11101111, B11111110, B00000000, //25
  B00000000, B00000000, B00000000, B00000000, B10000010, B00011000, B00000000, //26
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000};//27

  static const unsigned char PROGMEM ChemBeaker3[] =
{ B00000000, B00111000, B00000000, B00000000, B00000000, B00000000, B00000000, //1.
  B00000000, B11000110, B00000000, B00000000, B00000000, B00000000, B00000000, //2.
  B00000000, B10111010, B00000000, B00000000, B00000000, B00000000, B00000000, //3
  B00000000, B10000010, B00000000, B00000000, B00000000, B00000000, B00000000, //4
  B00000000, B10000010, B00000000, B00000000, B00000000, B00000000, B00000000, //5
  B00000001, B00000001, B00000000, B00000000, B00000000, B00000000, B00000000, //6
  B00000001, B00000001, B00000000, B00000000, B00000000, B00000000, B00000000, //7
  B00000010, B00000000, B10000000, B00000000, B00000000, B00000000, B00000000, //8
  B00000010, B00000000, B10000000, B00000000, B00000000, B00000000, B00000000, //9
  B00000100, B00000000, B01000000, B00000000, B00000000, B00000000, B00000000, //10
  B00000100, B00000000, B01000000, B00000000, B00000000, B00000000, B00000000, //11
  B00001000, B00000000, B00100000, B00000000, B00000000, B00000000, B00000000, //12
  B00001000, B00000000, B00100000, B00000000, B00000000, B00000000, B00000000, //13
  B00010000, B00000000, B00010000, B00000000, B10000000, B00000000, B00000000, //14
  B00010000, B00000000, B00010000, B10000000, B00000000, B00000000, B00000000, //15
  B00100000, B00000000, B01111111, B11100000, B00000000, B00000000, B00000000, //16
  B00100000, B00000000, B01111111, B10011000, B00000000, B00000000, B00000000, //17
  B01111111, B11111111, B11111111, B10100110, B00000000, B00000000, B00000000, //18
  B01111111, B11111111, B11111111, B11110001, B11100000, B00000000, B00000000, //19
  B11111111, B11111111, B11111110, B10011110, B01100000, B00100011, B00000000, //20
  B11111111, B11111111, B11111110, B00001101, B00111111, B11111111, B11110000, //21
  B01111111, B11111111, B11111100, B00001110, B11111111, B11111110, B11111100, //22
  B00000000, B00000000, B00000000, B00001111, B11111111, B11111111, B11111110, //23
  B00000000, B00000000, B00000000, B00001111, B11111111, B11111111, B11111111, //24
  B00000000, B00000000, B00000000, B00111111, B11101111, B11111111, B11111110, //25
  B00000000, B00000000, B00000000, B00000000, B10000010, B00011001, B00010000, //26
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000};//27



   //FSM States as Global Variables
   enum FSMStates {Calibration, CouplePortrait, RandomStars, KemistTime};
   FSMStates State = Calibration;         //Initialize start-up state 

   int FSMStates_Index = 0;
   FSMStates AllStates[4] = {Calibration,
                             CouplePortrait,
                             RandomStars,
                             KemistTime};
   int StateChangeFlag = 0;

//////////////////////////////
  //SETUP FUNCTION//
////////////////////////////
void setup() {
  // put your setup code here, to run once:

  //Set Analog input pin for Freq. to Voltage converter & Button
  pinMode(FREQ_TO_VOLT_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);    //Enable Pullup resistor 
  pinMode(13, OUTPUT);            //LED for debugging

  Serial.begin(9600);
  
  //Start the I2C library - Initialize it on Arduino Nano
  Wire.begin();

  //Initialize I2C with OLED display on bus w/ address 0x3C
  display.begin(SSD1306_SWITCHCAPVCC, OLED_DEVICEADDRESS);
  
  //Clear display
  display.clearDisplay(); 

  //Initialize other display features
  display.setTextColor(WHITE);    //Set text color to white
  display.setTextSize(1);         //Set text size to 1
  display.setCursor(0,0);         //Set cursor position to (0, 0) - top left

  //Set up the first print on display
  //display.print("Music Animator");

  randomSeed(analogRead(0));      //Shuffles random function
}
//////////////////////////////////
  //LOOP FUNCTION//
///////////////////////////////////
void loop() {
  // put your main code here, to run repeatedly:

static int Voltage, VoltageMapped;                                              //For Calibration and CoupleState
  static int Sx1, Sy1, Sx2, Sy2, Sx3, Sy3, Sx4, Sy4, DisplayCount = 0;          //For the RandomStars State
  static int ButtonValue; 
  static int BB_S1x, BB_S2x, BB_L1x, BB_L2x, BB_L3x,                            //For the Kemist Time State. Bubble x & y coordinates
             BB_S1y, BB_S2y, BB_L1y, BB_L2y, BB_L3y;
  
  //display.drawBitmap(xpos, ypos, BITMAP, width, hieght, color);
/*
  while(1){
    ButtonValue = digitalRead(BUTTON_PIN);

    digitalWrite(13 ,ButtonValue);
  }*/

  switch(State){
    case(Calibration)://////////////////////////////////////////////////////////Calibration State
      StateChangeFlag = 0;     //Reset Flag to allow for changing states

      ButtonValue = digitalRead(BUTTON_PIN);     //Get Button value 

      if(ButtonValue == LOW){                    //If button value LOW, change state
        State = CouplePortrait;                  //Change to next State
        FSMStates_Index++;                       //Increment this for the array of states to remain consistent
        
        while(ButtonValue == LOW)                //Wait for button to be released
          ButtonValue = digitalRead(BUTTON_PIN);
      }
        
      Voltage = analogRead(FREQ_TO_VOLT_PIN);    //Read analog voltage from Frequency to Voltage Converter
      VoltageMapped = Voltage/HALF_VOLT;         //Map voltage data to Voltage values

      display.clearDisplay();                    //Clear Display
      PrintVoltBars(VoltageMapped);              //Print # of Voltage bars corresponding to analog Voltage value
      display.setCursor(0,0);                    //Set printing Cursor
      display.print("Calibrate");                //Print to screen at cursor location
      display.setCursor(0,8);
      display.print("Polymer");
      display.setCursor(0,16);
      display.print("Flexing:");

      display.setCursor(80, 8);
      display.print("--- Max");

      display.setCursor(80, 17);
      display.print("--- Perf");

      display.setCursor(80, 25);
      display.print("--- Low");
      
      display.display();
      break;
      
    case(CouplePortrait)://///////////////////////////////////////////////////////Start cpl state
      StateChangeFlag = 0;     //Reset Flag to allow for changing states
      
      for(int i = 0; i < 5; i++){
        
        Voltage = analogRead(FREQ_TO_VOLT_PIN);   //****If reading is being funky and reading 1023 constantly, check GND & VCC connections...
                                                  // VOLTAGE GND & 5V & 3.3V on Nano are shit when doing I2C
        VoltageMapped = Voltage/HALF_VOLT;         //Map the analog read to a voltage value (0, 1, 2, 3, 4, ..., or 10)
        //Serial.print(Voltage);                    //Printing to terminal just to check (debugging)
        //Serial.print("\n");
    
        //Prep Display to reprint all
        display.clearDisplay();
    
        //Print the voltage bars based on current sound/freq. level
        PrintVoltBars(VoltageMapped);
    
        //Redraw everything and then display bars & pictures to screen
        display.drawBitmap(0, 0, Katie_ClosedEye, KE_WIDTH, KE_HEIGHT, 1);
        display.drawBitmap(128 - BE_WIDTH, 0, Brian_ClosedEye, BE_WIDTH, BE_HEIGHT, 1);
        display.drawBitmap(128/2 - HEART_WIDTH/2, 0, Heart, HEART_WIDTH, HEART_HEIGHT, 1);
        display.display();
      }
  
      BeatIncreaseWait();                           //Wait for a beat to come before continuing 
      display.clearDisplay();

      //Print to screen all this
      display.drawBitmap(0, 0, Katie_OpenEye, KE_WIDTH, KE_HEIGHT, 1);
      display.drawBitmap(128 - BE_WIDTH, 0, Brian_OpenEye, BE_WIDTH, BE_HEIGHT, 1);
      display.display();    
      BeatIncreaseWait();                           //Wait for a beat to come before continuing 
      display.clearDisplay();
      break;

    case(RandomStars)://///////////////////////////////////////////////////////Random Stars State

      StateChangeFlag = 0;     //Reset Flag to allow for changing states******************

      //Wait for beat to step up
      BeatIncreaseWait();

      ButtonValue = digitalRead(BUTTON_PIN);

      /*if(ButtonValue == LOW){             ********
        State = Calibration;
        while(ButtonValue == LOW)                //Wait for button to be released
          ButtonValue = digitalRead(BUTTON_PIN);
      }*/

      //Clear display for new stars after so many prints
      if(DisplayCount > 2){
        display.clearDisplay();   
        DisplayCount = 0;                  //Reset Star Display Counter
      }

      display.setCursor(0,0);       //Display the Mode on the screen
      display.print("Star Gazing Mode");

      //Use random() to get the x & y coordinates for 4 separate stars
      Sx1 = random(0, 32);                                        //I KNOW there is a more efficient way to do this but whatever
      randomSeed(analogRead(0));    //Set new seed for random()
      Sy1 = random(8, 32 - STAR_HEIGHT);
      randomSeed(analogRead(0));    //Set new seed for random()
      
      Sx2 = random(33, 64);
      randomSeed(analogRead(0));    //Set new seed for random()
      Sy2 = random(8, 32 - STAR_HEIGHT);
      randomSeed(analogRead(0));    //Set new seed for random()
      
      Sx3 = random(65, 96);
      randomSeed(analogRead(0));    //Set new seed for random()
      Sy3 = random(8, 32 - STAR_HEIGHT);
      randomSeed(analogRead(0));    //Set new seed for random()

      Sx4 = random(95, 128 - STAR_WIDTH);
      randomSeed(analogRead(0));    //Set new seed for random()
      Sy4 = random(0, 32 - STAR_HEIGHT);

      //Draw all stars with thier random coordinates
      display.drawBitmap(Sx1, Sy1, Star, STAR_WIDTH, STAR_HEIGHT, 1);
      display.drawBitmap(Sx2, Sy2, Star, STAR_WIDTH, STAR_HEIGHT, 1);
      display.drawBitmap(Sx3, Sy3, Star, STAR_WIDTH, STAR_HEIGHT, 1);
      display.drawBitmap(Sx4, Sy4, Star, STAR_WIDTH, STAR_HEIGHT, 1);
      display.display();

      DisplayCount++;   //Increment star display counter each print iteration ( each display.display(); )
      
      break;

    case(KemistTime): ////////////////////////////////////////////////////

      StateChangeFlag = 0;              //Reset Flag to allow for changing states************
      static int ChemCounter = 1;

      BB_S1x = random(24, 70 - BUBBLESMALL_WIDTH);
      randomSeed(analogRead(0));    //Set new seed for random()
      BB_S2x = random(24, 70 - BUBBLESMALL_WIDTH);
      randomSeed(analogRead(0));    //Set new seed for random()
      BB_S1y = random(8, 18 - BUBBLESMALL_HEIGHT);
      randomSeed(analogRead(0));    //Set new seed for random()
      BB_S2y = random(8, 18 - BUBBLESMALL_HEIGHT);
      randomSeed(analogRead(0));    //Set new seed for random()

      BB_L1x = random(24, 70 - BUBBLELARGE_WIDTH);
      randomSeed(analogRead(0));    //Set new seed for random()
      BB_L2x = random(24, 70 - BUBBLELARGE_WIDTH);
      randomSeed(analogRead(0));    //Set new seed for random()
      BB_L3x = random(24, 70 - BUBBLELARGE_WIDTH);
      randomSeed(analogRead(0));    //Set new seed for random()
      BB_L1y = random(8, 18 - BUBBLELARGE_HEIGHT);
      randomSeed(analogRead(0));    //Set new seed for random()
      BB_L2y = random(8, 18 - BUBBLELARGE_HEIGHT);
      randomSeed(analogRead(0));    //Set new seed for random()
      BB_L3y = random(8, 18 - BUBBLELARGE_HEIGHT);
      randomSeed(analogRead(0));    //Set new seed for random()

      BeatIncreaseWait();
      display.clearDisplay();

      display.setCursor(0, 0);
      display.print("Kemist Mode");

      //Draw Bubbles
      display.drawBitmap(BB_S1x, BB_S1y, BubbleSmall, BUBBLESMALL_WIDTH, BUBBLESMALL_HEIGHT, 1);
      display.drawBitmap(BB_S2x, BB_S2y, BubbleSmall, BUBBLESMALL_WIDTH, BUBBLESMALL_HEIGHT, 1);
      
      display.drawBitmap(BB_L1x, BB_L1y, BubbleLarge, BUBBLELARGE_WIDTH, BUBBLELARGE_HEIGHT, 1);
      display.drawBitmap(BB_L2x, BB_L2y, BubbleLarge, BUBBLELARGE_WIDTH, BUBBLELARGE_HEIGHT, 1);
      display.drawBitmap(BB_L3x, BB_L3y, BubbleLarge, BUBBLELARGE_WIDTH, BUBBLELARGE_HEIGHT, 1);
      
      //Draw H20 Man & Beaker
      switch(ChemCounter){
        case(1):
          display.drawBitmap(0, 10, ChemBeaker1, CHEMBEAKER_WIDTH, CHEMBEAKER_HEIGHT, 1);
          display.drawBitmap(90, 12, H20_Man_Standing, H20MAN_WIDTH, H20MAN_HEIGHT, 1);
          break;

        case(2):
          display.drawBitmap(0, 10, ChemBeaker2, CHEMBEAKER_WIDTH, CHEMBEAKER_HEIGHT, 1);
          display.drawBitmap(90, 12, H20_Man_Standing, H20MAN_WIDTH, H20MAN_HEIGHT, 1);
          break;

        default:
          display.drawBitmap(0, 10, ChemBeaker3, CHEMBEAKER_WIDTH, CHEMBEAKER_HEIGHT, 1);
          display.drawBitmap(90, 8, H20_Man_Squatting, H20MAN_WIDTH, H20MAN_HEIGHT, 1);
          ChemCounter = 0;
          break;

      }
      
      
      display.display();
      ChemCounter++;

      break;

    default:///////////////////////////////////////////////////////Catch-all case
      State = Calibration;
      break;
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //FUNCTIONS//
///////////////////////
void PrintVoltBars(int VoltageMapped){

  static int VoltBarYIncrement = 2;           //Used to increment the Y coordinate for drawing Voltage bars
  
  switch(VoltageMapped){
      case(0):      //Print no bars
        break;
        
      case(1):      //Print 1 bar
        display.drawBitmap(128/2 - VOLTBAR_WIDTH/2, 28, VoltBar, VOLTBAR_WIDTH, VOLTBAR_HEIGHT, 1);
        break;

      case(2):      //Print 2 bars ...
        for(int i = 0; i < 2; i++){
          display.drawBitmap(128/2 - VOLTBAR_WIDTH/2, 28 - i*VoltBarYIncrement, VoltBar, VOLTBAR_WIDTH, VOLTBAR_HEIGHT, 1);
        }
        break;

      case(3):      //...
        for(int i = 0; i < 3; i++){
          display.drawBitmap(128/2 - VOLTBAR_WIDTH/2, 28 - i*VoltBarYIncrement, VoltBar, VOLTBAR_WIDTH, VOLTBAR_HEIGHT, 1);
        }
        break;

      case(4):      //...
        for(int i = 0; i < 4; i++){
          display.drawBitmap(128/2 - VOLTBAR_WIDTH/2, 28 - i*VoltBarYIncrement, VoltBar, VOLTBAR_WIDTH, VOLTBAR_HEIGHT, 1);
        }
        break;

      case(5):      //...
        for(int i = 0; i < 5; i++){
          display.drawBitmap(128/2 - VOLTBAR_WIDTH/2, 28 - i*VoltBarYIncrement, VoltBar, VOLTBAR_WIDTH, VOLTBAR_HEIGHT, 1);
        }
        break;

      case(6):      //...
        for(int i = 0; i < 6; i++){
          display.drawBitmap(128/2 - VOLTBAR_WIDTH/2, 28 - i*VoltBarYIncrement, VoltBar, VOLTBAR_WIDTH, VOLTBAR_HEIGHT, 1);
        }
        break;

      case(7):      //...
        for(int i = 0; i < 7; i++){
          display.drawBitmap(128/2 - VOLTBAR_WIDTH/2, 28 - i*VoltBarYIncrement, VoltBar, VOLTBAR_WIDTH, VOLTBAR_HEIGHT, 1);
        }
        break;

      case(8):      //...
        for(int i = 0; i < 8; i++){
          display.drawBitmap(128/2 - VOLTBAR_WIDTH/2, 28 - i*VoltBarYIncrement, VoltBar, VOLTBAR_WIDTH, VOLTBAR_HEIGHT, 1);
        }
        break;

      case(9):      //...
        for(int i = 0; i < 9; i++){
          display.drawBitmap(128/2 - VOLTBAR_WIDTH/2, 28 - i*VoltBarYIncrement, VoltBar, VOLTBAR_WIDTH, VOLTBAR_HEIGHT, 1);
        }
        break;

      case(10):      //...
        for(int i = 0; i < 10; i++){
          display.drawBitmap(128/2 - VOLTBAR_WIDTH/2, 28 - i*VoltBarYIncrement, VoltBar, VOLTBAR_WIDTH, VOLTBAR_HEIGHT, 1);
        }
        break;

      default:      //Catch-all in case something breaks
        break;
      
    }
}

/////////////////////////
//Built into this function that waits for a beat of the music to hit is a button value checker on one of the digital 
// input pins. This button can change the state of the FSM (device) and it can only be triggered once. Once triggered a flag variable
// gets set to ensure it can't happen again. This Flag is cleared at the beginning of each State 
void BeatIncreaseWait(void){        //This function waits for the beat to hit (i.e wait for Voltage to jump)

  int BeatLevel_Old = 1023, BeatLevel_New = 0;
  int ButtonValue2;

  while(BeatLevel_New < BeatLevel_Old + BeatIncreaseWait_THRESHOLD){    //New voltage reading must be greater than Old voltage reading + 21*(5V / 1023) Volts
      BeatLevel_Old = analogRead(FREQ_TO_VOLT_PIN);                     //Read 1st analog voltage
      delay(40);                                                        //delay to get a newer sound reading

      //Check button value and IF it's LOW then change the state ONLY ONCE. This is ensured through the State Change Flag variable.
      ButtonValue2 = digitalRead(BUTTON_PIN);
      if((ButtonValue2 == LOW) && (StateChangeFlag == 0)){
      ChangeState();                                                    //Change FSM state
        StateChangeFlag = 1;
      }
      
      BeatLevel_New = analogRead(FREQ_TO_VOLT_PIN);                     //Read 2nd analog voltage
      
      
      /* FOR DEBUGGING 
      Serial.print(BeatLevel_Old);
      Serial.print(" ");
      Serial.print(BeatLevel_New);
      Serial.print("\n");*/
  }

  //delay(50);          //Added delay so one beat can't cause this to happen twice
}
////////////////////////////////
//This function uses a global array of FSM States to control (based on a LOW button value on a DIN pin)
// which state it will change to.
void ChangeState(void){     
  FSMStates_Index++;                  //Increment index
    
  if(FSMStates_Index > 4 - 1)             //If Index exceeds max allowable index
    FSMStates_Index = 0;
    
  State = AllStates[FSMStates_Index]; //Change State

  Serial.print(FSMStates_Index);
  Serial.print("\n");

}
