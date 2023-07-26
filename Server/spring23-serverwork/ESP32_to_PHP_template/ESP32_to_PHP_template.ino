#include <WiFi.h>
#include <HTTPClient.h>

const char WIFI_SSID[] = "NETGEAR";
const char WIFI_PASSWORD[] = "";

// http://mdl-vm11.eng.rpi.edu
// /sendToDatabase.php
// ?MAC_addr=%2700-B0-D0-63-C2-26%27&&RGB_R=120&&RGB_G=121&&RGB_B=122&&height=183.1&&speed=0.5&&direction=90
String HOST_NAME = "http://mdl-vm11.eng.rpi.edu";  // change to your PC's IP address
String PATH_NAME = "/sendToDatabase.php";
String queryString = "?MAC_addr=%2700-B0-D0-63-C2-26%27&&RGB_R=120&&RGB_G=121&&RGB_B=122&&height=183.1&&speed=0.5&&direction=90";

void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  HTTPClient http;

  http.begin(HOST_NAME + PATH_NAME + queryString);  //HTTP
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) { // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
    else { // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    }
  }
  else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

void loop() {

}
