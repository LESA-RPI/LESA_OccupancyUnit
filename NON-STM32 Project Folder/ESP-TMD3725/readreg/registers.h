/*
 * This is the file that has all the register addresses for TMD3725 color sensor.
 *
 * Note:
 * - The read and write ability of the register is commented
 * - Details about registers can be checked in the datasheet
 *
 * Authors: You Wu, Tyler Chan
 * 
 * History:
 * 11/02/2022 - Tyler Chan:
 *      Added comments for the index values for all the register addresses
 *          in the reginfo array
 * 10/27/2022 - Tyler Chan:
 *      Added TMD3725_ADDR
 * 07/26/2022 - You Wu:
 *      Creation
 */

#ifndef _REGISTERS_H
#define _REGISTERS_H

#define ENABLE_ADDR     0x80	// R/W    0
#define ATIME_ADDR      0x81	// R/W    1
#define PTIME_ADDR      0x82	// R/W    2
#define WTIME_ADDR      0x83	// R/W    3
#define AILTL_ADDR      0x84	// R/W    4
#define AILTH_ADDR      0x85	// R/W    5
#define AIHTL_ADDR      0x86	// R/W    6
#define AIHTH_ADDR      0x87	// R/W    7
#define PILT_ADDR       0x88	// R/W    8
#define PIHT_ADDR       0x8A	// R/W    9
#define PERS_ADDR       0x8C	// R/W    10
#define CFG0_ADDR       0x8D	// R/W    11
#define PCFG0_ADDR      0x8E	// R/W    12
#define PCFG1_ADDR      0x8F	// R/W    13
#define CFG1_ADDR       0x90	// R/W    14
#define REVID_ADDR      0x91	// R      15
#define ID_ADDR         0x92	// R      16
#define STATUS_ADDR     0x93	// R/SC   17
#define CDATAL_ADDR     0x94	// R      18
#define CDATAH_ADDR     0x95	// R      19
#define RDATAL_ADDR     0x96	// R      20
#define RDATAH_ADDR     0x97	// R      21
#define GDATAL_ADDR     0x98	// R      22
#define GDATAH_ADDR     0x99	// R      23
#define BDATAL_ADDR     0x9A	// R      24
#define BDATAH_ADDR     0x9B	// R      25
#define PDATA_ADDR      0x9C	// R      26
#define CFG2_ADDR       0x9F	// R/W    27
#define CFG3_ADDR       0xAB	// R/W    28
#define POFFSETL_ADDR   0xC0	// R/W    29
#define POFFSETH_ADDR   0xC1	// R/W    30
#define CALIB_ADDR      0xD7	// R/W    31
#define CALIBCFG_ADDR   0xD9	// R/W    32
#define CALIBSTAT_ADDR  0xDC	// R/W    33
#define INTENAB_ADDR    0xDD	// R/W    34

#define TMD3725_ADDR 0x39

#endif