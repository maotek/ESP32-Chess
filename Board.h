#include <stdint.h>

class Board {
private:
  void drawTiles(void);
  uint16_t *pieceToBmp(uint8_t num);

public:
  void drawPiece(uint8_t piece, uint32_t x, uint32_t y);
  uint8_t board[8][8] = { 0 };
  void init(bool color);
  void drawBoard(void);
  void draw(void);
  void drawCursor(uint8_t x, uint8_t y, uint32_t color);
  void movePiece(uint8_t src_x, uint8_t src_y, uint8_t dest_x, uint8_t dest_y);
  void resetBoard(void);
  void moveOpponentPiece(uint8_t src_x, uint8_t src_y, uint8_t dest_x, uint8_t dest_y);
  void drawTurn(void);
};