/*
 * This is the code for fetching data from all the registers of TMD3725. get_data function can be used once at 
 * the initialization in order to keep track of all the register value of the sensor. get_optics_data is used at 
 * every integration cycle to get the raw color values.
 * 
 * Note:
 * - Detailed description for functions are in the source code file.
 * 
 * Authors: You Wu, Tyler Chan
 *
 * History:
 * 11/04/2022 - Tyler Chan:
 *      Modified to run on Expressif's ESP-IDF
 *      Updated to use the modified I2C class
 *      Removed "get_data_opt()", obsolete
 *      Removed "get_optics_data_opt()", obsolete
 * 11/02/2022 - Tyler Chan:
 *      Created "get_data_opt()" that uses the optimized i2c functions
 *      Created "get_optics_data_opt()" that uses the optimized i2c functions
 *      Fixed camel case to snake case 
 * 10/27/2022 - Tyler Chan:
 *      Modified to be part of the Color Sensor class instead of its own
 *      Functions can now be accessed from a ColorSensor object
 * 10/19/2022 - Tyler Chan:
 *      Modified to be a C++ class and use the new I2C class to run on Arduino IDE
 * 10/15/2022 - Tyler Chan:
 *      Ported You Wu's work
 * 07/26/2022 - You Wu:
 *      Creation
 */

extern "C" {
    #include <stdio.h>
    #include <stdint.h>
}
#include "i2c_esp32.h"
#include "color_sensor.h"
#include "registers.h"

uint8_t ColorSensor::get_data(uint8_t reginfo[]) {
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

uint8_t ColorSensor::get_optics_data(uint8_t color_array[]) {
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

int ColorSensor::get_raw_clear() {
    /*
     * FUNCTION: Read the raw clear channel value for auto integration time adjustment
     * ---------
     * RETURN: the full byte raw clear channel value
     */

    // TODO ADD ERROR CHECKING
    uint8_t data[2] = {};
    get_reg(TMD3725_ADDR, CDATAL_ADDR, data, 2) ;

    return (data[1] << 8) + data[0];
}
