# LESA_OccupancyUnit (BLE_MeshLightingPRFNode)


Based on STM's *BLE_MeshLightingPRFNode* example, this project gets data from the ToF and color sensor. Bluetooth functionality has not been fully developed

### Current Program Overview (STM32 occupancy detection)
```mermaid
graph TD
boot(Initilizaiton of STM32 and task registrations) ---> init(Sensor Init) --wait for init to finish--> standby{Standby};
subgraph Appli_mesh.c & others
standby --sw1 short press--> reply1(BLE color value advatisement);
reply1-->standby
standby --sw1 long press--> reply2(BLE ToF value advatisement);
reply2---> standby
standby --sw2 long press--> base(background set up)---> standby
standby -->change{Color INT}-->sensor(wakes up ToF and data pulling)--No blobs present for long period-->standby
standby --sw2 short press--> loopyloop1(ToF matrix to UART)
loopyloop1--sw2 short press--> standby
end
```

### Project Overview 
```mermaid
graph LR
subgraph Room
unit1[Server];unit2[Server];unit3[Server];unit4[Server];unit5{Client};
unit1 & unit2 ---- unit3 & unit4
unit5 ---- unit1
end
unit5 ---- building[Building management sys]
```
Server and Clients are defined by
```
#define ENABLE_SENSOR_MODEL_SERVER                                           (1)
#define ENABLE_SENSOR_MODEL_CLIENT                                           (0)
```
, where server is the one with sensor running, and client is the one requesting or listening

### Conituous ToF matrix pulling configeration
```mermaid
graph TD

server[Server]
esp[EPS32]
server <--HTTP--> esp
subgraph Module
stm[STM32] <--UART--> esp
stm <--I2C--> colorsensor[ColorS]
stm <--I2C--> tof[ToF]
end
```



## How to use
In order to use this project you must download this entire repository and open the `.project` file in the STM32CubeIDE folder:


| <img src="https://github.com/IAmiku/LESA_OccupancyUnit/assets/28797384/7b4293d4-111c-4e78-a1c0-8d8e865e68df" width="450" /> | <img src="https://github.com/IAmiku/LESA_OccupancyUnit/assets/28797384/2f35a595-6f8a-4ca5-84a6-371e19c2acde" width="450" /> |
|--|--|
| This is how the files should be placed in the workspace folder | Open the project by clicking the .project file |


## How it works

### How it runs

The example software provided by ST uses a sequencer library for scheduling tasks. 

Before using the sequencer it must be initialized by a call to the function :

`UTIL_SEQ_Init( void )`

A function is registered as a task in the sequencer by

`UTIL_SEQ_RegTask( UTIL_SEQ_bm_t TaskId_bm, uint32_t Flags, void (*Task)( void ) )`

The first parameter is the task ID and is a 32-bit value where only one bit should be set, i.e. 
the maximum number of tasks is 32. The second parameter is not used for anything by the library, and the third parameter is a pointer to the function which should be called when it is time for the task to run.

To request a task to be runned, use function 

`UTIL_SEQ_SetTask( UTIL_SEQ_bm_t TaskId_bm , uint32_t Task_Prio )`

Example:
```
UTIL_SEQ_Init( void )
UTIL_SEQ_RegTask((1 << 4), UTIL_SEQ_RFU, my_task_function);
UTIL_SEQ_SetTask((1 << 4), CFG_SEQ_Prio_0);
```

### Bluetooth setting

This project envisioned using BLE mesh to communicate between units

## Tracking Algorithm

[`Tracking_Algo.h`](Core/Inc/Tracking_Algo.h) Function declerations and Macro settings for STM32

[`Tracking_Algo.c`](Core/Src/Tracking_Algo.c) Algorithm itself, can be copied into an online C compiler to run and test

##### TODO: 
- update `void link_latest_frame_to_previous(FIFObuffer *fifobuffer)` to allow multi-frame traceback 
- update `void int find_centers_of_mass(int matrix[N][N], Point *center_of_mass)` in `FrameInfo parse_frame(int matrix[N][N], float deltaX, float deltaY)` to allow track person better

       




## Sensor related files

##### VL53L8CX (TOF Sensor)

    app_tof.c  ->  /BLE_MeshLightingPRFNode/Application/Core/app_tof.c

    - void MX_TOF_Init(void)
  
      : Initialize the TOF sensor. Validates whether it is properly connected and starts the sensor
  
    - static void MX_VL53L8CX_SimpleRanging_Process(void)
  
      : Read and print out the data if the sensor is properly running
  
    - void MX_TOF_ToggleReso(void)
  
      : A public function that calls toggle_resolution. Change the resolution of TOF sensor
  
    You can change the ranging frequency by modifying "RANGING_FREQUENCY" public variable(Set to 20Hz in the code)

    *custom_ranging_sensor.c  ->  /BLE_MeshLightingPRFNode/Application/Core/custom_ranging_sensor.c

    - int32_t CUSTOM_RANGING_SENSOR_Start(uint32_t Instance, uint8_t Mode)
  
      : Start the TOF sensor
  
    - int32_t CUSTOM_RANGING_SENSOR_Stop(uint32_t Instance)
  
      : Stop the TOF sensor

##### TCS3430 (Tristimulus Sensor)
https://github.com/IAmiku/LESA_OccupancyUnit/tree/main/NON-STM32%20Project%20Folder/ESP-TCS3430

    TCS3430.h  ->  /BLE_MeshLightingPRFNode/Drivers/BSP/TCS3430/TCS3430.h
 
    - Includes all the address of the registers of TCS3430
    
    - Includes Prototype of functions

    - struct tcs3430_optics_val

        : Has X,Y,Z, and IR values

    - struct tcs3430

        reginfo -> data in the registers

        XYZinfo -> Low & High byte of X, Y, Z, and IR1

    TCS3430.c  ->  /BLE_MeshLightingPRFNode/Drivers/BSP/TCS3430/TCS3430.c

    ............

Above two functions are necessary whenever you change the setting of the TOF(such as frequency of configure mode)















Reference:

https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WB_BLE_MESH_Sensor_Model#/media/File:Connectivity_sensor-demo12.png

Summer Team Contact Information:

•	link10@rpi.edu 

•	kims29@rpi.edu 

•	zhengj8@rpi.edu



