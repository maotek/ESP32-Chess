#include "Engine.h"
#include "Board.h"
#include "Arduino.h"

extern Board board;
extern bool cannotSelect;
extern uint32_t cannotSelectTime;

extern uint8_t enemyLastSrc_x;
extern uint8_t enemyLastSrc_y;
extern uint8_t enemyLastDest_x;
extern uint8_t enemyLastDest_y;

bool Engine::checkValidMove(uint8_t src_x, uint8_t src_y, uint8_t dest_x, uint8_t dest_y, uint8_t color, uint8_t piece) {
  bool goodMove = true;
  // TODO: implement move check
  Serial.print(enemyLastSrc_x);
  Serial.print(enemyLastSrc_y);
  Serial.print(enemyLastDest_x);
  Serial.println(enemyLastDest_y);
  switch (piece) {
    // pawns
    case 1 ... 8:
    case 17 ... 24:
      goodMove = false;
      if (src_x == dest_x && dest_y == src_y - 1 && board.board[dest_y][dest_x] == 0) goodMove = true;             // one step
      else if (src_x == dest_x && dest_y == 4 && src_y == 6 && board.board[dest_y + 1][dest_x + 1] == 0 && board.board[dest_y][dest_x] == 0) goodMove = true;  // two step

      else if (dest_x == src_x + 1 && dest_y == src_y - 1 && enemyLastSrc_y == 1 && enemyLastDest_y == 3 && enemyLastSrc_x == src_x + 1) {  // en passant
        // remove piece at player who played en passant
        board.board[dest_y + 1][dest_x] = 0;
        goodMove = true;
      } else if (dest_x == src_x - 1 && dest_y == src_y - 1 && enemyLastSrc_y == 1 && enemyLastDest_y == 3 && enemyLastSrc_x == src_x - 1) {  // en passant
        board.board[dest_y + 1][dest_x] = 0;
        goodMove = true;
      }

      else if ((dest_x == src_x + 1 || dest_x == src_x - 1) && dest_y == src_y - 1) {  // pawn takes an enemy piece
        if (color == 0) {
          if (board.board[dest_y][dest_x] >= 17) goodMove = true;
        } else {
          if (board.board[dest_y][dest_x] < 17 && board.board[dest_y][dest_x] > 0) goodMove = true;
        }
      }
      break;

    // rooks
    case 9:
    case 16:
    case 25:
    case 32:
      goodMove = true;
      // going up / down
      if (src_x == dest_x && dest_y != src_y) {
        uint8_t min = dest_y < src_y ? dest_y : src_y;
        uint8_t max = dest_y > src_y ? dest_y : src_y;
        for (int i = min; i < max; i++) {
          if (i == dest_y) continue; // we check the destination at the end of the case
          if (board.board[i][src_x] != 0) goodMove = false;
        }
      }

      // going left / right
      else if (src_x != dest_x && dest_y == src_y) {
        uint8_t min = dest_x < src_x ? dest_x : src_x;
        uint8_t max = dest_x > src_x ? dest_x : src_x;
        for (int i = min; i < max; i++) {
          if (board.board[src_y][i] != 0) goodMove = false;
        }
      }

      else {
        goodMove = false;
      }

      // check destination
      if (color == 0) {
        if (board.board[dest_y][dest_x] < 17 && board.board[dest_y][dest_x] > 0) goodMove = false;
      } else {
        if (board.board[dest_y][dest_x] >= 17) goodMove = false;
      }
      break;

    // knights
    case 10:
    case 15:
    case 26:
    case 31:
      goodMove = true;

      if (!((abs(dest_y - src_y) == 1 && abs(dest_x - src_x) == 2) || (abs(dest_x - src_x) == 1 && abs(dest_y - src_y) == 2))) goodMove = false;

      // check destination
      if (color == 0 && board.board[dest_y][dest_x] != 0) {
        if (board.board[dest_y][dest_x] < 17 && board.board[dest_y][dest_x] > 0) goodMove = false;
      } else if (color == 1 && board.board[dest_y][dest_x] != 0) {
        if (board.board[dest_y][dest_x] >= 17) goodMove = false;
      }
      break;


    // bishops
    case 11:
    case 14:
    case 27:
    case 30:
      goodMove = true;

      if (!(abs(dest_y - src_y) == abs(dest_x - src_x))) {
        goodMove = false;
      }

      else if (abs(dest_y - src_y) == abs(dest_x - src_x)) {

        int8_t x_dir = dest_x < src_x ? -1 : 1;
        int8_t y_dir = dest_y < src_y ? -1 : 1;
        uint8_t x = src_x + x_dir;
        uint8_t y = src_y + y_dir;
        while (x != dest_x && y != dest_y) {
          if (board.board[y][x] != 0) {
            goodMove = false;
            break;
          }
          x += x_dir;
          y += y_dir;
        }
      }

      // check destination
      if (color == 0 && board.board[dest_y][dest_x] != 0) {
        if (board.board[dest_y][dest_x] < 17 && board.board[dest_y][dest_x] > 0) goodMove = false;
      } else if (color == 1 && board.board[dest_y][dest_x] != 0) {
        if (board.board[dest_y][dest_x] >= 17) goodMove = false;
      }
      break;

    // queen
    case 12:
    case 28:

      break;

    // king
    case 13:
    case 29:
      goodMove = true;

      if (!(abs(dest_y - src_y) <= 1 && abs(dest_x - src_x) <= 1)) {
        goodMove = false;
      }

      // check destination
      if (color == 0 && board.board[dest_y][dest_x] != 0) {
        if (board.board[dest_y][dest_x] < 17 && board.board[dest_y][dest_x] > 0) goodMove = false;
      } else if (color == 1 && board.board[dest_y][dest_x] != 0) {
        if (board.board[dest_y][dest_x] >= 17) goodMove = false;
      }
      break;
  }

  if (!goodMove) {
    // start the visual warning
    cannotSelect = true;
    cannotSelectTime = millis();
    return false;
  }
  return true;
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

bool Engine::checkKingIsCheck(uint8_t color) {

}