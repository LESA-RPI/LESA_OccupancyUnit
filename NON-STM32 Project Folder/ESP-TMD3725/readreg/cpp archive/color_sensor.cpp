/*
 *  This class is used to contain all the functions pertaining to the
 *    color sensor and the global object.
 *
 *  Author: Tyler Chan
 *
 *  History:
 *  12/04/2022 - Tyler Chan:
 *    Modified to run on Expressif's ESP-IDF
 *    Moved constructor and destructor to private (ensures only one object exists)
 *  11/02/2022 - Tyler Chan:
 *    Added functions (get_data_opt() and get_optics_data_opt()) that uses the
 *      optimized I2C functions
 *    Modified functions to use optimized functions
 *  10/27/2022 - Tyler Chan:
 *    Creation
 */

#include "color_sensor.h"
static const char *TAG = "ColorSensor"; // Used for logging messages in this file

ColorSensor ColorSensor::begin() {
    /*
     * RETURN:
     *      a singleton object created with the default parameters
     */
    static ColorSensor sensor = ColorSensor();
    sensor.get_data();
    sensor.set_atime(128);
    sensor.set_cfg1(0, 4);
    sensor.enable_sensor(0, 0, 1);
    return sensor;
}

#define SEPARATECOLORS 0
void ColorSensor::detect_color_shift() {
    optics_val sincolor;

    float redlist[50] = {0}, greenlist[50] = {0}, bluelist[50] = {0};
    float SD;
    float redSD, greenSD, blueSD;
    int index = 0;

    while (1) {
        sincolor = ColorSensor::get_calib_color(reginfo); // get the latest color information from sensor
        redlist[index] = sincolor.red;                    // add color into the analyzing array, or replace
        greenlist[index] = sincolor.green;                // the old data with the new one
        bluelist[index] = sincolor.blue;

// calculate standard deviation from all three color arrays
#if SEPARATECOLORS
        redSD = ColorSensor::calcSD(redlist);
        greenSD = ColorSensor::calcSD(greenlist);
        blueSD = ColorSensor::calcSD(bluelist);
        if (redSD >= CSTHRESHOLD || greenSD >= CSTHRESHOLD || blueSD >= CSTHRESHOLD) {
            Serial.printf("Color shift! %.2f, %.2f, %.2f\n", redSD, greenSD, blueSD);
            print_color(get_calib_color(reginfo));
        }
#else
        SD = (ColorSensor::calcSD(redlist) + ColorSensor::calcSD(greenlist) + ColorSensor::calcSD(bluelist)) / 3;
        if (SD >= CSTHRESHOLD) {
            print_color(get_calib_color(reginfo));
        }
#endif

        index = index + 1;
        if (index >= 50) {
            index = 0;
        }
    }

    return;
}