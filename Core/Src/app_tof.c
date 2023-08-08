/**
  ******************************************************************************
  * @file          : app_tof.c
  * @author        : IMG SW Application Team
  * @brief         : This file provides code for the configuration
  *                  of the STMicroelectronics.X-CUBE-TOF1.3.3.0 instances.
  ******************************************************************************
  *
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "app_tof.h"
#include "main.h"
#include <stdio.h>
#include <unistd.h>

#include "custom_ranging_sensor.h"
#include "stm32wbxx_nucleo.h"

#include "mesh_cfg_usr.h"
#include "mesh_cfg.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* uncomment following to use directly the bare driver instead of the BSP */
/* #define USE_BARE_DRIVER */
#define TIMING_BUDGET (5U) /* 5 ms < TimingBudget < 100 ms */
#define RANGING_FREQUENCY (20U) /* Ranging frequency Hz (shall be consistent with TimingBudget value) */
#define POLLING_PERIOD (1000U/RANGING_FREQUENCY) /* refresh rate for polling mode (milliseconds) */

/* Private variables ---------------------------------------------------------*/
#ifndef USE_BARE_DRIVER
static RANGING_SENSOR_Capabilities_t Cap;
#endif
static RANGING_SENSOR_ProfileConfig_t Profile;

static int32_t status = 0;
volatile uint8_t ToF_EventDetected = 0;

/* Private function prototypes -----------------------------------------------*/
#ifdef USE_BARE_DRIVER
static uint8_t map_target_status(uint8_t status);
static int32_t convert_data_format(VL53L8CX_Object_t *pObj,
    VL53L8CX_ResultsData *data, RANGING_SENSOR_Result_t *pResult);
#endif
static void MX_VL53L8CX_SimpleRanging_Init(void);
static void MX_VL53L8CX_SimpleRanging_Process(void);
static void toggle_resolution(void);
static void toggle_signal_and_ambient(void);
static void clear_screen(void);
static void display_commands_banner(void);
static void handle_cmd(uint8_t cmd);
static uint8_t get_key(void);
static uint32_t com_has_data(void);

void MX_TOF_Init(void)
{
  /* USER CODE BEGIN SV */

  /* USER CODE END SV */

  /* USER CODE BEGIN TOF_Init_PreTreatment */

  /* USER CODE END TOF_Init_PreTreatment */

  /* Initialize the peripherals and the TOF components */
  TRACE_M(TF_SENSOR, "CUSTOM_RANGING_SENSOR_Init");
  MX_VL53L8CX_SimpleRanging_Init();

  uint32_t Id;
  CUSTOM_RANGING_SENSOR_ReadID(CUSTOM_VL53L8CX, &Id);
  CUSTOM_RANGING_SENSOR_GetCapabilities(CUSTOM_VL53L8CX, &Cap);

  Profile.RangingProfile = RS_PROFILE_8x8_CONTINUOUS;
  Profile.TimingBudget = TIMING_BUDGET; /* 5 ms < TimingBudget < 100 ms */
  Profile.Frequency = RANGING_FREQUENCY; /* Ranging frequency Hz (shall be consistent with TimingBudget value) */
  Profile.EnableAmbient = 0; /* Enable: 1, Disable: 0 */
  Profile.EnableSignal = 0; /* Enable: 1, Disable: 0 */

  /* set the profile if different from default one */
  CUSTOM_RANGING_SENSOR_ConfigProfile(CUSTOM_VL53L8CX, &Profile);

  /* USER CODE BEGIN TOF_Init_PostTreatment */

  /* USER CODE END TOF_Init_PostTreatment */
}

void MX_TOF_Begin(void){
	CUSTOM_RANGING_SENSOR_Start(CUSTOM_VL53L8CX, RS_MODE_BLOCKING_CONTINUOUS);
}
void MX_TOF_Stop(void){
	CUSTOM_RANGING_SENSOR_Stop(CUSTOM_VL53L8CX);
}
/*
 * LM background task
 */
void MX_TOF_Process(void)
{
  /* USER CODE BEGIN TOF_Process_PreTreatment */

  /* USER CODE END TOF_Process_PreTreatment */

  MX_VL53L8CX_SimpleRanging_Process();

  /* USER CODE BEGIN TOF_Process_PostTreatment */

  /* USER CODE END TOF_Process_PostTreatment */
}

static void MX_VL53L8CX_SimpleRanging_Init(void)
{
  /* Initialize Virtual COM Port */
  BSP_COM_Init(COM1);

  status = CUSTOM_RANGING_SENSOR_Init(CUSTOM_VL53L8CX);

  if (status != BSP_ERROR_NONE)
  {
	TRACE_M(TF_SENSOR, "CUSTOM_RANGING_SENSOR_Init_Failed!, halting...");
    while(1);
  }
}

