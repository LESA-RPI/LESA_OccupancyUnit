/*
 * XYZ.c
 *
 *  Created on: 2023. 7. 7.
 *      Author: sean
 */
#include <stdint.h>
#include <stdio.h>

#include "TCS3430.h"
#include "app_tof.h"
#include "stm32wbxx_nucleo_bus.h"
/*new code*/
//#include "pthread.h"
#include "stdio.h"
#include "string.h"
//#include "unistd.h"

tcs3430 sensor = { {0}, {0} };
tcs3430_optics_val XYZ_data;
int32_t ret;
int16_t interruptrange = 255;

int32_t get_data(uint8_t *reginfo)
{
    /*
     * FUNCTION: Read from each register once and stored the values in array
     * ---------
     * INPUT: reginfo[24] - the array used to store all the values from registers, it should have size of 35
     * RETURN: 0 - success
     *         -1 - error
     */
	int addr[] = {0x80, 0x81, 0x83, 0x84, 0x85, 0x86, 0x87, 0x8C,
				0x8D, 0x90, 0x91, 0x92, 0x94, 0x95, 0x96, 0x97,
				0x98, 0x99, 0x9A, 0x9B, 0x9F, 0xAB, 0xD6, 0xDD};

    // TODO ADD ERROR CHECKING
	HAL_StatusTypeDef ret;

	for(int i = 0; i < 25; i++)
	{
		ret = BSP_I2C1_ReadReg(TCS3430_ADDR, addr[i], reginfo+i, 1);
		if(ret != BSP_ERROR_NONE) break;
	}
	return ret;
}

int32_t set_atime(uint8_t *reginfo, int cycle_No)
{
	/*
	 * FUNCTION: Change the integration time
	 * ---------
	 * INPUT: reginfo[24] - current values of all registers
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
	//set_reg(TCS3430_ADDR, ATIME_ADDR, reginfo[1]);
	ret = BSP_I2C1_WriteReg(TCS3430_ADDR, ATIME_ADDR, reginfo+1, 1);
	return ret;
}

int32_t set_cfg0(uint8_t *reginfo, uint8_t mode)
{
	/*
	 * FUNCTION: Change the wait time
	 * ---------
	 * INPUT: reginfo[24] - current values of all registers
	 *        cycle_No - integration cycle numbers, must be between 1-256
	 * RETURN: 0 - success
	 *         -1 - error
	 */
	if(mode)
	{
		reginfo[8] |= 0x04;
	}
	else
	{
		reginfo[8] &= 0x80;
	}
	ret = BSP_I2C1_WriteReg(TCS3430_ADDR, CFG0_ADDR, reginfo+8, 1);
}

int32_t set_cfg1(uint8_t *reginfo, uint8_t ALS_Mul, uint8_t again_flag)
{
	/*
	 * FUNCTION: Change the gain and IR2 setting
	 * ---------
<<<<<<< Updated upstream
	 * INPUT: reginfo[24] - current values of all registers
=======
	 * INPUT: fd - the file descriptor of the i2c device
	 *        reginfo[25] - current values of all registers
>>>>>>> Stashed changes
	 *        ALS_Mul - Sets the CH3 input. Default = 0 (X Channel). Set to 1 to read IR2.
	 *        again_flag - [x1 - gain of 1, x4 - gain of 4, x16 - gain of 16, x64 - gain of 64]
	 * RETURN: 0 - success
	 *         -1 - error
	 */
	if (ALS_Mul) { reginfo[9] = reginfo[9] | 0x08; }
	else { reginfo[9] = reginfo[9] & 0xF7; }

	switch (again_flag)
	{
	case 1:
		reginfo[9] = reginfo[9] & 0xFC;
		break;
	case 4:
		reginfo[9] = reginfo[9] & 0xFD;
		reginfo[9] = reginfo[9] | 0x01;
		break;
	case 16:
		reginfo[9] = reginfo[9] & 0xFE;
		reginfo[9] = reginfo[9] | 0x02;
		break;
	case 64:
		reginfo[9] = reginfo[9] | 0x03;
		break;
	default:
		reginfo[9] = reginfo[9] & 0xFD;
		reginfo[9] = reginfo[9] | 0x01;
	}

	// TODO ADD ERROR CHECKING
	//set_reg(TCS3430_ADDR, CFG1_ADDR, reginfo[14]);
	ret = BSP_I2C1_WriteReg(TCS3430_ADDR, CFG1_ADDR, reginfo+9, 1);

	return ret;
}

