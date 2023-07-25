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
int64_t time_us;

const char* Time_Zone = "CST6CDT,M3.2.0,M11.1.0";  // TimeZone rule for Europe/Rome including daylight adjustment rules (optional)


#define AckByte 0xFF
////////////////////////Global variables//////////////////////////////////////////////////////////////////////////////
uint8_t data[6 + 128];  //RAW data get from STM32, (low byte + high byte), arranged in this order: XYZ,cell 1,2,3...64
uint8_t AckByteVar;     //Listen for byte 0xFF
HTTPClient http;
SemaphoreHandle_t xDataAvailability = NULL;  //Semaphore Handler




/////////////////////////////Functions////////////////////////////////////////////////////////////////////////////////////
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
// Callback function (get's called when time adjusts via NTP)
void timeavailable(struct timeval* t) {
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
}
/////////////////////////////TASKS////////////////////////////////////////////////////////////////////////////////////

void ReadFromUARTTask(void* pt) {
  //this task reads UART
  Serial.print("ReadFromUARTTask created at core: ");
  Serial.println(xPortGetCoreID());
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
    vTaskDelay(50);
    Serial.print(".");
  }
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  printLocalTime();


  while (1) {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED & xSemaphoreTake(xDataAvailability, 2)) {

      gettimeofday(&tv_now, NULL);
      time_us = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;

      //TODO: CHANGE THE FOLLOWING TO SATISFY THE REQUEST//
      
      String Query_String = "&Color='";
      char snum[6];
      for (int i = 0; i < 3; i++) {
        Query_String = Query_String + itoa(((uint16_t*)data)[i], snum, 10) + "|";
      }
      Query_String = Query_String + "'&ToF='";
      //TODO: ADD TOF
      for (int i = 3; i < 67; i++) {
        Query_String = Query_String + itoa(((uint16_t*)data)[i], snum, 10) + "|";
      }
      char longlongtime[16];
      Query_String = Query_String + "'&Time_Logged=" + itoa(time_us, longlongtime, 10);
      Serial.println(Query_String);



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
  //default core is 0
  Serial.begin(115200);
  Serial.print("ESP32 Tick Period - ");
  Serial.print(portTICK_PERIOD_MS);
  Serial.println("ms");

  sntp_set_time_sync_notification_cb(timeavailable);
  sntp_servermode_dhcp(1);
  configTime(GMT_Offset_Sec, DaylightOffset_sec, NTP_Server1, NTP_Server2);
  configTzTime(Time_Zone, NTP_Server1, NTP_Server2);


  xDataAvailability = xSemaphoreCreateBinary();

  xTaskCreatePinnedToCore(ReadFromUARTTask,
                          "ReadFromUARTTask",
                          1024 * 10,
                          NULL,
                          2,
                          NULL,
                          1); 
  xTaskCreatePinnedToCore(WiFiTask,
                          "PrintTask",
                          1024 * 100,
                          NULL,
                          2,
                          NULL,
                          0);
}

void loop() {
}
