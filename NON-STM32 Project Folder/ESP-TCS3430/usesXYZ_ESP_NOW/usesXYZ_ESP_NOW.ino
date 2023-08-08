#include <esp_now.h>
#include <WiFi.h>
#include <math.h>

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  uint8_t XYZ[6];
} struct_message;
struct_message myData;



void setup(){
  //Setup serial, wifi connection, and sensor connection
  Serial.begin(115200);
  Serial.println();
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop(){

}
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  //Serial.print(len);

  //Format data
  //Serial.print(" Bytes");
  uint16_t X = (myData.XYZ[1])<<8|myData.XYZ[0];
  uint16_t Y = (myData.XYZ[3])<<8|myData.XYZ[2];
  uint16_t Z = (myData.XYZ[5])<<8|myData.XYZ[4];  
  uint16_t IR1Data = 0;
  /*
  Serial.print("\tX: ");
  Serial.print(int(X));
  Serial.print("\tY: ");
  Serial.print(int(Y));
  Serial.print("\tZ: ");
  Serial.print(int(Z));
  */
  X=0.8*X;
  Z=3*Z;

  //Calculate color values
  double R =  3.2404542*X - 1.5371385*Y - 0.4985314*Z;
  double G = -0.9692660*X + 1.8760108*Y + 0.0415560*Z;
  double B =  0.0556434*X - 0.2040259*Y + 1.0572252*Z;

  //Parse serial string
  String str = "X : " + String(X) + "    Y : " + String(Y) + "    Z : " +  String(Z) + "    IR1 : "+String(IR1Data)
  + "    R : " +  String(int(R)) + "    G : " +  String(int(G)) + "    B : " +  String(int(B)) ;
  double big = max(R,G);
  big = max(big, B);
  double ratio = (big)/255;
  ratio = ratio*6;
  /*
  Serial.print("\tbig: ");
  Serial.print(int(big));
  Serial.print("\tratio: ");
  Serial.print(ratio);*/
  int r = (int)(((float)R)/ratio);
  int g = (int)(((float)G)/ratio);
  int b = (int)(((float)B)/ratio);
  if(r<0){r=0;}
  if(g<0){g=0;}
  if(b<0){b=0;}
  /*
  Serial.print("\tr: ");
  Serial.print(r);
  Serial.print("\tg: ");
    Serial.print(g);
  Serial.print("\tB: ");
    Serial.print(b);
    */
  Serial.print(str+"\n");
    
  neopixelWrite(RGB_BUILTIN,r,g,b); // Red
}