int32_t set_cfg2(uint8_t *reginfo, uint8_t mode)
{
	/*
	 * FUNCTION: Change the gain to max
	 * ---------
	 * INPUT: reginfo[24] - current values of all registers
	 *        mode - If this bit is set to 1 and AGAIN in the CFG1 register is set to 11, then the 128x gain mode will be enabled.
	 * RETURN: 0 - success
	 *         -1 - error
	 */
	if(mode)
	{
		reginfo[21] |= 0x10;
	}
	else
	{
		reginfo[21] &= 0x04;
	}
	ret = BSP_I2C1_WriteReg(TCS3430_ADDR, CFG2_ADDR, reginfo+21, 1);
}

int32_t set_cfg3(uint8_t *reginfo, uint8_t mode, uint8_t sai)//Mode all flag bit
{
	/*
	 * FUNCTION:
	 * ---------
	 * INPUT: reginfo[24] - current values of all registers
	 *        mode - If this bit is set, all flag bits in the STATUS register will be reset whenever the STATUS register is read over I2C.
	 *        sai - Sleep After Interrupt. Power down the device at the end of the ALS cycle if an interrupt has been generated
	 * RETURN: 0 - success
	 *         -1 - error
	 */
	if(mode)
	{
		reginfo[22] |= 0x80;
	}
	else
	{
		reginfo[22] &= 0x7F;
	}

	if(sai)
	{
		reginfo[22] |= 0x10;
	}
	else
	{
		reginfo[22] &= 0xEF;
	}

	ret = BSP_I2C1_WriteReg(TCS3430_ADDR, CFG3_ADDR, reginfo+22, 1);

	return ret;
}

int32_t set_AutoZeroMode(uint8_t *reginfo, uint8_t mode, uint8_t AZ_ITERATION)
{
    /*
     * FUNCTION:
     * ---------
     * INPUT: reginfo[24] - current values of all registers, mode
     *        mode - 0: Always start at zero when searching the best offset value
		     1: Always start at the previous (offset_c) with the auto-zero mechanism
     *        AZ_ITERATION - Run autozero automatically every nth ALS iteration (0=never, 7Fh=only at first ALS cycle, n=every nth time)
     * RETURN: 0 - success
     *         -1 - error
     */
    if (mode)
    {
        reginfo[23] |= 0x80;
    }
    else
    {
        reginfo[23] &= 0x7F;
    }

    if(AZ_ITERATION == 0x7F)
    {
    	reginfo[23] |= 0x7F;
    }
    else
    {
    	reginfo[23] &= 0x80;
    }

    ret = BSP_I2C1_WriteReg(TCS3430_ADDR, AZ_CONFIG_ADDR, reginfo+23, 1);

    return ret; // Assuming 'ret' is a valid public variable accessible within the scope of this function
}

int32_t set_ALSInterrupt(uint8_t *reginfo, uint8_t AIEN, uint8_t ASIEN)
{
	/*
	 * FUNCTION:
	 * ---------
	 * INPUT: reginfo[24] - current values of all registers, mode
	 *        AIEN - 0: Writing '1' to this bit enables ALS interrupt.
						 1: Always start at the previous (offset_c) with the auto-zero mechanism
	 *        ASIEN - Writing '1' to this bit enables ASAT interrupt.
	 * RETURN: 0 - success
	 *         -1 - error
	 */
	if(AIEN)
	{
		reginfo[24] |= 0x10;
	}
	else
	{
		reginfo[24] &= 0xEF;
	}

	if(ASIEN)
	{
		reginfo[24] |= 0x80;
	}
	else
	{
		reginfo[24] &= 0x7F;
	}

	ret = BSP_I2C1_WriteReg(TCS3430_ADDR, INTENAB_ADDR, reginfo+24, 1);

	return ret;
}

int32_t enable_sensor(uint8_t *reginfo, uint8_t wait_flag, uint8_t als_flag)
{
	/*
	 * FUNCTION: Enable the wait or als and sensor
	 * ---------
	 * INPUT: fd - the file descriptor of the i2c device
	 *        reginfo[24] - current values of all registers
	 *        wait_flag - 1 to enable wait feature, 0 to disable wait feature
	 *        als_flag - 1 to enable als feature, 0 to disable wait feature
	 * RETURN: 0 - success
	 *         -1 - error
	 */
	if (wait_flag) {
		reginfo[0] = reginfo[0] | 0x08;
	} else {
		reginfo[0] = reginfo[0] & 0xF7;
	}

	if (als_flag) {
		reginfo[0] = reginfo[0] | 0x03;
	} else {
		reginfo[0] = reginfo[0] & 0xFC;
	}

	//set_reg(TCS3430_ADDR, ENABLE_ADDR, reginfo[0]);
	ret = BSP_I2C1_WriteReg(TCS3430_ADDR, ENABLE_ADDR, reginfo, 1);

	return ret;
}

