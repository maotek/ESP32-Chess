#include <stdint.h>

class Engine {
public:
  bool checkValidMove(uint8_t src_x, uint8_t src_y, uint8_t dest_x, uint8_t dest_y, uint8_t color, uint8_t piece);
  bool checkValidSelect(uint8_t cur_x, uint8_t cur_y, uint8_t color);
};