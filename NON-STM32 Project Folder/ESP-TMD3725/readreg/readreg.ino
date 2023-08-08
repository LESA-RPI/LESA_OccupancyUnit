#include "Wire.h"
#include "registers.h"


#define SCL 5
#define SDA 4
#define GPIO1  2
#define LPN 3

typedef struct TMD3725
{
	uint8_t reginfo[35];
	uint8_t colorinfo[9];
}tmd3725;

tmd3725 color_sensor;


void get_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, size_t len) {
    Wire.beginTransmission(dev_addr);
    Wire.write(reg_addr);
    Wire.endTransmission();
    Wire.requestFrom(dev_addr, len); // We want one byte
    int pointer=0;
    while(len!=0){
      data[pointer] =Wire.read();
      pointer++;
      len--;
    }
}
void set_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t data) {
    Wire.beginTransmission(dev_addr);
    Wire.write(reg_addr);
    Wire.write(data);
    Wire.endTransmission();
}

uint8_t get_data(uint8_t* reginfo) {

    /*Enable to PILT*/
    get_reg(TMD3725_ADDR, ENABLE_ADDR, reginfo, 9);

    // /*PIHT*/
    get_reg(TMD3725_ADDR, PIHT_ADDR, reginfo+9, 1);

    // /*PERS to PCFG1*/
    get_reg(TMD3725_ADDR, PERS_ADDR, reginfo+10, 4);

    // /*CFG1 to PDATA*/
    get_reg(TMD3725_ADDR, CFG1_ADDR, reginfo+14, 13);

    // /*CFG2*/
    get_reg(TMD3725_ADDR, CFG2_ADDR, reginfo+27, 1);

    // /*CFG3*/
    get_reg(TMD3725_ADDR, CFG3_ADDR, reginfo+28, 1);

    // /*POFFSETL*/
    get_reg(TMD3725_ADDR, POFFSETL_ADDR, reginfo+29, 1);

    // /*POFFSETH*/
    get_reg(TMD3725_ADDR, POFFSETH_ADDR, reginfo+30, 1);

    // /*CALIB*/
    get_reg(TMD3725_ADDR, CALIB_ADDR, reginfo+31, 1);

    // /*CALIBCFG*/
    get_reg(TMD3725_ADDR, CALIBCFG_ADDR, reginfo+32, 1);

    // /*CALIBSTAT*/
    get_reg(TMD3725_ADDR, CALIBSTAT_ADDR, reginfo+33, 1);

    // /*INTENAB*/
    get_reg(TMD3725_ADDR, INTENAB_ADDR, reginfo+34, 1);

    return 0;
}
uint8_t get_optics_data(uint8_t color_array[]) {
    /*
     * FUNCTION: Read from only the color data registers and store the values in array
     * ---------
     * INPUT: color_array[9] - the array used to store data from only the color data registers
     * RETURN: 0 - success
     *         -1 - error
     */

    // TODO ADD ERROR CHECKING
    get_reg(TMD3725_ADDR, CDATAL_ADDR, color_array, 9);
        // if ((color_array[i] = get_reg(TMD3725_ADDR, CDATAL_ADDR+i)) == -1) {
        //     printf("Error happens when reading color value.");
        //     return -1;
        // }

    return 0;
}

void read_ambient(uint8_t* reginfo);


void setup() {
  // put your setup code here, to run once:
  //pinMode(4, INPUT_PULLDOWN);
  //pinMode(5, INPUT_PULLDOWN);
  Wire.begin(4,5,100000);
  Serial.begin(115200);

  get_data(color_sensor.reginfo);

//Find all I2C devices
  int nDevices = 0;
  int address;
  uint error;
  for(address = 1; address < 127; address++ ) 
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4) 
    {
      Serial.print("Unknown error at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
//End find all I2C devices

//print regs
  int iteration = 0;
  while(iteration<35) {
    Serial.print("Element ");
    Serial.print(iteration);
    Serial.print(": ");
    Serial.println(color_sensor.reginfo[iteration]);
    iteration++;
  }
  //finish print regs
//init sensor
  set_reg(TMD3725_ADDR,ENABLE_ADDR,0b00000011);//enable sensor
  set_reg(TMD3725_ADDR,CFG1_ADDR,0b00000011);  //set A gain 64
  set_reg(TMD3725_ADDR,WTIME_ADDR,0b11111111);  //set A gain 64



}
//

void loop() {

  neopixelWrite(RGB_BUILTIN,1,0,0); // Red
  delay(100);
  neopixelWrite(RGB_BUILTIN,0,1,0); // Green
  delay(100);
  neopixelWrite(RGB_BUILTIN,0,0,1); // Blue
  delay(100);

  read_ambient(color_sensor.reginfo);

}

void read_ambient(uint8_t* reginfo){
  get_data(reginfo);
  /*
#define CDATAL_ADDR     0x94	// R      18
#define CDATAH_ADDR     0x95	// R      19
#define RDATAL_ADDR     0x96	// R      20
#define RDATAH_ADDR     0x97	// R      21
#define GDATAL_ADDR     0x98	// R      22
#define GDATAH_ADDR     0x99	// R      23
#define BDATAL_ADDR     0x9A	// R      24
#define BDATAH_ADDR     0x9B	// R      25*/
/*
  uint16_t clearreg=reginfo[18]|(reginfo[19]<<8);
  uint16_t redreg=reginfo[20]|(reginfo[21]<<8);
  uint16_t greenreg=reginfo[22]|(reginfo[23]<<8);
  uint16_t bluereg=reginfo[24]|(reginfo[25]<<8);*/
  uint16_t clearreg=reginfo[19];
  uint16_t redreg=reginfo[21];
  uint16_t greenreg=reginfo[23];
  uint16_t bluereg=reginfo[25];
  Serial.printf("Clear: %u\tRed: %u\tGreen: %u\tBlue: %u\n",clearreg,redreg,greenreg,bluereg);
}

