/*
 * XYZ.h
 *
 *  Created on: 2023. 7. 7.
 *      Author: sean
 */

#ifndef INC_XYZ_H_
#define INC_XYZ_H_

#include "stm32wbxx_nucleo_bus.h"



#include <stdint.h>
#include <stdio.h>
#if (0)


//REGISTERS
#define TMD3725_ADDR 	(0x39 << 1)

#define ENABLE_ADDR     (0x80)	// R/W    0
#define ATIME_ADDR      (0x81)	// R/W    1
#define PTIME_ADDR      (0x82)	// R/W    2
#define WTIME_ADDR      (0x83)	// R/W    3
#define AILTL_ADDR      (0x84)	// R/W    4
#define AILTH_ADDR      (0x85)	// R/W    5
#define AIHTL_ADDR      (0x86)	// R/W    6
#define AIHTH_ADDR      (0x87)	// R/W    7
#define PILT_ADDR       (0x88)	// R/W    8
#define PIHT_ADDR       (0x8A)	// R/W    9
#define PERS_ADDR       (0x8C)	// R/W    10
#define CFG0_ADDR       (0x8D)	// R/W    11
#define PCFG0_ADDR      (0x8E)	// R/W    12
#define PCFG1_ADDR      (0x8F)	// R/W    13
#define CFG1_ADDR       (0x90)	// R/W    14
#define REVID_ADDR      (0x91)	// R      15
#define ID_ADDR         (0x92)	// R      16
#define STATUS_ADDR     (0x93)	// R/SC   17
#define CDATAL_ADDR     (0x94)	// R      18
#define CDATAH_ADDR     (0x95)	// R      19
#define RDATAL_ADDR     (0x96)	// R      20
#define RDATAH_ADDR     (0x97)	// R      21
#define GDATAL_ADDR     (0x98)	// R      22
#define GDATAH_ADDR     (0x99)	// R      23
#define BDATAL_ADDR     (0x9A)	// R      24
#define BDATAH_ADDR     (0x9B)	// R      25
#define PDATA_ADDR      (0x9C)	// R      26
#define CFG2_ADDR       (0x9F)	// R/W    27
#define CFG3_ADDR       (0xAB)	// R/W    28
#define POFFSETL_ADDR   (0xC0)	// R/W    29
#define POFFSETH_ADDR   (0xC1)	// R/W    30
#define CALIB_ADDR      (0xD7)	// R/W    31
#define CALIBCFG_ADDR   (0xD9)	// R/W    32
#define CALIBSTAT_ADDR  (0xDC)	// R/W    33
#define INTENAB_ADDR    (0xDD)	// R/W    34

//END OF REGISTERS

#define CSTHRESHOLD 0.5       // set the sensitivity for sensing color shift, higher the less sensitive

typedef struct optics_val
{
	 // struct that stored the processed data that can be used in later analysis
	 float red;
	 float green;
	 float blue;
	 float clear;
	 float IR;
	 float CPL;
	 float Lux;
	 float CCT;
} optics_val;

typedef struct TMD3725
{
	uint8_t reginfo[35];
	uint8_t colorinfo[9];
}tmd3725;

//global var
int32_t ret;
tmd3725 sensor = { {0}, {0} };
optics_val color_data;


int32_t get_data(uint8_t *reginfo);

int32_t set_atime(uint8_t *reginfo, int cycle_No);

int32_t set_cfg1(uint8_t *reginfo, uint8_t IRtoG_flag, uint8_t again_flag);

int32_t enable_sensor(uint8_t *reginfo, uint8_t wait_flag, uint8_t prox_flag, uint8_t als_flag);

int32_t get_optics_data(uint8_t *color_array);

optics_val get_raw_color(const tmd3725 *sensor);

void print_color(const optics_val *color_data);

int32_t begin(tmd3725 *sensor_t);

void Color_Init(void);

void Color_Process(void);

#endif /* INC_XYZ_H_ */

#endif
