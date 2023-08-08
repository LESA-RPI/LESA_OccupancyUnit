#ifndef _COLOR_SENSOR_H
#define _COLOR_SENSOR_H

#include <stdint.h>

#define CSTHRESHOLD 0.5       // set the sensitivity for sensing color shift, higher the less sensitive

typedef struct optics_val {
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

class ColorSensor {
  private:
    uint8_t reginfo[35];
    uint8_t colorinfo[9];
    ColorSensor() {}

  public:
    // // Implemented in color_sensor.cpp
    static ColorSensor begin();
    void detect_color_shift();

    // // Implemented in data_get.cpp
    uint8_t get_data(uint8_t reginfo[]);
    uint8_t get_optics_data(uint8_t color_array[]);
    int get_raw_clear();

    // // Implemented in color_alg.cpp
    optics_val get_calib_color(const uint8_t reginfo[]);
    optics_val calib_color(const uint8_t colorarray[], const uint8_t reginfo[]);
    int combine_color(const uint8_t color_array[], uint8_t flag);
    void print_color(const optics_val color_data);
    float power(float base, int power);
    
    // // Implemented in setting_ctrl.cpp
    void auto_inttime_adj(uint8_t reginfo[]);
    uint8_t enable_sensor(uint8_t reginfo[], uint8_t wait_flag, uint8_t prox_flag, uint8_t als_flag);
    uint8_t set_atime(uint8_t reginfo[], int cycle_No);
    uint8_t set_cfg1(uint8_t reginfo[], uint8_t IRtoG_flag, uint8_t again_flag);
    float calcMean(float data[]);
    float calcSD(float data[]);

    inline uint8_t get_data() {
        return get_data(reginfo);
    }
    inline uint8_t get_optics_data() {
        return get_optics_data(colorinfo);
    }
    inline uint8_t enable_sensor(uint8_t wait_flag, uint8_t prox_flag, uint8_t als_flag) {
        return enable_sensor(reginfo, wait_flag, prox_flag, als_flag);
    }
    inline uint8_t set_atime(int cycle_No) {
        return set_atime(reginfo, cycle_No);
    }
    inline uint8_t set_cfg1(uint8_t IRtoG_flag, uint8_t again_flag) {
        return set_cfg1(reginfo, IRtoG_flag, again_flag);
    }
    inline void auto_inttime_adj() {
        auto_inttime_adj(reginfo);
    }
};

#endif