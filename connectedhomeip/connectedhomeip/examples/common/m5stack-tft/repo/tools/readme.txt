
Program to convert any ttf font to c source file that can be includes in ESP32 tft library.

This is a windows program, but can be used under Linux with wine:

Usage:

ttf2c_vc2003.exe <point-size> <input-file> <output-file> [<start_char> <end_char>]

or, under Linux:

wine ./ttf2c_vc2003.exe <point-size> <input-file> <output-file> [<start_char> <end_char>]


Example:
--------

wine ./ttf2c_vc2003.exe 18 Vera.ttf vera.c


After the c source is created, open it in editor and make following changes:

Delete the line:

#include <avr/pgmspace.h>

Change the line:

uint8_t vera18[] PROGMEM =

to:

unsigned char vera18[] =


The font name ("vera18" here) will be different for different font,
you can change it to any other name.

