This folder holds files that will be used to store files that would be used by both occ_sens_server and occ_sens_client.

However, it was revealed that the ESP32 would not have enough memory to store everything the occ_sens_client would use.

So as of now, the contents of this folder only appear in occ_sens_server.

Contents include:
- conversions.c and .h :  encodes/decodes messages between hex bytes and other data types for BLE.
- intergratedTrackingAlgorithm.c and .h:  algorithm to track occupants using ToF data.
- uart.c and .h:  old verison of UART communication.  Newer verison is sensor_uart_bi in occ_sens_server.
