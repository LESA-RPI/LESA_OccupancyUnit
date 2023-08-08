/*
 * This is the code for setting some of the control parameters we usually setup before 
 * integration, such as gain and integration time. More functions will be added to this
 * file to account for more advanced settings. 
 * 
 * Note:
 * - the integration adjustment algorithm implements a bound to bound control method 
 *   instead of PID control, since we don't have a strictly desired integration time
 * 
 * Author: You Wu, Tyler Chan
 * 
 * History: 
 * 12/04/2022 - Tyler Chan:
 *      Modified to run on Expressif's ESP-IDF
 *      Updated to use the modified I2C class
 *      TODO auto_inttime_adj() needs to be fixed
 * 11/28/2022 - Tyler Chan:
 *      Added calcMean() to calculate the mean
 *      Modified calcSD() to use calcMean()
 * 11/16/2022 - Tyler Chan:
 *      Added calcSD() to calculate the stdev for detecting color shifts
 * 11/02/2022 - Tyler Chan:
 *      Updated auto_inttime_adj() to a newer version found on You Wu's local machine
 *      Fixed camel case to snake case 
 * 10/27/2022 - Tyler Chan:
 *      Ported to work on Arduino IDE
 *      Modified to be part of the Color Sensor class instead of its own
 *      Functions can now be accessed from a ColorSensor object
 * 08/02/2022 - You Wu:
 *      Creation
 */

extern "C" {
    #include <stdint.h>
    #include <stdlib.h>
    #include <math.h>
}

#include "color_sensor.h"
#include "registers.h"
#include "i2c_esp32.h"

#define AUTOINTTIMETEST_FLAG 1      //flag for test prints

// Gain setting
#define x1 1
#define x4 4
#define x16 16
#define x64 64

void ColorSensor::auto_inttime_adj(uint8_t reginfo[]) {
    /* DOES NOT WORK ON ESP-IDF, millis is not defined (commented out to compile)
     * FUNCTION: Adjust the integration time automatically once started in a thread
     * ---------
     * INPUT: fd - the file descriptor of the i2c device
     *        reginfo[35] - current values of all registers
     * RETURN: void
     */
    
    int* max_als_limit = (int *) calloc(1, sizeof(int));
    int* uplimit = (int *) calloc(1, sizeof(int));
    int* lowlimit = (int *) calloc(1, sizeof(int));
    optics_val testread;
    int time_before = 0;

    while (1) {
        if ((reginfo[1] >= 63) && (reginfo[1] <= 255)) {
            *max_als_limit = 65535;
        }
        else {
            *max_als_limit = ((reginfo[1] + 1) * 1024)-1;     // read the current integration time and set the 
        }
        *uplimit = 0.8*(*max_als_limit);            // limit for checking integration time dynamically
        *lowlimit = 0.2*(*max_als_limit);

        
        // Doesn't work on ESP-IDF, millis is not defined
        // if (AUTOINTTIMETEST_FLAG) {                     // some testing print for this function, in read application
        //     if (millis() > time_before + 100) {
        //     testread = get_calib_color(reginfo);    // the flag can be change to 0 to stop the printing, the flag
        //     print_color(testread);                      // is in header file
        //     printf("Current raw Clear value is: %d\n", get_raw_clear());
        //     printf("Current ALS max is: %u (%d)\n", *max_als_limit, reginfo[1]);
        //     time_before = millis();
        //     }
        // }

        if ((get_raw_clear() >= *uplimit) && (get_raw_clear() <= *max_als_limit)) {   // if value is on the upper bound, then decrease integration time
            reginfo[1] = reginfo[1] - 5;
        }
        else if ((get_raw_clear() <= *lowlimit) && (get_raw_clear() >= 0)) {          // if value is on the lower bound, then increase integration time
            reginfo[1] = reginfo[1] + 5;
        }

        if (reginfo[1] <= 0) {
            reginfo[1] = 0;
        }
        if (reginfo[1] >= 255) {
            reginfo[1] = 255;
        }
        set_reg(TMD3725_ADDR, ATIME_ADDR, reginfo[1]);
    }
    
    return;
}

uint8_t ColorSensor::enable_sensor(uint8_t reginfo[], uint8_t wait_flag, uint8_t prox_flag, uint8_t als_flag) {
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

    if (wait_flag) { reginfo[0] = reginfo[0] | 0x08; }
    else { reginfo[0] = reginfo[0] & 0xF7; }

    if (prox_flag) { reginfo[0] = reginfo[0] | 0x04; }
    else { reginfo[0] = reginfo[0] & 0xFB; }

    if (als_flag) { reginfo[0] = reginfo[0] | 0x03; }
    else { reginfo[0] = reginfo[0] & 0xFC; }

    // TODO ADD ERROR CHECKING
    set_reg(TMD3725_ADDR, ENABLE_ADDR, reginfo[0]);
    return 0;
}

uint8_t ColorSensor::set_atime(uint8_t reginfo[], int cycle_No) {
    /*
     * FUNCTION: Change the integration time
     * ---------
     * INPUT: fd - the file descriptor of the i2c device
     *        reginfo[35] - current values of all registers
     *        cycle_No - integration cycle numbers, must be between 1-256
     * RETURN: 0 - success
     *         -1 - error
     */

    if ( (cycle_No <= 256) && (cycle_No >= 1) ) {
        reginfo[1] = cycle_No - 1;
    }
    else{ return -1; }

    // TODO ADD ERROR CHECKING
    set_reg(TMD3725_ADDR, ATIME_ADDR, reginfo[1]);
    return 0;
}

uint8_t ColorSensor::set_cfg1(uint8_t reginfo[], uint8_t IRtoG_flag, uint8_t again_flag) {
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

    switch (again_flag) {
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
    set_reg(TMD3725_ADDR, CFG1_ADDR, reginfo[14]);
    return 0;
}

float ColorSensor::calcMean(float data[]) {
    /*
     * FUNCTION: Calculate the mean of certain data array
     * ---------
     * INPUT: data[50] - the data set need has only 50 numbers
     * RETURN: mean of the data set
     */

    float sum = 0.0; 
    for(int i = 0; i < 50; i++) {
        sum += data[i];
    }
    return sum/50;
}

float ColorSensor::calcSD(float data[]) {
    /*
     * FUNCTION: Calculate the standard deviation of certain data array
     * ---------
     * INPUT: data[50] - the data set need has only 50 numbers
     * RETURN: standard deviation of the data set
     */

    float mean = calcMean(data);
    double SD = 0.0;
    for (int i = 0; i < 50; i++) {
        SD += pow(data[i] - mean, 2);
    }
    return sqrt(SD / 50.0);
}
