
/* This file is for documentation of how conversion an array of bytes (sent over BLE) to the desired values may look.
*        | building id |  room id   |  sensor id  |        time         | pos in sensor (x,y) |     speed    | direction  |    height (10 um)   |       rgb (cie coord)       |
* -------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  range |   [0,255]   | [0,65536]  |   [0,255]   | [00:00:00,24:59:59] |    [(0,0),(7,7)]    | [0.00,10.00] | [0,36000]  |   [0,21336] (~7ft)  | [(0,0),(0.996094,0.996094)] |
*  type  |   uint8_t   | uint8_t[2] |   uint8_t   |      uint8_t[7]     |       uint8_t       |    uint8_t   | uint8_t[2] |      uint8_t[2]     |          uint8_t[2]         |
*/ 
//TODO: add assert statements for unrealistic params: ie.) time = 99:99:99
//      implement time encoding and decoding

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//converts byte to fraction from 0.0 to 0.996094
double byteToFrac(int b){
    double frac = 0;
    frac += (float)((0b10000000 & b) >> 7)/2;
    frac += (float)((0b01000000 & b) >> 6)/4;
    frac += (float)((0b00100000 & b) >> 5)/8;
    frac += (float)((0b00010000 & b) >> 4)/16;
    frac += (float)((0b00001000 & b) >> 3)/32;
    frac += (float)((0b00000100 & b) >> 2)/64;
    frac += (float)((0b00000010 & b) >> 1)/128;
    frac += (float)(0b00000001 & b)/256;
    
    return frac;
}

//converts fraction to byte fraction f -> 0.byte
int fracToByte(double f){
    double frac = 0;
    if((f-(1.0/2)) > 0){
        frac += 0b10000000;
        f -= 1.0/2;
    } 
    if((f-(1.0/4)) > 0){
        frac += 0b01000000;
        f -= 1.0/4;
    } 
    if((f-(1.0/8)) > 0){
        frac += 0b00100000;
        f -= 1.0/8;
    } 
    if((f-(1.0/16)) > 0){
        frac += 0b00010000;
        f -= 1.0/16;
    } 
    if((f-(1.0/32)) > 0){
        frac += 0b00001000;
        f -= 1.0/32;
    } 
    if((f-(1.0/64)) > 0){
        frac += 0b00000100;
        f -= 1.0/64;
    } 
    if((f-(1.0/128)) > 0){
        frac += 0b00000010;
        f -= 1.0/128;
    } 
    if((f-(1.0/256)) > 0){
        frac += 0b00000001;
        f -= 1.0/256;
    } 

    return frac;
}

//converts stream of 21 bytes to a query string for the PHP server
char* byteStreamToQuery(uint8_t input[]){

    int bldgID, roomID, sensID, pos[2];
    float speed, dir, height, color[2];
    char clkTime[26]; //excess length to compensate for potential issue "65535-255-255 255:255:255"

    // first get building ID, room ID, and sensor ID
    bldgID = input[2];
    roomID = (input[3] << 8) + input[4];
    sensID = input[5];

    // time conversion
    //assert(input[10] < 25 && input[11] < 60 && input[12] < 60);  //only allow 00:00:00 to 24:59:59
    sprintf(clkTime, "%04d-%02d-%02d %02d:%02d:%02d", (input[6]<<8) + input[7], input[8], input[9], input[10], input[11], input[12]);  

    // occupant data
    pos[0] = input[13] >> 4;          // split byte: ie.) 0110 1000 becomes (0b0110, 0b1000) = (6,8)
    pos[1] = input[13] & 0b00001111;
    speed = (float)input[14]*10/255; //no ones going faster than 10m/s
    dir = (float)((input[15] << 8) + input[16])*359.99/65535;
    height = (float)((input[17] << 8) + input[18])*4000/65535; //255 cm is really tall
    
    // color data
    color[0] = byteToFrac(input[19]);
    color[1] = byteToFrac(input[20]);
    //color[2] = byteToFrac(input[17]); //check if correct

    //allocates memory for string, 7000 Bytes are allocated because in the worst case scenario with 64 occupants under one sensor, there would need to be 7000 Bytes of memory for the Query.
    //TO DO: Program needs to be able to extend query to account for multiple occupants.
    char* query = (char*) malloc(sizeof(char) * 7000); 
    //char query[7000];
    sprintf(query, "BID=%d&RID=%d&SID=%d&PosX=%d&PosY=%d&Speed=%.2f&Direction=%.2f&Height=%.2f&cieX=%.2f&cieY=%.2f",bldgID, roomID, sensID, pos[0], pos[1], speed, dir, height, color[0],color[1]);
    printf("PosX = %d\nPosY = %d\nSpeed = %.2f\nDirection = %.2f\nHeight = %.2f\n", pos[0], pos[1], speed, dir, height);

    //TO DO: FIX ADDING TIME
    return query;
}

//converts data from ToF sensor, color sensor, clock, and network to a sendable byte stream
void dataToByteStream(uint8_t byteStream[], uint8_t identifiers[], char* clkTime, float occupantData[], float colorData[]){
    int year;
    
    // identifiers = {bldg ID, room ID, sens ID}
    byteStream[0] = identifiers[0];
    byteStream[1] = identifiers[1]; //room ID is 2 bytes
    byteStream[2] = identifiers[2];
    byteStream[3] = identifiers[3];
    
    // time = "YYYY-MM-DD HH:MM:SS"
    year = (int)(clkTime[0]-'0')*1000 + (int)(clkTime[1]-'0')*100 + (int)(clkTime[2]-'0')*10 + (int)(clkTime[3]-'0');
    byteStream[4] = year >> 8;
    byteStream[5] = year & 0x00FF;
    byteStream[6] = (int)(clkTime[5]-'0')*10 + (int)(clkTime[6]-'0');
    byteStream[7] = (int)(clkTime[8]-'0')*10 + (int)(clkTime[9]-'0');
    byteStream[8] = (int)(clkTime[11]-'0')*10 + (int)(clkTime[12]-'0');
    byteStream[9] = (int)(clkTime[14]-'0')*10 + (int)(clkTime[15]-'0');
    byteStream[10] = (int)(clkTime[17]-'0')*10 + (int)(clkTime[18]-'0');

    // occupantData = {row, col, speed, dir, height}
    byteStream[11] = ((int)occupantData[0]<<4) + (int)occupantData[1]; //row col each take up 4 bits
    byteStream[12] = (int)(occupantData[2]*256/10);
    byteStream[13] = (int)(occupantData[3]*65535/359.99) >> 8; //2 byte
    byteStream[14] = (int)(occupantData[3]*65535/359.99) & 0x00FF;
    byteStream[15] = (int)(occupantData[4]*65535/4000) >> 8; //2 byte
    byteStream[16] = (int)(occupantData[4]*65535/4000) & 0x00FF;

    // colorData = {CIE x Coord, CIE y Coor}
    byteStream[17] = fracToByte(colorData[0]);
    byteStream[18] = fracToByte(colorData[1]);

    return *byteStream;
}
