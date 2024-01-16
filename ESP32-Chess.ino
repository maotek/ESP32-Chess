#include "SPI.h"
#include "TFT_eSPI.h"
#include "pieceDefs.h"
#include "Menu.h"
#include "Board.h"
#include <esp_now.h>
#include "WiFi.h"
#include "Connection.h"
#include "states.h"
#include "Buttons.h"
#include "ButtonMap.h"

#define CONNECT_INTERVAL 1000
#define DRAW_INTERVAL 15 // ~60 FPS
// #define HEARTBEAT_INTERVAL 1000

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
TFT_eSprite tmpSpr = TFT_eSprite(NULL);

TaskHandle_t task_game;
TaskHandle_t task_con;

extern struct_message receive_data;

Board board;
Menu menu;
Connection connection;

uint8_t color;
uint8_t state = STATE_START;

uint32_t lastDrawTime = 0;
uint32_t connectTime = 0;
// uint32_t heartBeatTime = 0;
// uint8_t heartBeatMisses = 0;

bool yourTurn;
uint8_t cur_x = 3;
uint8_t cur_y = 6;
uint8_t prev_x;
uint8_t prev_y;
bool selected = false;
uint8_t selectedPiece;
uint8_t prevPiece;


void setup() {
  Serial.begin(115200);

  tft.init();
  tft.fillScreen(TFT_BLACK);
  // Color depth 8 bit otherwise too much ram is being used
  sprite.setColorDepth(8);
  sprite.createSprite(240, 240);
  tmpSpr.createSprite(30, 30);
  tmpSpr.setSwapBytes(true);

  xTaskCreatePinnedToCore(
    core0_task,
    "con",
    50000,     /* Stack size in words */
    NULL,      /* Task input parameter */
    0,         /* Priority of the task */
    &task_con, /* Task handle. */
    0);        /* Core where the task should run */

  xTaskCreatePinnedToCore(
    core1_task,
    "game",
    50000,      /* Stack size in words */
    NULL,       /* Task input parameter */
    1,          /* Priority of the task (HIGHER)*/
    &task_game, /* Task handle. */
    1);         /* Core where the task should run */

  pinMode(BUTTON_UP, INPUT_PULLDOWN);
  pinMode(BUTTON_DOWN, INPUT_PULLDOWN);
  pinMode(BUTTON_RIGHT, INPUT_PULLDOWN);
  pinMode(BUTTON_LEFT, INPUT_PULLDOWN);
  pinMode(BUTTON_MENU, INPUT_PULLDOWN);
  attachInterrupt(BUTTON_DOWN, down_handler, RISING);
  attachInterrupt(BUTTON_MENU, menu_handler, RISING);
  attachInterrupt(BUTTON_LEFT, left_handler, RISING);
  attachInterrupt(BUTTON_RIGHT, right_handler, RISING);
  attachInterrupt(BUTTON_UP, up_handler, RISING);
}

void loop() {
}

void core0_task(void *pvParameters) {

  (void)pvParameters;

  while (1) {

    if (millis() - lastDrawTime > DRAW_INTERVAL) {
      switch (state) {
        case STATE_START:
          menu.drawStartMenu();
          break;
        case STATE_CONNECT:
          menu.drawConnectMenu();
          break;
        case STATE_INGAME:
          board.drawBoard();
          board.drawTurn();

          if (selected) {
            board.drawPiece(selectedPiece, cur_x, cur_y);
            board.drawCursor(cur_x, cur_y, TFT_GREEN);
          } else {
            board.drawCursor(cur_x, cur_y, TFT_WHITE);
          }

          board.draw();
          break;
      }
      lastDrawTime = millis();
    }
    // vTaskDelay(pdMS_TO_TICKS(1));
  }
}

void core1_task(void *pvParameters) {

  (void)pvParameters;

  while (1) {
    handleButtons();

    switch (state) {
      case STATE_CONNECT:
        if (!connection.connected) {
          connection.init();
          break;
        }

        if (connection.received) {
          // Serial.println("Connected!, color is: " + String(color));
          connection.received = false;
          initGame(receive_data.dest_y == 0 ? 1 : 0);
          state = STATE_INGAME;
          break;
        }

        if (connection.delivered) {
          // Serial.println("Connected!, color is: " + String(color));
          initGame(color);
          state = STATE_INGAME;
          break;
        }

        if (millis() - connectTime > CONNECT_INTERVAL) {
          // Serial.println("Trying to Connect...");
          color = random(0, 2);
          connection.sendData(255, 255, 255, color);
          connectTime = millis();
        }
        break;

      case STATE_INGAME:
        if (connection.received) {
          // if other user started a game due to disconnection etc.
          if (receive_data.src_x == 255 && receive_data.src_y == 255 && receive_data.dest_x == 255) {
            initGame(receive_data.dest_y == 0 ? 1 : 0);
          } else {
            Serial.println("received");
            // TODO: when moved a piece, reset the prevpiece etc.
            board.moveOpponentPiece(receive_data.src_x, receive_data.src_y, receive_data.dest_x, receive_data.dest_y);
            yourTurn = !yourTurn;
          }
          connection.received = false; // clear flag
        }
        break;
    }


    // if (state == STATE_INGAME && millis() - heartBeatTime > HEARTBEAT_INTERVAL && connection.connected)
    // {
    //   if (!connection.heartBeat())
    //   {
    //     heartBeatMisses++;

    //     if (heartBeatMisses > 5)
    //     {
    //       Serial.println("Connection lost");
    //       connection.connected = false;
    //       esp_now_deinit();
    //       WiFi.mode(WIFI_OFF);
    //       board.resetBoard();
    //       state = STATE_START;
    //       cur_x = 3;
    //       cur_y = 6;
    //       prev_x = 0;
    //       prev_y = 0;
    //       prevPiece = 0;
    //       selectedPiece = 0;
    //       selected = false;
    //     }
    //   }
    //   else {
    //     heartBeatMisses = 0;
    //   }
    //   heartBeatTime = millis();
    // }

    // vTaskDelay(pdMS_TO_TICKS(1));
  }
}


void initGame(uint8_t color_) {
  color = color_;
  board.resetBoard();
  board.init(color);

  cur_x = 3;
  cur_y = 6;
  prev_x = 0;
  prev_y = 0;
  prevPiece = 0;
  selectedPiece = 0;
  selected = false;

  yourTurn = color == 0 ? false : true;
}