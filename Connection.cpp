#include "Connection.h"
#include "WiFi.h"
#include <Arduino.h>

struct_message receive_data;
struct_message send_data;

extern uint8_t state;

bool Connection::delivered = false;
bool Connection::received = false;


// Mac addresses for ESPNow
// uint8_t broadcastAddress[6] = {0xA8, 0x42, 0xE3, 0x48, 0xDA, 0x0C}; // usbc
uint8_t broadcastAddress[6] = { 0xC8, 0xF0, 0x9E, 0xF3, 0x7B, 0xA4 };  // usb micro


void Connection::data_sent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  delivered = status == ESP_NOW_SEND_SUCCESS ? true : false;
}


void Connection::data_receive(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&receive_data, incomingData, sizeof(receive_data));
  // Serial.print("Bytes received: ");
  // Serial.println(len);
  // Serial.println("Data: " + String(receive_data.src_x) + " " + String(receive_data.src_y) + " " +
  // String(receive_data.dest_x) + " " + String(receive_data.dest_y));
  // if (receive_data.src_x == 255 && receive_data.src_y == 255 && receive_data.dest_x == 255 && receive_data.dest_y == 255) return;  // ignore heart beat
  received = true;
}

bool Connection::sendData(uint8_t src_x, uint8_t src_y, uint8_t dest_x, uint8_t dest_y) {
  send_data.src_x = src_x;
  send_data.src_y = src_y;
  send_data.dest_x = dest_x;
  send_data.dest_y = dest_y;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&send_data, sizeof(send_data));
  return result == ESP_OK;
}


bool Connection::init(void) {
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return false;
  }
  esp_now_register_send_cb(data_sent);
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return false;
  }
  esp_now_register_recv_cb(data_receive);
  Serial.println("Success setting up ESPNow");
  connected = true;
  return true;
}

// bool Connection::heartBeat()
// {
//   sendData(255, 255, 255, 255);
//   vTaskDelay(pdMS_TO_TICKS(20));
//   if (delivered)
//   {
//     delivered = false;
//     return true;
//   }
//   return false;
// }