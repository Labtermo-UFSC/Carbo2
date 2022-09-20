/*
Made by Lucas Paiva da Silva 
Inspired by https://randomnerdtutorials.com/esp-now-many-to-one-esp32/
UFSC - 2022
*/

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "MHZ19.h"

//#define SensorNumber1
#define SensorNumber2
//#define SensorNumber3
//#define SensorNumber4

// Sensor 1 to 3 are CO2 sensors with thermistor support
// Sensor 4 is a sensor for measuring wind speed (indoor)

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


hw_timer_t * timer = NULL;
#define TIMERTick_Hz 5
bool timerFlag = false;

#define MHZBAUDRATE 9600 
MHZ19 mhz; 


// ESP com antena
// 40:91:51:9B:A9:E0

typedef struct struct_message {
    unsigned char id;
    unsigned short ch1;
    unsigned short ch2;
    unsigned short ch3;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

void IRAM_ATTR onTimer() {
  timerFlag = true;
}

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void collectAndSendData(){
  myData.id = GlobalId;
  myData.ch1 = mhz.getCO2();
  Serial.println(myData.ch1);
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(adressToBroadcastTo, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
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
  // Initiate timer
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, (1000000 * TIMERTick_Hz), true);
  timerAlarmEnable(timer);
}
 
void loop() {
  if (timerFlag) {
    timerFlag = false;
    collectAndSendData();
  }
}