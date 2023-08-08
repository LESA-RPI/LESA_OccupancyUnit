/*
 * This is the code for processing data from the TMD3725. The main purpose is to correct the raw data from
 * the sensor and convert it into data that human can understand and analyze. Detailed function can be found
 * in the comments below.
 * 
 * Note:
 * - Detailed description for functions are in the source code file.
 * 
 * Authors: You Wu, Tyler Chan
 * 
 * History:
 * 11/04/2022 - Tyler Chan:
 *      Modified to run on Expressif's ESP-IDF
 *      Modified print_color() to use ESP_LOGI()
 * 10/27/2022 - Tyler Chan:
 *      Modified to be part of the Color Sensor class instead of its own
 *      Functions can now be accessed from a ColorSensor object
 * 10/19/2022 - Tyler Chan:
 *      Ported to work on Arduino IDE
 *      Modified to be a C++ class and use the new I2C and DataGet classes
 * 07/27/2022 - You Wu:
 *      Creation
 */


extern "C" {
    #include <stdint.h>
    #include <stdio.h>
}

#include "color_sensor.h"
static const char *TAG = "coloralg"; // Used for logging messages in this file

#define C_coef      0.16    // clear channel coefficient
#define R_coef      -0.04   
#define G_coef      0.16
#define B_coef      -0.29
#define DGF         682.85
#define CT_coef     4520    // color temperature coefficient
#define CT_offset   1804    // color temperature offset

/*combine color flag*/
#define C           1
#define R           2
#define G           3
#define B           4

optics_val ColorSensor::get_calib_color(const uint8_t reginfo[]) {
    /*
     * FUNCTION: Read color data from sensor and caliberate them to usable data
     * ---------
     * INPUT: fd - the file descriptor of the i2c device
     *        reginfo[35] - current values of all registers
     * RETURN: calibed - return a struct that contains all the caliberated data
     *         empty struct calibed - return empty calibed if errors occur reading from register
     */

    uint8_t colorarray[9];
    optics_val calibed;

    if ((get_optics_data(colorarray)) == -1) {
        return calibed;     // if errors occur when reading from register, return empty calibed
    }

    calibed = calib_color(colorarray, reginfo);     // caliberating ...
    
    return calibed;
}

optics_val ColorSensor::calib_color(const uint8_t colorarray[], const uint8_t reginfo[]) {
    /*
     * FUNCTION: Caliberate color data
     * ---------
     * INPUT: fd - the file descriptor of the i2c device
     *        colorarray[9] - values read only from color data registers
     *        reginfo[35] - current values of all registers
     * RETURN: calibed - return a struct that contains all the caliberated data
     */

    optics_val calibed;
    int rawr, rawg, rawb, rawc, Again;
    float Atime;

    rawr = combine_color(colorarray, R);
    rawg = combine_color(colorarray, G);
    rawb = combine_color(colorarray, B);
    rawc = combine_color(colorarray, C);

    Atime = 2.81 * (reginfo[1] + 1);            // calculate the integration time in ms
    if (reginfo[11] & 0x02) {
        Atime = Atime * 12;                     // account for wlong case
    }
    Again = power(2.0, (reginfo[14] & 0x03) * 2);   // calculate the gain in 1x, 4x, 16x, 64x
    if (!(reginfo[27] & 0x04)) {
        Again = Again/2;                        // account for wider range of gain
    }

    calibed.IR = ((rawr + rawg + rawb) - rawc)/2;
    calibed.CPL = (Again * Atime)/DGF;
    calibed.Lux = ((C_coef * rawc) + (R_coef * rawr) + (G_coef * rawg) + (B_coef * rawb))/calibed.CPL;  // equation for lux

    calibed.red = rawr - calibed.IR;            // caliberated red value
    calibed.green = rawg - calibed.IR;          // caliberated green value
    calibed.blue = rawb - calibed.IR;           // caliberated blue value
    calibed.clear = rawc - calibed.IR;          // caliberated clear value

    calibed.CCT = (CT_coef * (calibed.blue/calibed.red)) + CT_offset;   // equation for color temperature

    return calibed;
}

int ColorSensor::combine_color(const uint8_t color_array[], uint8_t flag) {
    /*
     * FUNCTION: combine the seperate high bit and low bit color data into 2 bytes color data
     * ---------
     * INPUT: color_array[9] - the array that stored color value of registers
     *        flag - choose which color to combined. [options: C - clear, R - red, G - green, B - blue]
     * RETURN: tempcolor - the combined color value
     */

    int tempcolor = 0;

    switch(flag) {
        case 1:
            tempcolor = (color_array[1] << 8) + color_array[0];
            break;
        case 2:
            tempcolor = (color_array[3] << 8) + color_array[2];
            break;
        case 3:
            tempcolor = (color_array[5] << 8) + color_array[4];
            break;
        case 4:
            tempcolor = (color_array[7] << 8) + color_array[6];
            break;
        default:
            tempcolor = -1;
    }

    return tempcolor;
}

void ColorSensor::print_color(const optics_val color_data) {
    /*
     * FUNCTION: Print the color from the processed data struct
     * ---------
     * INPUT: color_data - the struct that is storing caliberated color data
     * RETURN: void
     */

    /* formatting the data to left aligned, zero decimal, width of eight floats */

    return;
}

float ColorSensor::power(float base, int power) {
    /*
     * FUNCTION: A simple function to calculate positive power
     * ---------
     * INPUT: base - the base number 
     *        power - a positive number represents the power
     * RETURN: result - the result of the power
     */

    float result = 1;

    for (int i=0; i<power; i++) {
        result = result * base;
    }

    return result;
}