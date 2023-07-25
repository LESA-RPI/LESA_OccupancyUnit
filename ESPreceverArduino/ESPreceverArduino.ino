//Watch Dog
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
//Wifi
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "time.h"
#include "sntp.h"
//Others
#include <String.h>
#include <inttypes.h>

////////////////////////Defines//////////////////////////////////////////////////////////////////////////////
// REPLACE with your Domain name and URL path or IP address with path
#define Host_Name "http://mdl-vm11.eng.rpi.edu"
#define Path_Name "/sendToDatabase_Summer.php"
#define Wifi_SSid "NETGEAR"
#define Wifi_password ""
// Time related

const char* NTP_Server1 = "pool.ntp.org";
const char* NTP_Server2 = "time.nist.gov";
const long GMT_Offset_Sec = -14400;
const int DaylightOffset_sec = 3600;
struct timeval tv_now;
struct tm timeinfo;
unsigned long long int time_us;

const char* Time_Zone = "CST6CDT,M3.2.0,M11.1.0";  // TimeZone rule for Europe/Rome including daylight adjustment rules (optional)


#define AckByte 0xFF
////////////////////////Global variables//////////////////////////////////////////////////////////////////////////////
uint8_t data[6 + 128];  //RAW data get from STM32, (low byte + high byte), arranged in this order: XYZ,cell 1,2,3...64
uint8_t AckByteVar;     //Listen for byte 0xFF
HTTPClient http;
SemaphoreHandle_t xDataAvailability = NULL;  //Semaphore Handler
SemaphoreHandle_t WIFISetupFinished = NULL;  //Semaphore Handler

uint64_t chip_id;




/////////////////////////////Functions////////////////////////////////////////////////////////////////////////////////////
void printLocalTime() {
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}
// Callback function (get's called when time adjusts via NTP)
void timeavailable(struct timeval* t) {
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
  xSemaphoreGive(WIFISetupFinished);
}

/////////////////////////////TASKS////////////////////////////////////////////////////////////////////////////////////

void ReadFromUARTTask(void* pt) {
  //this task reads UART
  Serial.print("ReadFromUARTTask created at core: ");
  Serial.println(xPortGetCoreID());
  xSemaphoreTake(xDataAvailability, 50);
  while (1) {
    while (Serial.available()) {
      Serial.readBytes(&AckByteVar, 1);
      if (AckByteVar == 0xFF) {
      } else {
        continue;
      }
      Serial.readBytes(data, 6 + 128);
      xSemaphoreGive(xDataAvailability);
//print data for debug
#if (0)
      Serial.print("DATA:");
      for (int i = 0; i < 67; i++) {
        Serial.printf("%x|", ((uint16_t*)data)[i]);
      }
      Serial.print("\n\r");
#endif
    }

    //watchdog
    TIMERG1.wdt_wprotect = TIMG_WDT_WKEY_VALUE;  // write enable
    TIMERG1.wdt_feed = 1;                        // feed dog
    TIMERG1.wdt_wprotect = 0;                    // write protect
  }
}

void PrintTask(void* pt) {
  while (1) {
  }
}

void WiFiTask(void* pt) {
  Serial.print("WiFiTask created at core: ");
  Serial.println(xPortGetCoreID());

  WiFi.begin(Wifi_SSid, Wifi_password);
  Serial.println("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(25);
    Serial.print(".");
  }
  Serial.print("\nConnected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  printLocalTime();

  char snum[20];  //string num. only used to convert strings


  while (1) {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED & xSemaphoreTake(xDataAvailability, 2)) {

      gettimeofday(&tv_now, NULL);
      time_us = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;

      //TODO: CHANGE THE FOLLOWING TO SATISFY THE REQUEST//
      //COLOR SENSOR
      String Query_String = "?Color=";
      for (int i = 0; i < 3; i++) {
        Query_String = Query_String + itoa(((uint16_t*)data)[i], snum, 16) + "|";
      }
      Query_String.remove(Query_String.length() - 1);
      //TOF
      Query_String = Query_String + "&ToF=";
      for (int i = 3; i < 67; i++) {
        Query_String = Query_String + itoa(((uint16_t*)data)[i], snum, 16) + "|";
      }
      Query_String.remove(Query_String.length() - 1);
      //TIME STAMP
      gettimeofday(&tv_now, NULL);
      time_us = ((uint64_t)getTime()) * 1000000 + (uint64_t)tv_now.tv_usec;
      char str[256];
      sprintf(str, "%llx", time_us);
      Query_String = Query_String + "&Time_Captured=" + str;
      Serial.println(Query_String);
      /*    gettimeofday(&tv_now,NULL);
    time_us = ((uint64_t)getTime()) * 1000000+ (uint64_t)tv_now.tv_usec;
    char snum[20];//string num. only used to convert strings
    Serial.printf("%lu,--,%llx\n\r",getTime(),time_us);*/


      //http request send
      http.begin(String(Host_Name) + String(Path_Name) + String(Query_String));  //HTTP
      int httpCode = http.GET();
      // httpCode will be negative on error
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      } else {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      }
    }
  }
}

/////////////////////////////SET LOOP TASK////////////////////////////////////////////////////////////////////////////////////

void setup() {
  //semaphore
  xDataAvailability = xSemaphoreCreateBinary();
  WIFISetupFinished = xSemaphoreCreateBinary();
  //default core is 1
  Serial.begin(115200);
  Serial.print("ESP32 Tick Period - ");
  Serial.print(portTICK_PERIOD_MS);
  Serial.println("ms");
  Serial.print("SETUP LOOP TASK created at core: ");
  Serial.println(xPortGetCoreID());

  sntp_set_time_sync_notification_cb(timeavailable);
  sntp_servermode_dhcp(1);
  configTime(GMT_Offset_Sec, DaylightOffset_sec, NTP_Server1, NTP_Server2);
  configTzTime(Time_Zone, NTP_Server1, NTP_Server2);
  chip_id = ESP.getEfuseMac();
  //task create
  xTaskCreatePinnedToCore(WiFiTask,
                          "PrintTask",
                          1024 * 100,
                          NULL,
                          10,
                          NULL,
                          0);
  xTaskCreatePinnedToCore(ReadFromUARTTask,
                          "ReadFromUARTTask",
                          1024 * 10,
                          NULL,
                          10,
                          NULL,
                          1);
}

void loop() {
}
