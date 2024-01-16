#include "states.h"
#include <stdint.h>
#include <Arduino.h>
#include "Board.h"
#include "Buttons.h"
#include "Connection.h"
#include "ButtonMap.h"
#include "Engine.h"

extern uint8_t state;
extern Board board;
extern Connection connection;

extern uint8_t color;

extern bool yourTurn;
extern uint8_t cur_x;
extern uint8_t cur_y;
extern uint8_t prev_x;
extern uint8_t prev_y;
extern bool selected;
extern uint8_t selectedPiece;
extern uint8_t prevPiece;

uint32_t buttonUpTime = 0;
uint32_t buttonDownTime = 0;
uint32_t buttonLeftTime = 0;
uint32_t buttonRightTime = 0;
uint32_t buttonMenuTime = 0;

uint32_t buttonUpPrevTime = 0;
uint32_t buttonDownPrevTime = 0;
uint32_t buttonLeftPrevTime = 0;
uint32_t buttonRightPrevTime = 0;
uint32_t buttonMenuPrevTime = 0;

bool buttonUpPressed = false;
bool buttonDownPressed = false;
bool buttonLeftPressed = false;
bool buttonRightPressed = false;
bool buttonMenuPressed = false;

uint8_t menuPresses = 0;

#define DEBOUNCE_TIME 85
#define AUTOSCROLL_DELAY 500
#define AUTOSCROLL_INTERVAL 200

void handleButtons() {
  if (digitalRead(BUTTON_UP) == LOW) buttonUpPressed = false;
  if (digitalRead(BUTTON_DOWN) == LOW) buttonDownPressed = false;
  if (digitalRead(BUTTON_LEFT) == LOW) buttonLeftPressed = false;
  if (digitalRead(BUTTON_RIGHT) == LOW) buttonRightPressed = false;
  if (digitalRead(BUTTON_MENU) == LOW) buttonMenuPressed = false;

  if (digitalRead(BUTTON_DOWN) == HIGH && millis() - buttonDownTime > DEBOUNCE_TIME) {
    if (!buttonDownPressed) {
      Serial.println("Detected Down");
      down_handle();
      buttonDownPressed = true;
    }
    // Hold the button at least 0.75s to activate fast scroll
    if (millis() - buttonDownTime > AUTOSCROLL_DELAY && millis() - buttonDownPrevTime > AUTOSCROLL_INTERVAL && buttonDownPressed) {
      down_handle();
      buttonDownPrevTime = millis();
    }
  }
  if (digitalRead(BUTTON_UP) == HIGH && millis() - buttonUpTime > DEBOUNCE_TIME) {
    if (!buttonUpPressed) {
      Serial.println("Detected Up");
      up_handle();
      buttonUpPressed = true;
    }
    if (millis() - buttonUpTime > AUTOSCROLL_DELAY && millis() - buttonUpPrevTime > AUTOSCROLL_INTERVAL && buttonUpPressed) {
      up_handle();
      buttonUpPrevTime = millis();
    }
  }
  if (digitalRead(BUTTON_LEFT) == HIGH && millis() - buttonLeftTime > DEBOUNCE_TIME) {
    if (!buttonLeftPressed) {
      Serial.println("Detected Left");
      left_handle();
      buttonLeftPressed = true;
    }
    if (millis() - buttonLeftTime > AUTOSCROLL_DELAY && millis() - buttonLeftPrevTime > AUTOSCROLL_INTERVAL && buttonLeftPressed) {
      left_handle();
      buttonLeftPrevTime = millis();
    }
  }
  if (digitalRead(BUTTON_RIGHT) == HIGH && millis() - buttonRightTime > DEBOUNCE_TIME) {
    if (!buttonRightPressed) {
      Serial.println("Detected Right");
      right_handle();
      buttonRightPressed = true;
    }
    if (millis() - buttonRightTime > AUTOSCROLL_DELAY && millis() - buttonRightPrevTime > AUTOSCROLL_INTERVAL && buttonRightPressed) {
      right_handle();
      buttonRightPrevTime = millis();
    }
  }
  if (digitalRead(BUTTON_MENU) == HIGH && millis() - buttonMenuTime > DEBOUNCE_TIME) {
    if (!buttonMenuPressed) {
      Serial.println("Detected Menu");
      menuPresses++;
      buttonMenuPressed = true;
    }
  }

  if (millis() - buttonMenuTime > 500) {
    // Serial.println(String(menuPresses));
    if (menuPresses == 1 && digitalRead(BUTTON_MENU) == LOW) {
      Serial.println("Pressed Once");
      menu_handle();
    } else if (menuPresses == 1 && digitalRead(BUTTON_MENU) == HIGH) {
      Serial.println("Longpress");
      menu_long_handle();
    } else if (menuPresses == 2 && digitalRead(BUTTON_MENU) == LOW) {
      Serial.println("Double tap");
    }
    menuPresses = 0;
  }
}

void down_handle() {
  switch (state) {
    case STATE_INGAME:
      cur_y = (cur_y + 1) % 8;
      break;
  }
}

void up_handle() {
  switch (state) {
    case STATE_INGAME:
      cur_y = (cur_y - 1 + 8) % 8;
      break;
  }
}

void left_handle() {
  switch (state) {
    case STATE_INGAME:
      cur_x = (cur_x - 1 + 8) % 8;
      break;
  }
}

void right_handle() {
  switch (state) {
    case STATE_INGAME:
      cur_x = (cur_x + 1) % 8;
      break;
  }
}
void menu_long_handle() {
  switch (state) {
    case STATE_START:
      // TODO
      break;
    case STATE_INGAME:
      if (yourTurn) {
        selected = false;
        board.board[prev_y][prev_x] = prevPiece;
        cur_x = prev_x;
        cur_y = prev_y;
        selectedPiece = 0;
      }
      break;
  }
}

void menu_handle() {
  switch (state) {
    case STATE_START:
      state = STATE_CONNECT;
      break;
    case STATE_INGAME:
      if (yourTurn) {
        if (!selected) {

          // If the cursor is not on any piece
          if (board.board[cur_y][cur_x] == 0) break;
          // If the cursor is on an enemy piece
          if (color == 0) {
            if (board.board[cur_y][cur_x] >= 17) break;
          } else {
            if (board.board[cur_y][cur_x] < 17) break;
          }

          selected = true;
          selectedPiece = board.board[cur_y][cur_x];
          board.board[cur_y][cur_x] = 0;
          prev_x = cur_x;
          prev_y = cur_y;
          prevPiece = selectedPiece;
        } else {
          // Serial.println("released");
          board.board[cur_y][cur_x] = selectedPiece;
          selected = false;
          selectedPiece = 0;

          // Validate move

          connection.sendData(prev_x, prev_y, cur_x, cur_y);

          delay(100);
          while (!connection.delivered) {
            delay(750);
            Serial.println("trying to send...");
            connection.sendData(prev_x, prev_y, cur_x, cur_y);
          }
          
          Serial.println("Move sent!");
          yourTurn = !yourTurn;
        }
      }
      break;
  }
}

void down_handler() {
  buttonDownTime = millis();
}
void up_handler() {
  buttonUpTime = millis();
}
void menu_handler() {
  buttonMenuTime = millis();
}
void left_handler() {
  buttonLeftTime = millis();
}
void right_handler() {
  buttonRightTime = millis();
}