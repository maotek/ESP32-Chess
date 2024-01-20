#include "Board.h"
#include "TFT_eSPI.h"
#include <Arduino.h>
#include "pieces.h"
#include "pieceDefs.h"

extern TFT_eSprite sprite;
extern TFT_eSprite tmpSpr;

extern bool yourTurn;
extern uint8_t color;

uint32_t turnTimer;
#define TURN_INDICATOR_INTERVAL 500

// false = black, true = white
void Board::init(bool color) {
  if (color) {
    for (int i = 0; i < 8; i++) {
      board[0][i] = i + 9;
      board[1][i] = i + 1;
      board[6][i] = i + 17;
      board[7][i] = i + 25;
    }
  } else {
    for (int i = 0; i < 8; i++) {
      board[0][i] = i + 25;
      board[1][i] = i + 17;
      board[6][i] = i + 1;
      board[7][i] = i + 9;

      // Swap queen and king for black
      board[0][3] = 29;
      board[0][4] = 28;
      board[7][3] = 13;
      board[7][4] = 12;
    }
  }
}

void Board::drawBoard(void) {
  drawTiles();
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      if (board[y][x] == 0) continue;
      drawPiece(board[y][x], x, y);
    }
  }
}

void Board::drawTiles(void) {
  sprite.fillScreen(0x5acb);
  for (int j = 0; j < 8; j++) {
    for (int i = j % 2 == 0 ? 1 : 0; i < 8; i += 2) {
      sprite.fillRect(i * 30, j * 30, 30, 30, 0x0000);
    }
  }
};

void Board::drawPiece(uint8_t piece, uint32_t x, uint32_t y) {
  // https://github.com/Bodmer/TFT_eSPI/discussions/1160
  tmpSpr.pushImage(0, 0, 30, 30, pieceToBmp(piece));
  tmpSpr.pushToSprite(&sprite, x * 30, y * 30, TFT_WHITE);
}

void Board::draw(void) {
  sprite.pushSprite(0, 0);
}

uint16_t *Board::pieceToBmp(uint8_t num) {
  switch (num) {
    case 1 ... 8:
      return b_pawn;
      break;
    case 9:
    case 16:
      return b_rook;
      break;
    case 10:
    case 15:
      return b_knight;
      break;
    case 11:
    case 14:
      return b_bishop;
      break;
    case 12:
      return b_queen;
      break;
    case 13:
      return b_king;
      break;
    case 17 ... 24:
      return w_pawn;
      break;
    case 25:
    case 32:
      return w_rook;
      break;
    case 26:
    case 31:
      return w_knight;
      break;
    case 27:
    case 30:
      return w_bishop;
      break;
    case 28:
      return w_queen;
      break;
    case 29:
      return w_king;
      break;
  }
}

void Board::movePiece(uint8_t src_x, uint8_t src_y, uint8_t dest_x, uint8_t dest_y) {
  board[dest_y][dest_x] = board[src_y][src_x];
  board[src_y][src_x] = 0;
}

void Board::moveOpponentPiece(uint8_t src_x, uint8_t src_y, uint8_t dest_x, uint8_t dest_y) {
  // en passant check
  if (board[7 - dest_y][7 - dest_x] == 0 && 7 - dest_x != 7 - src_x) {
    Serial.println("enemy en passanted");
    board[7 - dest_y - 1][7 - dest_x] = 0;
  }

  board[7 - dest_y][7 - dest_x] = board[7 - src_y][7 - src_x];
  board[7 - src_y][7 - src_x] = 0;


}

void Board::drawCursor(uint8_t x, uint8_t y, uint32_t color) {
  sprite.drawRect(x * 30, y * 30, 30, 30, color);
}

void Board::resetBoard(void) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      board[i][j] = 0;
    }
  }
}

void Board::drawTurn() {
  if (millis() - turnTimer > TURN_INDICATOR_INTERVAL) {
    if (yourTurn) {
      sprite.drawLine(0, 239, 239, 239, TFT_GREEN);
    } else {
      sprite.drawLine(0, 0, 239, 0, TFT_GREEN);
    }
  }

  if (millis() - turnTimer > TURN_INDICATOR_INTERVAL * 2) turnTimer = millis();
}