/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    p2p_server_app.c
  * @author  MCD Application Team
  * @brief   Peer to peer Server Application
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_common.h"
#include "dbg_trace.h"
#include "ble.h"
#include "p2p_server_app.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "hw_if.h"
#include "TCS3430.h"
#include "Tracking_Algo.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
 typedef struct{
    uint8_t             Device_Led_Selection;
    uint8_t             Led1;
 }P2P_LedCharValue_t;

 typedef struct{
    uint8_t             Device_Button_Selection;
    uint8_t             ButtonStatus;
 }P2P_ButtonCharValue_t;

typedef struct
{
  uint8_t               Notification_Status; /* used to check if P2P Server is enabled to Notify */
  P2P_LedCharValue_t    LedControl;
  P2P_ButtonCharValue_t ButtonControl;
  uint16_t              ConnectionHandle;
} P2P_Server_App_Context_t;
/* USER CODE END PTD */

/* Private defines ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BOUNCE_THRESHOLD                20U
#define LONG_PRESS_THRESHOLD            1000U

#define TOF_ENABLE	1
/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/**
 * START of Section BLE_APP_CONTEXT
 */

static P2P_Server_App_Context_t P2P_Server_App_Context;
uint8_t colorTimer_Id, ToFTimer_Id;
static tcs3430_optics_val rawBaseColor, rawColor;
tcs3430_optics_val color_data;
static tcs3430 sensor = { { 0 }, { 0 } };
//tof
static RANGING_SENSOR_Result_t rawBaseFrame, rawFrame;
uint8_t Sensor_Running = 0, BaseFrameValid = 0;
uint16_t ble_notify_data[68];
/**
 * END of Section BLE_APP_CONTEXT
 */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void Button1PressTask(void);