int32_t disable_sensor(uint8_t *reginfo)
{
	/*
	 * FUNCTION: Disable the sesor
	 * ---------
	 * INPUT: fd - the file descriptor of the i2c device
	 * RETURN: 0 - success
	 */
	reginfo[0] = reginfo[0] & 0x00;
	ret = BSP_I2C1_WriteReg(TCS3430_ADDR, ENABLE_ADDR, reginfo, 1);
	return ret;
}

void softReset(uint8_t *reginfo)
{
 /*
  * FUNCTION: Change the enable register settings
  * ---------
  * INPUT: fd - the file descriptor of the i2c device
  * RETURN: 0 - success
  */
  disable_sensor(reginfo);
  set_atime(reginfo, 18);

  //setWaitTime(0);
  set_cfg0(reginfo, 0);

  //setALSGain(3);
  set_cfg1(reginfo, 0, 16);

  //setHighGAIN(false);
  set_cfg2(reginfo, 1);

  //setIntReadClear(false);
  //setSleepAfterInterrupt(false);
  set_cfg3(reginfo, 0, 0);

  //setAutoZeroMode(0);
  //setAutoZeroNTHIteration(0x7f);
  set_AutoZeroMode(reginfo, 0, 0x7F);

  //setALSInterrupt(false);
  //setALSSaturationInterrupt(false);
  set_ALSInterrupt(reginfo, 0, 0);
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


	//read Z
	ret = BSP_I2C1_ReadReg(TCS3430_ADDR, CHATAL_Z_ADDR, color_array, 2);
	//color_array[0] = buf[0];

	//read Y
	ret = BSP_I2C1_ReadReg(TCS3430_ADDR, CHATAL_Y_ADDR, color_array+2, 2);
	//color_array[1] = buf[0];

	//read IR
	ret = BSP_I2C1_ReadReg(TCS3430_ADDR, CHATAL_IR_ADDR, color_array+4, 2);

	//read X
	ret = BSP_I2C1_ReadReg(TCS3430_ADDR, CHATAL_X_ADDR, color_array+6, 2);

	return ret;
}

tcs3430_optics_val get_raw_XYZ(const tcs3430 *sensor) {

    tcs3430_optics_val calibed;

    if ((get_optics_data((uint8_t*)sensor->XYZinfo)) != 0) {
    	return calibed;     // if errors occur when reading from register, return empty calibed
    }

    uint16_t temp = 0;

    temp = sensor->XYZinfo[1];
    calibed.Z = temp<<8 | sensor->XYZinfo[0];

    temp = sensor->XYZinfo[3];
    calibed.Y = temp<<8 | sensor->XYZinfo[2];

    temp = sensor->XYZinfo[5];
    calibed.IR = temp<<8 | sensor->XYZinfo[4];

    temp = sensor->XYZinfo[7];
    calibed.X = temp<<8  | sensor->XYZinfo[6];

    calibed.CCT = 0;
    calibed.Lux = 0;
    /*calibed.IR = 0;*/

    return calibed;
}

