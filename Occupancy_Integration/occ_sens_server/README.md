| Supported Targets | ESP32 |
| ----------------- | ----- |

ESP BLE Mesh Sensor Server Example
==================================

For description of this example please refer to [ESP BLE Mesh Sensor Client Example](../sensor_client/README.md)

==================================

For UART transmit and receive, TX is Pin 17, RX is Pin 16, both of them use UART Port 2. 
It would receive in total 512 bit data, into 4 arrays: Distance, Status, Ambient, Signal in a custom struct. 
It would transfer a pre-defined wake up signal to STM32.