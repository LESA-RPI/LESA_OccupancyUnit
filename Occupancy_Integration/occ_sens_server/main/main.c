//TODO: add real ID configuration, fix color sensor input

//server model is the individual sensor

//include all libraries and headers
#include "sensor_server.h"
// #include "sensor_uart.h"
#include "sensor_uart_bi.h"
#include "integratedTrackingAlgorithm.h"
#include <stdbool.h>
#include "time.h"
#include "conversions.h"


void app_main(void){
    time_t current_time;
    struct tm* local_time;
    char time_str[20];
    // initialize uart
    init_uart();
    // testBytes: bldg 9, room 1024, sensor 30, time 09:40:00, pos (5,6), 180 degrees, moving 1.2 m/s, 180 cm tall, CIE (0.3,0.3)
    //uint8_t testBytes[19] = {0x09,0x04,0x00,0x1e,0x07,0xe7,0x04,0x1b,0x09,0x28,0x00,0x56,0x12,0x7f,0xff,0xb4,0xb4,0x4c,0x4c};
    uint8_t bleMessage[19], mockIDs[4] = {9, 0x04, 0x00, 30}; //bldg 9, room 1024, sensor 30
    float mockCIE[2] = {0.42, 0.69}; //mock CIE data

    tof_data_struct tofdata;
    // tof_data_struct tofdata_previous;
    // initialize ble
    occ_sens_server_ble_init();
    //int control[8][8]
    int control[8][8] = {
    {3092,2990,3004,3002,3038,3011,3072,1792},
    {3027,2999,2981,3034,3040,3071,3010,3090},
    {3010,3015,3011,3022,3028,3034,3014,3061},
    {2982,2990,3033,3042,3007,3050,3029,3077},
    {2993,3041,3010,3020,3023,3039,3047,3055},
    {2937,2975,3015,3022,3058,3087,3026,3051},
    {3018,3007,3040,3065,3058,3033,3049,3033},
    {3031,3031,3031,3050,3057,3107,3107,5000}};

    int previousReading[8][8];
    float occupantData[5];
    for(int i = 0; i<8;i++){
        for(int j = 0; j<8; j++){
            previousReading[i][j] = control[i][j];
        }
    }
    
    // main loop
    while(true){
        tofdata = rx_task(); // receive data from UART and put into the struct
        //tx_task(); // send wake up signal for future use
        time_t current_time = time(NULL); //Gets current time.
        local_time = localtime(&current_time);//converts to local time format
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time);
        printf("Current time is %s",time_str);

        // if bad status reread bytes
        for(int i = 0; i < 64; i++){
            printf("num: %d, distance: %d, status: %d, ambient: %d, signal: %d\r\n"
                , i, tofdata.Distance[i], tofdata.Status[i], tofdata.Ambient[i], tofdata.Signal[i]);

        }
        // if (tofdata_previous.Distance[0] == tofdata.Distance[0] && tofdata_previous.Distance[16] == tofdata.Distance[16] && tofdata_previous.Distance[32] == tofdata.Distance[32] && tofdata_previous.Distance[48] == tofdata.Distance[48] && tofdata_previous.Distance[63] == tofdata.Distance[63])
        // {
        //     printf("Warning: Possible same ToF data as before!!!\r\n");
        // }
        
        //tof_get_data
        int count = 0;
        int currentReading[8][8];
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                currentReading[i][j] = tofdata.Distance[count];
                count++;
            }
        }
        exampleRun(occupantData, control, previousReading, currentReading); //FIX HERE
        memcpy(previousReading,currentReading,64);
        //previousReading = currentReading;
        //tofdata_previous = tofdata;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                previousReading[i][j] = currentReading[i][j];
            }
        }

        //convert_data_to_bytes
        dataToByteStream(&bleMessage, mockIDs, time_str, occupantData, mockCIE);
        //ble_data_set
        occ_sens_server_ble_msg_set(bleMessage);

        //if no color sensor change -> sleep?

    }
}