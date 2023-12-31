#include "states.h"
#include <stdint.h>
#include <Arduino.h>
#include "Board.h"
#include "Buttons.h"
#include "Connection.h"
#include "ButtonMap.h"

extern uint8_t state;
extern Board board;
extern Connection connection;

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

uint8_t menuPresses = 0;

#define DEBOUNCE_TIME 50

void handleButtons()
{
  if (millis() - buttonDownTime > DEBOUNCE_TIME && digitalRead(BUTTON_DOWN) == HIGH)
  {
    Serial.println("Detected Down");
    down_handle();
  }
  if (millis() - buttonUpTime > DEBOUNCE_TIME && digitalRead(BUTTON_UP) == HIGH)
  {
    Serial.println("Detected Up");
    up_handle();
  }
  if (millis() - buttonLeftTime > DEBOUNCE_TIME && digitalRead(BUTTON_LEFT) == HIGH)
  {
    Serial.println("Detected Left");
    left_handle();
  }
  if (millis() - buttonRightTime > DEBOUNCE_TIME && digitalRead(BUTTON_RIGHT) == HIGH)
  {
    Serial.println("Detected Right");
    right_handle();
  }
  if (millis() - buttonMenuTime > DEBOUNCE_TIME && digitalRead(BUTTON_MENU) == HIGH)
  {
    Serial.println("Detected Menu");
    menuPresses++;
    // menu_handle();
  }

  if(millis() - buttonMenuTime > 500)
  {
    // Serial.println(String(menuPresses));
    if(menuPresses == 1 && digitalRead(BUTTON_MENU) == LOW)
    {
      Serial.println("Pressed Once");
      menu_handle();
    }
    else if(menuPresses == 1 && digitalRead(BUTTON_MENU) == HIGH)
    {
      Serial.println("Longpress");
      menu_long_handle();
    }
    menuPresses = 0;
  }
}

void down_handle()
{
  switch (state)
  {
    case STATE_INGAME:
        cur_y = (cur_y + 1) % 8;
      break;
  }
}

void up_handle()
{
  switch (state)
  {
    case STATE_INGAME:
      cur_y = (cur_y - 1 + 8) % 8;
      break;
  }
}

void left_handle()
{
  switch (state)
  {
    case STATE_INGAME:
      cur_x = (cur_x - 1 + 8) % 8;
      break;
  }
}

void right_handle()
{
  switch (state)
  {
    case STATE_INGAME:
      cur_x = (cur_x + 1) % 8;
      break;
  }
}
void menu_long_handle()
{
  switch (state)
  {
    case STATE_START:
      // TODO
      break;
    case STATE_INGAME:
      if (yourTurn)
      {
        selected = false;
        board.board[prev_y][prev_x] = prevPiece;
        cur_x = prev_x;
        cur_y = prev_y;
        selectedPiece = 0;
      }
      break;
  }
}

void menu_handle()
{
  switch (state)
  {
    case STATE_START:
      state = STATE_CONNECT;
      break;
    case STATE_INGAME:
      if (yourTurn)
      {
        if (!selected)
        {
          selected = true;
          selectedPiece = board.board[cur_y][cur_x];
          board.board[cur_y][cur_x] = 0;
          prev_x = cur_x;
          prev_y = cur_y;
          prevPiece = selectedPiece;
        }
        else
        {
          Serial.println("released");
          board.board[cur_y][cur_x] = selectedPiece;
          selected = false;
          selectedPiece = 0;

          connection.sendData(prev_x, prev_y, cur_x, cur_y);
          yourTurn = !yourTurn;
        }
      }
      break;
  }
}

void down_handler() 
{
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