void TCS3430_print_color(const tcs3430_optics_val *color_data) {
    /*
     * FUNCTION: Print the color from the processed data struct
     * ---------
     * INPUT: color_data - the struct that is storing caliberated color data
     * RETURN: void
     */

    /* formatting the data to left aligned, zero decimal, width of eight floats */
    printf( """\n\r|X\t|Y\t|Z\t|IR\t|LUX\t|CCT\t|\n\r|%d\t|%d\t|%d\t|%d\t|%d\t|%d\t|\r\n",
    		color_data->X, color_data->Y, color_data->Z, color_data->IR, color_data->Lux, color_data->CCT);//*/

	//printf("%x|%x|%x|",color_data->X, color_data->Y, color_data->Z );
	//fflush(stdout);
	/*
    write(stdout,color_data->X,2);
    write(stdout,color_data->Y,2);
    write(stdout,color_data->Z,2);*/
}

int32_t begin(tcs3430 *sensor_t)
{
	softReset(sensor_t->reginfo);
	ret = set_ALSInterrupt(sensor_t->reginfo, 0, 0);   //enable als interrupt
	ret = set_cfg3(sensor_t->reginfo,0,0); // enable als interrupt SC
	set_IRT_Cycle(sensor_t->reginfo,1); // multiple occurance of ALS interrupt
	Update_ALSThreshold(sensor_t);
	reset_Status(sensor_t->reginfo);
	BSP_I2C1_WriteReg(TCS3430_ADDR, WTIME_ADDR, &(sensor_t->reginfo[2]), 1);

	ret = enable_sensor(sensor_t->reginfo, 0, 1);
	ret = get_data(sensor_t->reginfo);//copy from sensor


	return ret;
}
int MinV(uint16_t x, uint16_t y) {
    return x < y ? x : y;
}
int MaxV(uint16_t x, uint16_t y) {
    return x < y ? x : y;
}

void Update_ALSThreshold(tcs3430 *sensor_t){
	XYZ_data = get_raw_XYZ(sensor_t);
	//TODO: incoperate the above into the following
	int temp =XYZ_data.Z;
	if (temp<Range){
		temp = Range;
	}else if ((temp+Range)>65525){
		temp = 65525-Range;
	}
	set_ALS_THR_LOW(sensor_t->reginfo,temp-Range);
	set_ALS_THR_HIGH(sensor_t->reginfo,temp+Range);
	printf("\n\rNew Low:%d New High:%d\n\r",temp-Range,temp+Range);
	reset_Status(sensor_t->reginfo);
	set_ALSInterrupt(sensor_t->reginfo, 1, 0);   //enable als interrupt
}



void Color_Init(void)
{
	BSP_I2C1_Init();
	begin(&sensor);
}
void Color_Process(void){

	XYZ_data = get_raw_XYZ(&sensor);
	TCS3430_print_color(&XYZ_data);

}

void set_ALS_THR_LOW(uint8_t *reginfo, uint16_t val)
{
	if(val > 65525)
	{
		return;
	}

	uint16_t value = val;

	uint8_t low = value;
	reginfo[3] = low;

	uint8_t high = value >> 8;
	reginfo[4] = high;

	ret = BSP_I2C1_WriteReg(TCS3430_ADDR, AILTL_ADDR, reginfo+3, 1);
	ret = BSP_I2C1_WriteReg(TCS3430_ADDR, AILTH_ADDR, reginfo+4, 1);


}

void set_ALS_THR_HIGH(uint8_t *reginfo, uint16_t val)
{
	if(val > 65525)
	{
		return;
	}
	uint16_t value = val;

	uint8_t low = value;
	reginfo[5] = low;

	uint8_t high = value >> 8;
	reginfo[6] = high;

	ret = BSP_I2C1_WriteReg(TCS3430_ADDR, AIHTL_ADDR, reginfo+5, 1);
	ret = BSP_I2C1_WriteReg(TCS3430_ADDR, AIHTH_ADDR, reginfo+6, 1);


}

void set_IRT_Cycle(uint8_t *reginfo, uint8_t cycle)
{
	if(cycle > 15)
	{
		cycle = 15;
	}
	reginfo[7] = cycle;
	ret = BSP_I2C1_WriteReg(TCS3430_ADDR, PERS_ADDR, reginfo+7, 1);


}

void TCS3430_print_reg(tcs3430 *sensor_t) {
	{

		int addr[] = {0x80, 0x81, 0x83, 0x84, 0x85, 0x86, 0x87, 0x8C,
					0x8D, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
					0x98, 0x99, 0x9A, 0x9B, 0x9F, 0xAB, 0xD6, 0xDD};


		for(int i = 0; i < 25; i++)
		{
			BSP_I2C1_ReadReg(TCS3430_ADDR, addr[i], &(sensor_t->reginfo[i]), 1);
			printf("%x---%d---%x\n\r",addr[i],i,(sensor_t->reginfo[i]));
			if(ret != BSP_ERROR_NONE) break;
		}
		ret = BSP_I2C1_WriteReg(TCS3430_ADDR, STATUS_ADDR, &(sensor_t->reginfo[12]), 0x10);

	}

}

int32_t reset_Status(uint8_t *reginfo)
{
	reginfo[12] = 0xFF;

	ret = BSP_I2C1_WriteReg(TCS3430_ADDR, STATUS_ADDR, reginfo+12, 1);

	return ret;
}

void colordiff (tcs3430_optics_val base, tcs3430_optics_val current, float * delta_X, float * delta_Y){
    //calculate cie color
    float base_x,base_y,curr_x,curr_y;
    base_x = (float)base.X/(float)(base.X+base.Y+base.Z)*10;
    base_y = (float)base.Y/(float)(base.X+base.Y+base.Z)*10;
    curr_x = (float)current.X/(float)(current.X+current.Y+current.Z)*10;
    curr_y = (float)current.Y/(float)(current.X+current.Y+current.Z)*10;


    *delta_X = curr_x - base_x;//calculate frame deltaX
    *delta_Y = curr_y - base_y;//calculate frame deltaY
}




