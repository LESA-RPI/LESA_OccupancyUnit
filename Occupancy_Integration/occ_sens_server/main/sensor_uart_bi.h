

#ifndef _UARTBI_H_
#define _UARTBI_H_



#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"


typedef struct // struct for saving tof data
{
  uint8_t Status[64];
  uint16_t Distance[64];
  uint8_t Ambient[64];
  uint8_t Signal[64];
} tof_data_struct;

void init_uart(void);
int sendData(const char* logName, const uint8_t* data);
void tx_task(void);
tof_data_struct rx_task(void);

#endif
