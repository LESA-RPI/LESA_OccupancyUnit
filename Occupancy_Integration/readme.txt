Note that SVN has been set to global-ignore the build folder to save repo space.

In this directory is the Spring 2023 teams effort to integrate the following subsystems into two occupancy sensor models:
* BLE Mesh Communication
* WiFi Communication to PHP Server
* UART Communication between ESP and STM boards for ToF sensor reading
* ToF Data Processing Algorithm
* Color Sensor Wake-Up Code

The two main models are as follows:
* occ_sens_server: Sensors running this model only have BLE mesh communication enabled. There is no need for them to have WiFi as they will forward processed sensor data to the client node.
* occ_sens_client: Sensors running this model will have both BLE and WiFi communication enabled. This node is called the gateway/provisioner node, and will upload the local BLE sensor mesh data to the PHP server over WiFi to be logged in an SQL database.

The client/server models can further be understoood by looking at the sensor_client/sensor_server software in the ESP IDFs C:\Users\USERNAME\esp\esp-idf\examples\bluetooth\esp_ble_mesh\ble_mesh_sensor_model folder. 
* This software was modified and it's modified versions exist in the src folder in this directory as well as the \working\Software\BLE_Mesh\ble_example directory.