#ifdef USE_BARE_DRIVER
#else
static void MX_VL53L8CX_SimpleRanging_Process(void)//NOT BEING USED
{
  static RANGING_SENSOR_Result_t Result;
  status = CUSTOM_RANGING_SENSOR_GetDistance(CUSTOM_VL53L8CX, &Result);

  while (status!=0)
  {    status = CUSTOM_RANGING_SENSOR_GetDistance(CUSTOM_VL53L8CX, &Result);
  }
    /* polling mode */

    if (status == BSP_ERROR_NONE)
    {
      print_result(&Result);
    }

    if (com_has_data())
    {
      handle_cmd(get_key());
    }

    //break;
  }

#endif /* USE_BARE_DRIVER */

void MX_TOF_ToggleReso(void)
{
	toggle_resolution();
}

// Function to parse RANGING_SENSOR_Result_t to [8][8] matrix
void parse_TOF_result(RANGING_SENSOR_Result_t* result, int  matrix[8][8]) {
    //assign values from result
    uint8_t j, k;
    for (j = 0; j < 8; j ++) {//row
        for (k = 0; k < 8; k++) {//column
            matrix[j][k] = (int)*result->ZoneResult[j*8+k].Distance; //matrix[1][0]=result[8=1*8+0], matrix[1][7]=result[15=1*8+7]
        }
    }
    return;
}

void print_result(RANGING_SENSOR_Result_t *Result)
{

#if(1)
	//display_commands_banner();
  int8_t i, j, k, l;
  uint8_t zones_per_line;

  zones_per_line = ((Profile.RangingProfile == RS_PROFILE_8x8_AUTONOMOUS) ||
         (Profile.RangingProfile == RS_PROFILE_8x8_CONTINUOUS)) ? 8 : 4;
  printf("\n\r");

  for (j = 0; j < Result->NumberOfZones; j += zones_per_line)
  {
    for (i = 0; i < zones_per_line; i++)
      printf(" ---------");
    printf("\n\r");
    for (l = 0; l < RANGING_SENSOR_NB_TARGET_PER_ZONE; l++)
    {
      for (k = (zones_per_line - 1); k >= 0; k--)
      {
        if (Result->ZoneResult[j+k].NumberOfTargets > 0)
        {
          if ((long)Result->ZoneResult[j+k].Distance[l] < 500)
          {
            printf("|\033[38;5;9m%4ld\033[0m:%3ld ",
              (long)Result->ZoneResult[j+k].Distance[l],
              (long)j+k);
          } else
          {
            printf("|\033[38;5;10m%4ld\033[0m:%3ld ",
              (long)Result->ZoneResult[j+k].Distance[l],
              (long)j+k);
          }
        }
        else
          printf("|%5s:%d", "X",j+k);
      }
      printf("|\n\r");
    }
  }

  for (i = 0; i < zones_per_line; i++)
	  printf(" ---------");
  printf("\n\r");
#else
  int8_t j, k, l;
  uint8_t zones_per_line;
  zones_per_line = ((Profile.RangingProfile == RS_PROFILE_8x8_AUTONOMOUS) ||
         (Profile.RangingProfile == RS_PROFILE_8x8_CONTINUOUS)) ? 8 : 4;

  for (j = 0; j < Result->NumberOfZones; j += zones_per_line)
  {
    for (l = 0; l < RANGING_SENSOR_NB_TARGET_PER_ZONE; l++)
    {
      for (k = (zones_per_line - 1); k >= 0; k--)
      {
        if (Result->ZoneResult[j+k].NumberOfTargets > 0)
        {
            printf("%x|",(uint16_t)Result->ZoneResult[j+k].Distance[l]);
            //Pure_result[j+k]=(uint16_t)Result->ZoneResult[j+k].Distance[l];
        }
      }
    }
  }


#endif
}

static void toggle_resolution(void)
{
  CUSTOM_RANGING_SENSOR_Stop(CUSTOM_VL53L8CX);

  switch (Profile.RangingProfile)
  {
    case RS_PROFILE_4x4_AUTONOMOUS:
      Profile.RangingProfile = RS_PROFILE_8x8_AUTONOMOUS;
      break;

    case RS_PROFILE_4x4_CONTINUOUS:
      Profile.RangingProfile = RS_PROFILE_8x8_CONTINUOUS;
      break;

    case RS_PROFILE_8x8_AUTONOMOUS:
      Profile.RangingProfile = RS_PROFILE_4x4_AUTONOMOUS;
      break;

    case RS_PROFILE_8x8_CONTINUOUS:
      Profile.RangingProfile = RS_PROFILE_4x4_CONTINUOUS;
      break;

    default:
      break;
  }

  CUSTOM_RANGING_SENSOR_ConfigProfile(CUSTOM_VL53L8CX, &Profile);
  CUSTOM_RANGING_SENSOR_Start(CUSTOM_VL53L8CX, RS_MODE_BLOCKING_CONTINUOUS);
}

