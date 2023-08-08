/* This file is for documentation of how conversion an array of bytes (sent over BLE) to the desired values may look.
*        | building id |  room id   |  sensor id  |        time         | pos in sensor (x,y) |  speed   | direction  |    height (10 um)   |          rgb           |
* ---------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  range |   [0,255]   | [0,65536]  |   [0,255]   | [00:00:00,24:59:59] |    [(0,0),(7,7)]    | not sure | [0,36000]  |   [0,21336] (~7ft)  | [00:00:00,255:255:255] |
*  type  |   uint8_t   | uint8_t[2] |   uint8_t   |      uint8_t[3]     |       uint8_t       | uint8_t  | uint8_t[2] |      uint8_t[2]     |       uint8_t[3]       |
*/ 

/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

double byteToFrac(int b);

int fracToByte(double f);

char* byteStreamToQuery(int input[]);

void dataToByteStream(uint8_t byteStream[], uint8_t identifiers[], char* clkTime, float occupantData[], float colorData[]);