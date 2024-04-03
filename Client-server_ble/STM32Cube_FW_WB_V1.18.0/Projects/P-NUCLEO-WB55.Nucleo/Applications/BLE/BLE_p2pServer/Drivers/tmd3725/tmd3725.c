/*
 * tmd3725.c
 *
 *  Created on: 2023. 7. 7.
 *      Author: sean
 */
#include <stdint.h>
#include <stdio.h>

#include "tmd3725.h"
#include "stm32wbxx_nucleo_bus.h"


#if (0)
int32_t get_data(uint8_t *reginfo)
{
    /*
     * FUNCTION: Read from each register once and stored the values in array
     * ---------
     * INPUT: reginfo[35] - the array used to store all the values from registers, it should have size of 35
     * RETURN: 0 - success
     *         -1 - error
     */

    // /*Check if the size of registers info array */
    // if (((sizeof(reginfo))/(sizeof(int))) != 35) {
    //     printf("Register info array has the wrong size.");
    //     return -1;
    // }
	int addr[] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x8A, 0x8C,
			0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
			0x99, 0x9A, 0x9B, 0x9C, 0x9F, 0xAB, 0xC0, 0xC1, 0xD7, 0xD9, 0xDC, 0xDD};

    // TODO ADD ERROR CHECKING
	HAL_StatusTypeDef ret;

	for(int i = 0; i < 35; i++)
	{
		ret = BSP_I2C1_ReadReg(TMD3725_ADDR, addr[i], reginfo+i, 1);
		if(ret != BSP_ERROR_NONE) break;
	}
	return ret;
}

int32_t set_atime(uint8_t *reginfo, int cycle_No)
{
	/*
	 * FUNCTION: Change the integration time
	 * ---------
	 * INPUT: fd - the file descriptor of the i2c device
	 *        reginfo[35] - current values of all registers
	 *        cycle_No - integration cycle numbers, must be between 1-256
	 * RETURN: 0 - success
	 *         -1 - error
	 */

	if ( (cycle_No <= 256) && (cycle_No >= 1) )
	{
		reginfo[1] = cycle_No - 1;
	}
	else{ return -1; }

	// TODO ADD ERROR CHECKING
	//set_reg(TMD3725_ADDR, ATIME_ADDR, reginfo[1]);
	ret = BSP_I2C1_WriteReg(TMD3725_ADDR, ATIME_ADDR, reginfo+1, 1);
	return ret;
}

int32_t set_cfg1(uint8_t *reginfo, uint8_t IRtoG_flag, uint8_t again_flag)
{
	/*
	 * FUNCTION: Change the gain and ir to green setting
	 * ---------
	 * INPUT: fd - the file descriptor of the i2c device
	 *        reginfo[35] - current values of all registers
	 *        IRtoG_flag - 1 to enable ir to green, 0 to disable ir to green
	 *        again_flag - [x1 - gain of 1, x4 - gain of 4, x16 - gain of 16, x64 - gain of 64]
	 * RETURN: 0 - success
	 *         -1 - error
	 */
	if (IRtoG_flag) { reginfo[14] = reginfo[14] | 0x08; }
	else { reginfo[14] = reginfo[14] & 0xF7; }

	switch (again_flag)
	{
	case 1:
		reginfo[14] = reginfo[14] & 0xFC;
		break;
	case 4:
		reginfo[14] = reginfo[14] & 0xFD;
		reginfo[14] = reginfo[14] | 0x01;
		break;
	case 16:
		reginfo[14] = reginfo[14] & 0xFE;
		reginfo[14] = reginfo[14] | 0x02;
		break;
	case 64:
		reginfo[14] = reginfo[14] | 0x03;
		break;
	default:
		reginfo[14] = reginfo[14] & 0xFD;
		reginfo[14] = reginfo[14] | 0x01;
	}

	// TODO ADD ERROR CHECKING
	//set_reg(TMD3725_ADDR, CFG1_ADDR, reginfo[14]);
	ret = BSP_I2C1_WriteReg(TMD3725_ADDR, CFG1_ADDR, reginfo+14, 1);

	return ret;
}

int32_t enable_sensor(uint8_t *reginfo, uint8_t wait_flag, uint8_t prox_flag, uint8_t als_flag)
{
	/*
	 * FUNCTION: Change the enable register settings
	 * ---------
	 * INPUT: fd - the file descriptor of the i2c device
	 *        reginfo[35] - current values of all registers
	 *        wait_flag - 1 to enable wait feature, 0 to disable wait feature
	 *        prox_flag - 1 to enable proximity feature, 0 to disable wait feature
	 *        als_flag - 1 to enable als feature, 0 to disable wait feature
	 * RETURN: 0 - success
	 *         -1 - error
	 */
	if (wait_flag) {
		reginfo[0] = reginfo[0] | 0x08;
	} else {
		reginfo[0] = reginfo[0] & 0xF7;
	}

	if (prox_flag) {
		reginfo[0] = reginfo[0] | 0x04;
	} else {
		reginfo[0] = reginfo[0] & 0xFB;
	}

	if (als_flag) {
		reginfo[0] = reginfo[0] | 0x03;
	} else {
		reginfo[0] = reginfo[0] & 0xFC;
	}

	//set_reg(TMD3725_ADDR, ENABLE_ADDR, reginfo[0]);
	ret = BSP_I2C1_WriteReg(TMD3725_ADDR, ENABLE_ADDR, reginfo, 1);

	return ret;
}

