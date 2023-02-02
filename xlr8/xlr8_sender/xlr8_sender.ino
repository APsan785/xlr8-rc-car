#include <esp_now.h>
#include <WiFi.h>
#include <stdio.h>
#include <math.h>

//TODO : define x axis, y axis , switch pins

#define X_AXIS_PIN 34
#define Y_AXIS_PIN 33
#define SWITCH_PIN 32

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t receiverMacAddress[] = {0x08,0xB6,0x1F,0x3B,0x48,0xBC};  //AC:67:B2:36:7F:28
uint8_t broadcastAddress1[] = {0xC0, 0x49, 0xEF, 0xCB, 0x83, 0x84}; // sender's address

struct PacketData
{
  byte xAxisValue;
  byte yAxisValue;
  byte switchPressed;
};
PacketData data;


// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t ");
  Serial.println(status);
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Message sent" : "Message failed");
}

void initPinModes(){

  pinMode(SWITCH_PIN, INPUT_PULLUP);   
  pinMode(X_AXIS_PIN, INPUT);
  pinMode(Y_AXIS_PIN, INPUT);
  // digitalWrite(X_AXIS_PIN, HIGH);
  // digitalWrite(Y_AXIS_PIN, HIGH);

}

void setup() {

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  initPinModes();

 // Init ESP-NOW
  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  else
  {
    Serial.println("Succes: Initialized ESP-NOW");
  }

  /*
  
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  esp_now_register_send_cb(OnDataSent);

   // Register peer
  esp_now_add_peer(broadcastAddress1, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

    // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  else
  {
    Serial.println("Succes: Added peer");
  } 

  pinMode(SWITCH_PIN, INPUT_PULLUP);  

  */

  

    esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMacAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  else
  {
    Serial.println("Succes: Added peer");
  } 

}

void loop() {
  
  data.xAxisValue = map(analogRead(X_AXIS_PIN), 0, 4095, 0, 255);
  data.yAxisValue =  map(analogRead(Y_AXIS_PIN), 0, 4095, 0, 255);  
  data.switchPressed = false; 

  if (digitalRead(SWITCH_PIN) == LOW)
  {
    data.switchPressed = true;
  }
  
  esp_err_t result = esp_now_send(receiverMacAddress, (uint8_t *) &data, sizeof(data));
  if (result == ESP_OK) 
  {
    Serial.println("Sent with success");
    Serial.println("Y axis : ");
    Serial.println(analogRead(Y_AXIS_PIN));
    Serial.println(data.yAxisValue);
    // Serial.print("HEY");
    // Serial.println("Y axis : ");
    // Serial.print(analogRead(Y_AXIS_PIN));
    // Serial.print("HEY");
    // Serial.println("Y axis : " + analogRead(Y_AXIS_PIN) + std::string(" GOO"));
  }
  else 
  {
    Serial.println("Error sending the data");
  }    
  
  if (data.switchPressed == true)
  {
    delay(500);
  }
  else
  {
    delay(50);
  }

}
