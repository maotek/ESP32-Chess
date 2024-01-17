#include "Engine.h"
#include "Board.h"
#include "Arduino.h"

extern Board board;
extern bool cannotSelect;
extern uint32_t cannotSelectTime;

bool Engine::checkValidMove(uint8_t src_x, uint8_t src_y, uint8_t dest_x, uint8_t dest_y, uint8_t **board, uint8_t color, uint8_t piece)
{
  // TODO: implement move check
  switch (piece) {
    case 1 ... 8:
      
      break;
    case 9:
    case 16:
      
      break;
    case 10:
    case 15:
      
      break;
    case 11:
    case 14:
      
      break;
    case 12:
      
      break;
    case 13:
      
      break;
    case 17 ... 24:
      
      break;
    case 25:
    case 32:
      
      break;
    case 26:
    case 31:
      
      break;
    case 27:
    case 30:
      
      break;
    case 28:
      
      break;
    case 29:
      
      break;
  }
}

bool Engine::checkValidSelect(uint8_t cur_x, uint8_t cur_y, uint8_t color) {
  // If the cursor is not on any piece
  if (board.board[cur_y][cur_x] == 0) {
    cannotSelect = true;
    cannotSelectTime = millis();
    return false;
  };
  // If the cursor is on an enemy piece
  if (color == 0) {
    if (board.board[cur_y][cur_x] >= 17) {
      cannotSelect = true;
      cannotSelectTime = millis();
      return false;
    };
  } else {
    if (board.board[cur_y][cur_x] < 17) {
      cannotSelect = true;
      cannotSelectTime = millis();
      return false;
    };
  }

  return true;
}