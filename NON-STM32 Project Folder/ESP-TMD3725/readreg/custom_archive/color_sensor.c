#include "color_sensor.h"
#include <stdint.h>
#include <stdio.h>

#define AUTOINTTIMETEST_FLAG 1      //flag for test prints


// Gain setting
#define x1 1
#define x4 4
#define x16 16
#define x64 64


void tmd3725_begin(struct TMD3725* sensor_t, uint8_t c)
{
	get_data(sensor_t->reginfo);
  /*
	set_atime(sensor_t->reginfo, 144);
	set_cfg1(sensor_t->reginfo, c, 16);
	enable_sensor(sensor_t->reginfo, 0, 0, 1);*/
}

void get_data(uint8_t *reginfo)
{  // TODO ADD ERROR CHECKING
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
}