//client model is the network provisioner/gateway node

//include all libraries and headers

#include "sensor_client.h"
#include "data_upload.h"
#include "conversions.h"

void app_main(){
    uint8_t occ_sens_server_ble_msg[19];
    char* query;
    //initialize uart

    //initialize ble
    occ_sens_ble_init();
    printf("BLE MESH INITIALIZED\n");
    //initialize wifi 
    initialise_wifi();//BLE NEEDS TO INITIALIZE CAUASE IT USES "nvs_flash_init" WHICH WIFI NEEDS.
    printf("WIFI CONNECTED\n");
    
    //main loop
    while(true){
        vTaskDelay(1000); //sleep for 1 second each iteration
        //uart_read_bytes
            //if bad status reread bytes
        
        //tof_get_occupants
        //tof_get_data

        //for all sensors in network (not figured out)
            //get message from BLE server
            occ_sens_client_ble_msg_get(occ_sens_server_ble_msg);
            // if no BLE nodes, skip rest of code
            if(occ_sens_server_ble_msg[0] == 0 && occ_sens_server_ble_msg[1] == 0)
                continue;
            // otherwise convert to occupancy message
            query = byteStreamToQuery(&occ_sens_server_ble_msg);
            //printf(query);
            // printf("Data:");
            // for(int i = 0; i < 18; i++){
            //     printf(" %02x", occ_sens_server_ble_msg[i]);
            // }
            // printf("\n");            
            // upload query to database (wifi)
            xTaskCreate(&http_test_task, "http_test_task", 8192, (void *) query, 5, NULL);
            //xTaskCreate(&http_test_task, "http_test_task", 8192, (void *) query, 5, NULL);

        //if no color sensor change -> sleep?
    }
}