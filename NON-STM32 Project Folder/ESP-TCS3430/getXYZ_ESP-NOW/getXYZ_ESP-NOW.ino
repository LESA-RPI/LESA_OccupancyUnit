/*!
 * @file getXYZIRData.ino
 * @brief Detection of XYZ tristimulus and infrared data
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [yangfeng]<feng.yang@dfrobot.com>
 * @version  V1.0
 * @date  2021-01-26
 * @get from https://www.dfrobot.com
 * @url  https://github.com/DFRobot/DFRobot_TCS3430
 */
#include <DFRobot_TCS3430.h>

#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x58,0xCF,0x79,0x04,0xF2,0x30};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  uint8_t XYZ[8];
} struct_message;
struct_message myData;
esp_now_peer_info_t peerInfo;


DFRobot_TCS3430 TCS3430;


//#define SCL 4
//#define SDA 5

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent); 
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  //Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  while(!TCS3430.begin()){
    Serial.println("Please check that the IIC device is properly connected. scl at: "+String(SCL)+"sda: "+String(SDA));
    delay(1000);
  }
  TCS3430.setALSGain(3);
}

void loop() {
  uint16_t X = TCS3430.getXData();
  uint16_t Y = TCS3430.getYData();
  uint16_t Z = TCS3430.getZData();
  uint16_t IR1Data = TCS3430.getIR1Data();
  //uint16_t IR2Data = TCS3430.getIR2Data();
  int R =  3.2404542*X - 1.5371385*Y - 0.4985314*Z;
  int G = -0.9692660*X + 1.8760108*Y + 0.0415560*Z;
  int B =  0.0556434*X - 0.2040259*Y + 1.0572252*Z;
  String str = "X : " + String(X) + "    Y : " + String(Y) + "    Z : " +  String(Z) + "    IR1 : "+String(IR1Data)
  + "    R : " +  String(R) + "    G : " +  String(G) + "    B : " +  String(B) ;// + "    IR2 : "+String(IR2Data);
  Serial.print(str);
  Serial.print("\n");
  //neopixelWrite(RGB_BUILTIN,RGB_BRIGHTNESS,0,0); // Red

  myData.XYZ[0]=X;myData.XYZ[1]=X>>8;
  myData.XYZ[2]=Y;myData.XYZ[3]=Y>>8;
  myData.XYZ[4]=Z;myData.XYZ[5]=Z>>8;
  //Serial.printf("%x%x",X,(myData.XYZ[1])<<8|myData.XYZ[0]);
  //Serial.println((myData.XYZ[1])<<8|myData.XYZ[0]);
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  if (result == ESP_OK) {
    //Serial.println("Sent with success");
  }
  else {
    //Serial.println("Error sending the data");
  }
  delay(500);
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
