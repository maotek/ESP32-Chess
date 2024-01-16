#include <esp_now.h>

typedef struct struct_message {
  uint8_t src_x;
  uint8_t src_y;
  uint8_t dest_x;
  uint8_t dest_y;
} struct_message;

class Connection {
private:
  static void data_sent(const uint8_t *mac_addr, esp_now_send_status_t status);
  static void data_receive(const uint8_t *mac, const uint8_t *incomingData, int len);

public:
  static bool delivered;
  static bool received;

  bool connected = 0;
  bool sendData(uint8_t src_x, uint8_t src_y, uint8_t dest_x, uint8_t dest_y);
  bool init(void);
  // bool heartBeat(void);
};