static void toggle_signal_and_ambient(void)
{
  CUSTOM_RANGING_SENSOR_Stop(CUSTOM_VL53L8CX);

  Profile.EnableAmbient = (Profile.EnableAmbient) ? 0U : 1U;
  Profile.EnableSignal = (Profile.EnableSignal) ? 0U : 1U;

  CUSTOM_RANGING_SENSOR_ConfigProfile(CUSTOM_VL53L8CX, &Profile);
  CUSTOM_RANGING_SENSOR_Start(CUSTOM_VL53L8CX, RS_MODE_BLOCKING_CONTINUOUS);
}

static void clear_screen(void)
{
  /* clear Screen : 27 is ESC command */
  printf("%c[2J", 27);
}

static void display_commands_banner(void)
{
  /* clear screen */
  printf("%c[2H", 27);

  printf("VL53L8CX Simple Ranging demo application\n\r");
#ifdef USE_BARE_DRIVER
  printf("Using direct calls to VL53L8CX bare driver API\n\r");
#endif
  printf("Polling mode\n\r");
  printf("----------------------------------------\n\r\n\r");

  printf("Use the following keys to control application\n\r");
  printf(" 'r' : change resolution\n\r");
  printf(" 's' : enable signal and ambient\n\r");
  printf(" 'c' : clear screen\n\r");
  printf("\n\r");
}

static void handle_cmd(uint8_t cmd)
{
  switch (cmd)
  {
    case 'r':
      toggle_resolution();
      clear_screen();
      break;

    case 's':
      toggle_signal_and_ambient();
      clear_screen();
      break;

    case 'c':
      clear_screen();
      break;

    default:
      break;
  }
}

static uint8_t get_key(void)
{
  uint8_t cmd = 0;

  HAL_UART_Receive(&hcom_uart[COM1], &cmd, 1, HAL_MAX_DELAY);

  return cmd;
}

static uint32_t com_has_data(void)
{
  return __HAL_UART_GET_FLAG(&hcom_uart[COM1], UART_FLAG_RXNE);;
}

#ifdef USE_BARE_DRIVER
static uint8_t map_target_status(uint8_t status)
{
  uint8_t ret;

  if ((status == 5U) || (status == 9U))
  {
    ret = 0U; /* ranging is OK */
  }
  else if (status == 0U)
  {
    ret = 255U; /* no update */
  }
  else
  {
    ret = status; /* return device status otherwise */
  }

  return ret;
}

static int32_t convert_data_format(VL53L8CX_Object_t *pObj,
    VL53L8CX_ResultsData *data, RANGING_SENSOR_Result_t *pResult)
{
  int32_t ret;
  uint8_t i, j;
  uint8_t resolution;
  uint8_t target_status;

  if ((pObj == NULL) || (pResult == NULL))
  {
    ret = VL53L8CX_INVALID_PARAM;
  }
  else if (vl53l8cx_get_resolution(&pObj->Dev, &resolution) != VL53L8CX_STATUS_OK)
  {
    ret = VL53L8CX_ERROR;
  }
  else
  {
    pResult->NumberOfZones = resolution;

    for (i = 0; i < resolution; i++)
    {
      pResult->ZoneResult[i].NumberOfTargets = data->nb_target_detected[i];

      for (j = 0; j < data->nb_target_detected[i]; j++)
      {
        pResult->ZoneResult[i].Distance[j] = (uint32_t)data->distance_mm[(VL53L8CX_NB_TARGET_PER_ZONE * i) + j];

        /* return Ambient value if ambient rate output is enabled */
        if (pObj->IsAmbientEnabled == 1U)
        {
          /* apply ambient value to all targets in a given zone */
          pResult->ZoneResult[i].Ambient[j] = (float_t)data->ambient_per_spad[i];
        }
        else
        {
          pResult->ZoneResult[i].Ambient[j] = 0.0f;
        }

        /* return Signal value if signal rate output is enabled */
        if (pObj->IsSignalEnabled == 1U)
        {
          pResult->ZoneResult[i].Signal[j] =
            (float_t)data->signal_per_spad[(VL53L8CX_NB_TARGET_PER_ZONE * i) + j];
        }
        else
        {
          pResult->ZoneResult[i].Signal[j] = 0.0f;
        }

        target_status = data->target_status[(VL53L8CX_NB_TARGET_PER_ZONE * i) + j];
        pResult->ZoneResult[i].Status[j] = map_target_status(target_status);
      }
    }

    ret = VL53L8CX_OK;
  }

  return ret;
}
#endif

#ifdef __cplusplus
}
#endif
