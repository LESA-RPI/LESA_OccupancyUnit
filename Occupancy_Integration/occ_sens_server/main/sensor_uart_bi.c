/* UART asynchronous example, that uses separate RX and TX tasks

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "sensor_uart_bi.h"

static const int RX_BUF_SIZE = 1000;
uint16_t tofout[256]; // tof data ouput array
uint8_t wakeup[] = {88}; // the wake up signal, 88 is only a random number, no specific meaning
tof_data_struct tofdatareturn; // struct to pass to main.c

#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)
#define UART_PORT_NUM (UART_NUM_2) // not default

void init_uart(void) {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    // We won't use a buffer for sending data.
    uart_driver_install(UART_PORT_NUM, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_PORT_NUM, &uart_config);
    uart_set_pin(UART_PORT_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

int sendData(const char* logName, const uint8_t* data)
{
    const int len = 1;
    const int txBytes = uart_write_bytes(UART_PORT_NUM, data, len);
    ESP_LOGI(logName, "Wrote %d bytes", txBytes);
    return txBytes;
}

void tx_task(void)
{
    static const char *TX_TASK_TAG = "TX_TASK";
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    sendData(TX_TASK_TAG, wakeup); // send wake up signal for future use

}

tof_data_struct rx_task(void)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint16_t* data = (uint16_t*) malloc(RX_BUF_SIZE+1); // data buffer to receive UART
    const int len = uart_read_bytes(UART_PORT_NUM, data, (RX_BUF_SIZE - 1), 1150 / portTICK_RATE_MS);
    if (len)
    {
        data[len] = 0;
        memcpy(tofout, data, 512); // move received data to another array to modify
        int num = 0;
        for (int i = 0; i < 32; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                // move data into struct
                // printf("%d ", tofout[num]);
                num++;
                if (i < 8)
                    {tofdatareturn.Distance[num-1] = tofout[num-1];}
                else if (i < 16)
                    {tofdatareturn.Status[num-1-64] = tofout[num-1];}
                else if (i < 24)
                    {tofdatareturn.Ambient[num-1-64*2] = tofout[num-1];}
                else if (i < 32)
                    {tofdatareturn.Signal[num-1-64*3] = tofout[num-1];}
            }
            printf("\r\n");
            // if (num == 64 || num == 128 || num == 192)
            // {
            //     printf("\n\n");
            // }
        }
        printf("receive end\r\n");
    }

    free(data);
    return tofdatareturn;
}
