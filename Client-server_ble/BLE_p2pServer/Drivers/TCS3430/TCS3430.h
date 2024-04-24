
#include "stm32wbxx_nucleo_bus.h"
#include "app_tof.h"
#include "stm32wbxx_hal.h"

#include "stm32wbxx_nucleo.h"


#include <stdint.h>
#include <stdio.h>

#define Range            1
#define TCS3430_ADDR  	(0x39 << 1)

#define ENABLE_ADDR     (0x80)	// R/W    0
#define ATIME_ADDR      (0x81)	// R/W    1
#define WTIME_ADDR      (0x83)	// R/W    2
#define AILTL_ADDR      (0x84)	// R/W    3
#define AILTH_ADDR      (0x85)	// R/W    4
#define AIHTL_ADDR      (0x86)	// R/W    5
#define AIHTH_ADDR      (0x87)	// R/W    6
#define PERS_ADDR       (0x8C)	// R/W    7
#define CFG0_ADDR       (0x8D)	// R/W    8
#define CFG1_ADDR       (0x90)	// R/W    9
#define REVID_ADDR      (0x91)	// R      10
#define ID_ADDR         (0x92)	// R      11
#define STATUS_ADDR     (0x93)	// R      12
#define CHATAL_Z_ADDR   (0x94)	// R      13
#define CHATAH_Z_ADDR   (0x95)	// R      14
#define CHATAL_Y_ADDR   (0x96)	// R      15
#define CHATAH_Y_ADDR   (0x97)	// R      16
#define CHATAL_IR_ADDR  (0x98)	// R      17
#define CHATAH_IR_ADDR  (0x99)	// R      18
#define CHATAL_X_ADDR   (0x9A)	// R      19
#define CHATAH_X_ADDR   (0x9B)	// R      20
#define CFG2_ADDR       (0x9F)	// R/W    21
#define CFG3_ADDR       (0xAB)	// R/W    22
#define AZ_CONFIG_ADDR  (0xD6)	// R/W    23
#define INTENAB_ADDR    (0xDD)	// R/W    24

typedef struct tcs3430_optics_val
{
	 // struct that stored the processed data that can be used in later analysis
	 uint16_t X;
	 uint16_t Y;
	 uint16_t Z;
	 uint16_t IR;
	 uint16_t CPL; // Not necessary
	 uint16_t Lux; // Not necessary
	 uint16_t CCT; // Not necessary
} tcs3430_optics_val;

typedef struct TCS3430
{
	uint8_t reginfo[24];
	uint8_t XYZinfo[8];
}tcs3430;


int32_t get_data(uint8_t *reginfo);

int32_t set_atime(uint8_t *reginfo, int cycle_No);

int32_t set_cfg0(uint8_t *reginfo, uint8_t mode);

int32_t set_cfg1(uint8_t *reginfo, uint8_t IRtoG_flag, uint8_t again_flag);

int32_t set_cfg2(uint8_t *reginfo, uint8_t mode);

int32_t set_cfg3(uint8_t *reginfo, uint8_t mode, uint8_t sai);

int32_t set_AutoZeroMode(uint8_t *reginfo, uint8_t mode, uint8_t AZ_ITERATION);

int32_t set_ALSInterrupt(uint8_t *reginfo, uint8_t AIEN, uint8_t ASIEN);

int32_t enable_sensor(uint8_t *reginfo, uint8_t wait_flag, uint8_t als_flag);

int32_t disable_sensor(uint8_t *reginfo);

void softReset(uint8_t *reginfo);

int32_t get_optics_data(uint8_t *color_array);

tcs3430_optics_val get_raw_XYZ(const tcs3430 *sensor);

void TCS3430_print_color(const tcs3430_optics_val *XYZ_data);

void Color_Init(void);

void Color_Process(void);

void Update_ALSThreshold(tcs3430 *sensor_t);

void set_ALS_THR_LOW(uint8_t *reginfo, uint16_t val);

void set_ALS_THR_HIGH(uint8_t *reginfo, uint16_t val);

void set_IRT_Cycle(uint8_t *reginfo, uint8_t cycle);

void TCS3430_print_reg(tcs3430 *sensor_t);

void colordiff (tcs3430_optics_val base, tcs3430_optics_val current, float * delta_X, float * delta_Y);

int32_t reset_Status(uint8_t *reginfo);