static void Button2PressTask(void);
static void P2PS_APP_LED_BUTTON_context_Init(void);
static void TrackingTask(void);
static void TCS3430Read(void);
static void ColorTask(void);
static void ToFTask(void);
static void LoopApp(void);	//TODO: finish up
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void P2PS_STM_App_Notification(P2PS_STM_App_Notification_evt_t *pNotification)
{
/* USER CODE BEGIN P2PS_STM_App_Notification_1 */

/* USER CODE END P2PS_STM_App_Notification_1 */
  switch(pNotification->P2P_Evt_Opcode)
  {
/* USER CODE BEGIN P2PS_STM_App_Notification_P2P_Evt_Opcode */
#if(BLE_CFG_OTA_REBOOT_CHAR != 0)
    case P2PS_STM_BOOT_REQUEST_EVT:
      APP_DBG_MSG("-- P2P APPLICATION SERVER : BOOT REQUESTED\n");
      APP_DBG_MSG(" \n\r");

      *(uint32_t*)SRAM1_BASE = *(uint32_t*)pNotification->DataTransfered.pPayload;
      NVIC_SystemReset();
      break;
#endif
/* USER CODE END P2PS_STM_App_Notification_P2P_Evt_Opcode */

    case P2PS_STM__NOTIFY_ENABLED_EVT:
/* USER CODE BEGIN P2PS_STM__NOTIFY_ENABLED_EVT */
      P2P_Server_App_Context.Notification_Status = 1;
      APP_DBG_MSG("-- P2P APPLICATION SERVER : NOTIFICATION ENABLED\n"); 
      APP_DBG_MSG(" \n\r");
/* USER CODE END P2PS_STM__NOTIFY_ENABLED_EVT */
      break;

    case P2PS_STM_NOTIFY_DISABLED_EVT:
/* USER CODE BEGIN P2PS_STM_NOTIFY_DISABLED_EVT */
      P2P_Server_App_Context.Notification_Status = 0;
      APP_DBG_MSG("-- P2P APPLICATION SERVER : NOTIFICATION DISABLED\n");
      APP_DBG_MSG(" \n\r");
/* USER CODE END P2PS_STM_NOTIFY_DISABLED_EVT */
      break;

    case P2PS_STM_WRITE_EVT:
/* USER CODE BEGIN P2PS_STM_WRITE_EVT */
      if(pNotification->DataTransfered.pPayload[0] == 0x00){ /* ALL Deviceselected - may be necessary as LB Routeur informs all connection */
        if(pNotification->DataTransfered.pPayload[1] == 0x01)
        {
          BSP_LED_On(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER  : LED1 ON\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x01; /* LED1 ON */
        }
        if(pNotification->DataTransfered.pPayload[1] == 0x00)
        {
          BSP_LED_Off(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER  : LED1 OFF\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x00; /* LED1 OFF */
        }
      }
#if(P2P_SERVER1 != 0)  
      if(pNotification->DataTransfered.pPayload[0] == 0x01){ /* end device 1 selected - may be necessary as LB Routeur informs all connection */
        if(pNotification->DataTransfered.pPayload[1] == 0x01)
        {
          BSP_LED_On(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER 1 : LED1 ON\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x01; /* LED1 ON */
        }
        if(pNotification->DataTransfered.pPayload[1] == 0x00)
        {
          BSP_LED_Off(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER 1 : LED1 OFF\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x00; /* LED1 OFF */
        }
      }
#endif
#if(P2P_SERVER2 != 0)
      if(pNotification->DataTransfered.pPayload[0] == 0x02){ /* end device 2 selected */ 
        if(pNotification->DataTransfered.pPayload[1] == 0x01)
        {
          BSP_LED_On(LED_BLUE);
           APP_DBG_MSG("-- P2P APPLICATION SERVER 2 : LED1 ON\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x01; /* LED1 ON */
        }
        if(pNotification->DataTransfered.pPayload[1] == 0x00)
        {
          BSP_LED_Off(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER 2 : LED1 OFF\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x00; /* LED1 OFF */
        }   
      }
#endif      
#if(P2P_SERVER3 != 0)  
      if(pNotification->DataTransfered.pPayload[0] == 0x03){ /* end device 3 selected - may be necessary as LB Routeur informs all connection */
        if(pNotification->DataTransfered.pPayload[1] == 0x01)
        {
          BSP_LED_On(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER 3 : LED1 ON\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x01; /* LED1 ON */
        }
        if(pNotification->DataTransfered.pPayload[1] == 0x00)
        {
          BSP_LED_Off(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER 3 : LED1 OFF\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x00; /* LED1 OFF */
        }
      }
#endif
#if(P2P_SERVER4 != 0)
      if(pNotification->DataTransfered.pPayload[0] == 0x04){ /* end device 4 selected */ 
        if(pNotification->DataTransfered.pPayload[1] == 0x01)
        {
          BSP_LED_On(LED_BLUE);
           APP_DBG_MSG("-- P2P APPLICATION SERVER 2 : LED1 ON\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x01; /* LED1 ON */
        }
        if(pNotification->DataTransfered.pPayload[1] == 0x00)
        {
          BSP_LED_Off(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER 2 : LED1 OFF\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x00; /* LED1 OFF */
        }   
      }
#endif     
#if(P2P_SERVER5 != 0)  
      if(pNotification->DataTransfered.pPayload[0] == 0x05){ /* end device 5 selected - may be necessary as LB Routeur informs all connection */
        if(pNotification->DataTransfered.pPayload[1] == 0x01)
        {
          BSP_LED_On(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER 5 : LED1 ON\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x01; /* LED1 ON */
        }
        if(pNotification->DataTransfered.pPayload[1] == 0x00)
        {
          BSP_LED_Off(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER 5 : LED1 OFF\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x00; /* LED1 OFF */
        }
      }
#endif
#if(P2P_SERVER6 != 0)
      if(pNotification->DataTransfered.pPayload[0] == 0x06){ /* end device 6 selected */ 
        if(pNotification->DataTransfered.pPayload[1] == 0x01)
        {
          BSP_LED_On(LED_BLUE);
           APP_DBG_MSG("-- P2P APPLICATION SERVER 6 : LED1 ON\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x01; /* LED1 ON */
        }
        if(pNotification->DataTransfered.pPayload[1] == 0x00)
        {
          BSP_LED_Off(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER 6 : LED1 OFF\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x00; /* LED1 OFF */
        }   
      }
#endif 
/* USER CODE END P2PS_STM_WRITE_EVT */
      break;

    default:
/* USER CODE BEGIN P2PS_STM_App_Notification_default */
      
/* USER CODE END P2PS_STM_App_Notification_default */
      break;
  }
/* USER CODE BEGIN P2PS_STM_App_Notification_2 */

/* USER CODE END P2PS_STM_App_Notification_2 */
  return;
}

void P2PS_APP_Notification(P2PS_APP_ConnHandle_Not_evt_t *pNotification)
{
/* USER CODE BEGIN P2PS_APP_Notification_1 */

/* USER CODE END P2PS_APP_Notification_1 */
  switch(pNotification->P2P_Evt_Opcode)
  {
/* USER CODE BEGIN P2PS_APP_Notification_P2P_Evt_Opcode */

/* USER CODE END P2PS_APP_Notification_P2P_Evt_Opcode */
  case PEER_CONN_HANDLE_EVT :
/* USER CODE BEGIN PEER_CONN_HANDLE_EVT */
          
/* USER CODE END PEER_CONN_HANDLE_EVT */
    break;

    case PEER_DISCON_HANDLE_EVT :
/* USER CODE BEGIN PEER_DISCON_HANDLE_EVT */
       P2PS_APP_LED_BUTTON_context_Init();       
/* USER CODE END PEER_DISCON_HANDLE_EVT */
    break;

    default:
/* USER CODE BEGIN P2PS_APP_Notification_default */

/* USER CODE END P2PS_APP_Notification_default */
      break;
  }
/* USER CODE BEGIN P2PS_APP_Notification_2 */

/* USER CODE END P2PS_APP_Notification_2 */
  return;
}

void P2PS_APP_Init(void)
{
/* USER CODE BEGIN P2PS_APP_Init */
  UTIL_SEQ_RegTask( 1<< CFG_TASK_SW1_BUTTON_PUSHED_ID, UTIL_SEQ_RFU, Button1PressTask);
  UTIL_SEQ_RegTask( 1<< CFG_TASK_SW2_BUTTON_PUSHED_ID, UTIL_SEQ_RFU, Button2PressTask);

  UTIL_SEQ_RegTask(1 << CFG_TASK_USER_SET_TASK, UTIL_SEQ_RFU, TCS3430Read);

  UTIL_SEQ_RegTask(1 << CFG_TASK_COLOR_REQ_ID, UTIL_SEQ_RFU, TrackingTask);

  UTIL_SEQ_RegTask(1 << CFG_TASK_TOFLOOP_TASK, UTIL_SEQ_RFU, LoopApp);

  HW_TS_Create(0, &colorTimer_Id, hw_ts_Repeated, ColorTask);

  HW_TS_Start(colorTimer_Id, 200000/CFG_TS_TICK_VAL);

  HW_TS_Create(0, &ToFTimer_Id, hw_ts_Repeated, ToFTask);


  /**
   * Initialize LedButton Service
   */
  P2P_Server_App_Context.Notification_Status=0; 
  P2PS_APP_LED_BUTTON_context_Init();

#if TOF_ENABLE == 1

  MX_TOF_Init();

#endif

  Color_Init();

/* USER CODE END P2PS_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
void P2PS_APP_LED_BUTTON_context_Init(void){

  #if(P2P_SERVER1 != 0)
  P2P_Server_App_Context.LedControl.Device_Led_Selection=0x01; /* Device1 */
  P2P_Server_App_Context.LedControl.Led1=0x00; /* led OFF */
  P2P_Server_App_Context.ButtonControl.Device_Button_Selection=0x01;/* Device1 */
  P2P_Server_App_Context.ButtonControl.ButtonStatus=0x00;
#endif
#if(P2P_SERVER2 != 0)
  P2P_Server_App_Context.LedControl.Device_Led_Selection=0x02; /* Device2 */
  P2P_Server_App_Context.LedControl.Led1=0x00; /* led OFF */
  P2P_Server_App_Context.ButtonControl.Device_Button_Selection=0x02;/* Device2 */
  P2P_Server_App_Context.ButtonControl.ButtonStatus=0x00;
#endif  
#if(P2P_SERVER3 != 0)
  P2P_Server_App_Context.LedControl.Device_Led_Selection=0x03; /* Device3 */
  P2P_Server_App_Context.LedControl.Led1=0x00; /* led OFF */
  P2P_Server_App_Context.ButtonControl.Device_Button_Selection=0x03; /* Device3 */
  P2P_Server_App_Context.ButtonControl.ButtonStatus=0x00;
#endif
#if(P2P_SERVER4 != 0)
  P2P_Server_App_Context.LedControl.Device_Led_Selection=0x04; /* Device4 */
  P2P_Server_App_Context.LedControl.Led1=0x00; /* led OFF */
  P2P_Server_App_Context.ButtonControl.Device_Button_Selection=0x04; /* Device4 */
  P2P_Server_App_Context.ButtonControl.ButtonStatus=0x00;
#endif  
 #if(P2P_SERVER5 != 0)
  P2P_Server_App_Context.LedControl.Device_Led_Selection=0x05; /* Device5 */
  P2P_Server_App_Context.LedControl.Led1=0x00; /* led OFF */
  P2P_Server_App_Context.ButtonControl.Device_Button_Selection=0x05; /* Device5 */
  P2P_Server_App_Context.ButtonControl.ButtonStatus=0x00;
#endif
#if(P2P_SERVER6 != 0)
  P2P_Server_App_Context.LedControl.Device_Led_Selection=0x06; /* device6 */
  P2P_Server_App_Context.LedControl.Led1=0x00; /* led OFF */
  P2P_Server_App_Context.ButtonControl.Device_Button_Selection=0x06; /* Device6 */
  P2P_Server_App_Context.ButtonControl.ButtonStatus=0x00;
#endif  
}


static void Appli_ShortButtonPress(void)
{
	APP_DBG_MSG("\n\rServer Short Button Pressed\r\n");

	tcs3430 sensor = { {0}, {0} };
	TCS3430_print_reg(&sensor);
	color_data = get_raw_XYZ(&sensor);
	TCS3430_print_color(&color_data);

	memcpy(ble_notify_data, &color_data, 8);

	if(P2P_Server_App_Context.Notification_Status){
		APP_DBG_MSG("-- P2P APPLICATION SERVER  : INFORM CLIENT BUTTON 1 PUSHED \n ");
		APP_DBG_MSG(" \n\r");
		P2PS_STM_App_Update_Char(P2P_NOTIFY_CHAR_UUID, (uint8_t *)ble_notify_data, sizeof(ble_notify_data));
	} else {
	   APP_DBG_MSG("-- P2P APPLICATION SERVER : CAN'T INFORM CLIENT -  NOTIFICATION DISABLED\n ");
	}
}

/**
 * @brief  Function calls when a button is pressed for Long duration
 * @param  void
 * @retval void
 */
static void Appli_LongButtonPress(void)
{
#if TOF_ENABLE == 1
	CDC_Transmit_FS("helloworld\r\n", 12);

	APP_DBG_MSG("\n\rServer Long Button Pressed\r\n");

	tcs3430 sensor = { {0}, {0} };
	TCS3430_print_reg(&sensor);
	color_data = get_raw_XYZ(&sensor);
	TCS3430_print_color(&color_data);

	memcpy(ble_notify_data, &color_data, 8);

	//length 2, peer 0004, dst 0003, element index 0
	MX_TOF_Begin();

	RANGING_SENSOR_Result_t Result;

	int32_t status = CUSTOM_RANGING_SENSOR_GetDistance(CUSTOM_VL53L8CX, &Result);
	while (status != 0)
	{
		status = CUSTOM_RANGING_SENSOR_GetDistance(CUSTOM_VL53L8CX, &Result);
	}
	uint16_t *tof = print_result(&Result);

	memcpy(ble_notify_data+4, tof, 128);

	P2PS_STM_App_Update_Char(P2P_NOTIFY_CHAR_UUID, (uint8_t *)ble_notify_data, sizeof(ble_notify_data));

	MX_TOF_Stop();
#else
	tcs3430 sensor = { {0}, {0} };
	TCS3430_print_reg(&sensor);
	color_data = get_raw_XYZ(&sensor);
	TCS3430_print_color(&color_data);

	memcpy(ble_notify_data, &color_data, 8);

	P2PS_STM_App_Update_Char(P2P_NOTIFY_CHAR_UUID, (uint8_t *)ble_notify_data, sizeof(ble_notify_data));
#endif
}
/**
 * @brief  Function calls when a button2 is pressed for short duration
 * @param  void
 * @retval void
 */
static void Appli_ShortButton2Press(void) {

	if (Sensor_Running) {
		HW_TS_Stop(ToFTimer_Id);
#if TOF_ENABLE == 1
		MX_TOF_Stop();
#endif
		Sensor_Running = 0;
	} else {
#if TOF_ENABLE == 1
		MX_TOF_Begin();
#endif
		HW_TS_Start(ToFTimer_Id, 50000/CFG_TS_TICK_VAL);
		Sensor_Running = 1;
	}

}
/**
 * @brief  Function calls when a button2 is pressed for Long duration
 * @param  void
 * @retval void
 */
static void Appli_LongButton2Press(void) {
#if TOF_ENABLE == 1

	MX_TOF_Begin();
	//change formate to fit STM_DataPacket.c

	//get sensor values
	CUSTOM_RANGING_SENSOR_GetDistance(CUSTOM_VL53L8CX, &rawBaseFrame);
	rawBaseColor = get_raw_XYZ(&sensor);

	printf("base value being set\n");

	MX_TOF_Stop();
	BaseFrameValid = 1;

	Update_ALSThreshold(&sensor);
#endif
}

/**
 * @brief  Updates the button status
 * @param  int isPressed
 * @retval void
 */
static void Appli_UpdateButtonState(int isPressed) {
	uint32_t t0 = 0, t1 = 1;

	t0 = HAL_GetTick(); /* press timing */

	while (BSP_PB_GetState(BUTTON_SW1) == GPIO_PIN_RESET);
	t1 = HAL_GetTick(); /* SW1 release timing */

	if ((t1 - t0) > LONG_PRESS_THRESHOLD) {
		Appli_LongButtonPress();
	} else if ((t1 - t0) > BOUNCE_THRESHOLD) {
		/* Button 1 short press action */
		Appli_ShortButtonPress();
	}
}
static void Appli_UpdateButton2State(int isPressed) {

	uint32_t t0 = 0, t1 = 1;

	t0 = HAL_GetTick(); /* press timing */
	while (BSP_PB_GetState(BUTTON_SW2) == GPIO_PIN_RESET);	//TODO: fill out this

	t1 = HAL_GetTick(); /* SW2 release timing */

	if ((t1 - t0) > LONG_PRESS_THRESHOLD) {
		Appli_LongButton2Press();
	} else if ((t1 - t0) > BOUNCE_THRESHOLD) {
		Appli_ShortButton2Press();
	}
}


static void Button1PressTask(void)
{
	APP_DBG_MSG("BUTTON PRESS\r\n");
	Appli_UpdateButtonState(BSP_PB_GetState(BUTTON_SW1) == GPIO_PIN_RESET);
	return;
}

static void Button2PressTask(void)
{
	Appli_UpdateButton2State(BSP_PB_GetState(BUTTON_SW2) == GPIO_PIN_RESET);
	return;
}

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

static void beginTracking(void)
{
#if TOF_ENABLE == 1
	//TODO: this function is called when an interrupt happened
	/* Tasks:
	 * 1 check for the existance of base frame
	 * 2 remove self clear color interrupt register
	 * 3 while (20frames) read from the color register and tof, constantly ranging until no more **
	 * 4 sets up the interrupt register again
	 *
	 */
	//task 1
	printf("begin Tracking triggered\n");
	BSP_LED_On(LED_GREEN);

	if (BaseFrameValid==0){
		printf("BaseFrame uninitialized");
		return;
	}
	//task 2
	set_ALSInterrupt(sensor.reginfo, 0, 0);
	reset_Status(sensor.reginfo);


	//task 3

	//Creates 2D arrays to hold the normalized data
	int BaseFrame[8][8];
	int Frame[8][8];
	int NormFrame[8][8];
	MX_TOF_Begin();

	parse_TOF_result(&rawBaseFrame, BaseFrame);
	FIFObuffer fifobuffer;
	init_fifo(&fifobuffer); // Initialize the FIFObuffer

	//printf("\033[2J\033[H");
	uint8_t loopblob = 40;
	while (loopblob!=0) {
		//reads new frame
#if (1)
		//printf("\033[H");
		//printf("\033[2J\033[H");
#endif
		printf("%d loop til sleep\n",--loopblob);
		printf("NEWFRAME\n");
		int32_t status = CUSTOM_RANGING_SENSOR_GetDistance(CUSTOM_VL53L8CX,
				&rawFrame);
		while (status != 0) {
			status = CUSTOM_RANGING_SENSOR_GetDistance(CUSTOM_VL53L8CX,
					&rawFrame);
		}
		rawColor = get_raw_XYZ(&sensor);
		//TCS3430_print_color(&rawColor);
		float dx, dy;
		colordiff(rawBaseColor,rawColor,&dx,&dy);


		//parse new frame
		parse_TOF_result(&rawFrame, Frame);

		subtract_matrices(NormFrame, BaseFrame, Frame); //norm_data=8 by 8 matrix

//TODO: decide whether print matrix or not
		print_matrix(NormFrame);


        FrameInfo frame_info = parse_frame(NormFrame,dx,dy);

        //print_frame_info(frame_info);

        if (frame_info.num_blobs>0){loopblob=40;}

        enqueue(&fifobuffer, frame_info); // Store the FrameInfo into the FIFObuffer
        //print_frame_info(fifobuffer.buffer[fifobuffer.rear]);

        print_frame_info_simple(fifobuffer.buffer[fifobuffer.rear]);
        printf("NEWFRAMEEND\n");


        //printf( "About to print FIFO\n");
        //print_fifo(&fifobuffer);


/*
        printf( "\n\r************************************************************"
                "\n\r************************************************************\033[H\033[H\033[H");//*/

        //printf("\033[H");
	}

	//task 4
	Update_ALSThreshold(&sensor);
	reset_Status(sensor.reginfo);
	MX_TOF_Stop();
	BSP_LED_Off(LED_GREEN);
	rawBaseColor = get_raw_XYZ(&sensor);
	set_ALSInterrupt(sensor.reginfo, 1, 0);
#endif
}

static void TrackingTask(void)
{
	if(BSP_PB_GetState(COLOR_INT) == GPIO_PIN_RESET){
		beginTracking();
	}
	return;
}

static void ColorTask(void)
{
	UTIL_SEQ_SetTask(1 << CFG_TASK_USER_SET_TASK, CFG_SCH_PRIO_0);

	return;
}

static void ToFTask(void)
{
	UTIL_SEQ_SetTask(1 << CFG_TASK_TOFLOOP_TASK, CFG_SCH_PRIO_0);
}


#define COLOR_THRESHOLD 			100			/*调整阈值的时机*/
#define HYSTERESIS			10			/*迟滞�?�数*/

uint8_t Sensitivity = 0;				/*当�?�?��?度*/

#define LONG_ATIME			(0)
#define	HIGH_GAIN			(0)

#define SHORT_ATIME			(0)
#define	LOW_GAIN			(0)

void adjust_exposure(void) {

#if 0

    uint16_t light_level = color_data.IR;  // 读�?�环境光亮度

    if(Sensitivity == 0)	/*低�?��?度*/
    {
    	if(light_level < COLOR_THRESHOLD-HYSTERESIS)
    	{
    		set_atime(LONG_ATIME, 18);
    		set_cfg1(HIGH_GAIN, 0, 16);
    		Sensitivity = 0;
    	}
    }
    else
    {
    	if(light_level > COLOR_THRESHOLD+HYSTERESIS)
		{
			set_atime(SHORT_ATIME, 18);
			set_cfg1(LOW_GAIN, 0, 16);
			Sensitivity = 1;
		}
    }
#endif
}


static void TCS3430Read(void)
{
	BSP_LED_Toggle(LED_BLUE);

	Appli_LongButtonPress();
//
//	adjust_exposure();
}

static void LoopApp(void)	//TODO: finish up
{
	static uint8_t Counter = 0;
	if (Counter == 20) {
		Counter = 0;

//		BSP_LED_Toggle(LED_RED);
	}
	Counter++;

#if TOF_ENABLE == 1
	uint8_t datas[6+128];
	tcs3430_optics_val color_data;
	tcs3430 sensor = { {0}, {0} };
	color_data = get_raw_XYZ(&sensor);
	datas[0]=(uint8_t)color_data.X;datas[1]=(uint8_t)((color_data.X)>>8);
	datas[2]=(uint8_t)color_data.Y;datas[3]=(uint8_t)((color_data.Y)>>8);
	datas[4]=(uint8_t)color_data.Z;datas[5]=(uint8_t)((color_data.Z)>>8);
	//TCS3430_print_color(&color_data);

	RANGING_SENSOR_Result_t Result;
	CUSTOM_RANGING_SENSOR_GetDistance(CUSTOM_VL53L8CX, &Result);
	int8_t j, k, l;
	for (j = 0; j < Result.NumberOfZones; j += 8){
		for (l = 0; l < RANGING_SENSOR_NB_TARGET_PER_ZONE; l++){
			for (k = (8 - 1); k >= 0; k--){
				if (Result.ZoneResult[j+k].NumberOfTargets > 0){
					datas[6+k+k+j+j]=(uint8_t)Result.ZoneResult[j+k].Distance[l];
					datas[6+k+k+j+j+1]=(uint8_t)((Result.ZoneResult[j+k].Distance[l])>>8);
				}
			}
		}
	}
#endif
}
/* USER CODE END FD_LOCAL_FUNCTIONS*/
