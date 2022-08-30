#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "MHZ19.h"

#define SensorNumber1
//#define SensorNumber2
//#define SensorNumber3
//#define SensorNumber4
//#define SensorNumber5
//#define SensorNumber6

#ifdef SensorNumber1
  #define GlobalId 1
  #define GlobalRXMhz19 16
  #define GlobalTXMhz19 17
#endif
#ifdef SensorNumber2
  #define GlobalId 2
  #define GlobalRXMhz19 16
  #define GlobalTXMhz19 17
#endif
#ifdef SensorNumber3
  #define GlobalId 3
  #define GlobalRXMhz19 16
  #define GlobalTXMhz19 17
#endif
#ifdef SensorNumber4
  #define GlobalId 4
  #define GlobalRXMhz19 16
  #define GlobalTXMhz19 17
#endif
#ifdef SensorNumber5
  #define GlobalId 5
  #define GlobalRXMhz19 16
  #define GlobalTXMhz19 17
#endif
#ifdef SensorNumber6
  #define GlobalId 6
  #define GlobalRXMhz19 16
  #define GlobalTXMhz19 17
#endif

#define MHZBAUDRATE 9600 
MHZ19 mhz; 
uint8_t adressToBroadcastTo[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

typedef struct struct_message {
    int id;
    int co2;
    float temperature;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); // WiFi mode is station
  Serial2.begin(MHZBAUDRATE, SERIAL_8N1, GlobalRXMhz19, GlobalTXMhz19); // Serial2 is the MH-Z19 serial port
  mhz.begin(Serial2);
  mhz.autoCalibration();
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    esp_restart();
  }
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  // Register peer
  memcpy(peerInfo.peer_addr, adressToBroadcastTo, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    esp_restart();
  }
}
 
void loop() {
  myData.id = GlobalId;
  myData.co2 = mhz.getCO2();
  myData.temperature = mhz.getTemperature();

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(adressToBroadcastTo, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(5000);
}