int32_t get_optics_data(uint8_t *color_array)
{
	/*
	 * FUNCTION: Read from only the color data registers and store the values in array
	 * ---------
	 * INPUT: color_array[9] - the array used to store data from only the color data registers
	 * RETURN: 0 - success
	 *         -1 - error
	 */

	// TODO ADD ERROR CHECKING

	//uint8_t buf[2];

	//read Clear
	//ret = get_reg(TMD3725_ADDR, CDATAL_ADDR, buf, 1);
	ret = BSP_I2C1_ReadReg(TMD3725_ADDR, CDATAL_ADDR, color_array, 1);
	//color_array[0] = buf[0];

	ret = BSP_I2C1_ReadReg(TMD3725_ADDR, CDATAH_ADDR, color_array+1, 1);
	//color_array[1] = buf[0];

	//read Red
	ret = BSP_I2C1_ReadReg(TMD3725_ADDR, RDATAL_ADDR, color_array+2, 1);
	//color_array[2] = buf[0];

	ret = BSP_I2C1_ReadReg(TMD3725_ADDR, RDATAH_ADDR, color_array+3, 1);
	//color_array[3] = buf[0];

	//read Green
	ret = BSP_I2C1_ReadReg(TMD3725_ADDR, GDATAL_ADDR, color_array+4, 1);
	//color_array[4] = buf[0];

	ret = BSP_I2C1_ReadReg(TMD3725_ADDR, GDATAH_ADDR, color_array+5, 1);
	//color_array[5] = buf[0];

	//read Blue
	ret = BSP_I2C1_ReadReg(TMD3725_ADDR, BDATAL_ADDR, color_array+6, 1);
	//color_array[6] = buf[0];

	ret = BSP_I2C1_ReadReg(TMD3725_ADDR, BDATAH_ADDR, color_array+7, 1);
	//color_array[7] = buf[0];


	//read Proximity
	ret = BSP_I2C1_ReadReg(TMD3725_ADDR, PDATA_ADDR, color_array+8, 1);
	//color_array[8] = buf[0];

	return ret;
}

optics_val get_raw_color(const tmd3725 *sensor) {

    optics_val calibed;

    if ((get_optics_data(sensor->colorinfo)) != 0) {
    	return calibed;     // if errors occur when reading from register, return empty calibed
    }

    uint16_t temp = 0;

    temp = sensor->colorinfo[1];
    calibed.clear = temp<<8 | sensor->colorinfo[0];

    temp = sensor->colorinfo[3];
    calibed.red = temp<<8 | sensor->colorinfo[2];

    temp = sensor->colorinfo[5];
    calibed.green = temp<<8 | sensor->colorinfo[4];

    temp = sensor->colorinfo[7];
    calibed.blue = temp<<8 | sensor->colorinfo[6];

    calibed.CCT = 0;
    calibed.Lux = 0;
    calibed.IR = 0;

    return calibed;
}

void print_color(const optics_val *color_data) {
    /*
     * FUNCTION: Print the color from the processed data struct
     * ---------
     * INPUT: color_data - the struct that is storing caliberated color data
     * RETURN: void
     */
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_WHITE   "\x1b[37m"
#define ANSI_COLOR_RESET   "\x1b[0m"

    /* formatting the data to left aligned, zero decimal, width of eight floats */
    printf( "\n\r|CLEAR   |RED     |GREEN   |BLUE    |\n\r"
    		"|"ANSI_COLOR_WHITE"%-8.0f"ANSI_COLOR_RESET
    		"|"ANSI_COLOR_RED"%-8.0f"ANSI_COLOR_RESET
			"|"ANSI_COLOR_GREEN"%-8.0f"ANSI_COLOR_RESET
			"|"ANSI_COLOR_BLUE"%-8.0f"ANSI_COLOR_RESET"|\n\r",
    		color_data->clear, color_data->red, color_data->green, color_data->blue);
}
//
int32_t begin(tmd3725 *sensor_t)
{
	ret = get_data(sensor_t->reginfo);
	ret = set_atime(sensor_t->reginfo, 144);
	ret = set_cfg1(sensor_t->reginfo, 0, 16);
	ret = enable_sensor(sensor_t->reginfo, 0, 0, 1);
	return ret;
}

void Color_Init()
{
	begin(&sensor);
}

void Color_Process(){

	color_data = get_raw_color(&sensor);
	print_color(&color_data);

}
#endif
