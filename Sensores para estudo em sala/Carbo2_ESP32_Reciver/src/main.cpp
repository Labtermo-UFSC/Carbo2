#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

hw_timer_t * timer = NULL;
#define TIMERTick_Hz 10
bool timerFlag = false;

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    unsigned char id;
    unsigned short ch1;
    unsigned short ch2;
    unsigned short ch3;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Create a structure to hold the readings from each board
struct_message board1;
struct_message board2;
struct_message board3;
struct_message board4;

// Create an array with all the structures
struct_message boardsStruct[4] = {board1, board2, board3, board4};

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  boardsStruct[myData.id-1].ch1 = myData.ch1;
  boardsStruct[myData.id-1].ch2 = myData.ch2;
  Serial.printf("CO2 value: %d \n", boardsStruct[myData.id-1].ch1);
  Serial.printf("TEMP value: %d \n", boardsStruct[myData.id-1].ch2);
  Serial.println();
}

void sendToSerial(){
  Serial.println("Sending data to software");
  Serial.printf("%d;%d;%d;%d\n", boardsStruct[0].ch1, boardsStruct[1].ch1, boardsStruct[2].ch1, boardsStruct[3].ch1);
  for (int i = 0; i < 4; i++) {
    boardsStruct[i].ch1 = 0;
    boardsStruct[i].ch2 = 0;
    boardsStruct[i].ch3 = 0;
  }

}

void IRAM_ATTR onTimer() {
  timerFlag = true;
}
 
void setup() {
  //Initialize Serial Monitor
  Serial.begin(115200);
  Serial.println("ESPNow Reciver");
  Serial.println();
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    esp_restart();
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

  // Initiate timer
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, (1000000 * TIMERTick_Hz), true);
  timerAlarmEnable(timer);
}
 
void loop() {
  if (timerFlag) {
    timerFlag = false;
    sendToSerial();
  }
}