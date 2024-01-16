#include "Menu.h"
#include "TFT_eSPI.h"
#include "WiFi.h"

extern TFT_eSprite sprite;

uint32_t dotTime = 0;
uint8_t dots = 0;
uint8_t batteryLevel = 80;

extern uint8_t state;

void Menu::drawStartMenu() {
  sprite.fillScreen(0x0000);
  sprite.setTextSize(2);
  sprite.drawString("MaoTek Chess V1.0.0", 2, 2);
  sprite.drawString("Press to start", 40, 110);
  sprite.setTextSize(1);
  sprite.drawString("Battery: " + String(batteryLevel) + " %", 2, 220);
  sprite.drawString("Mac: " + WiFi.macAddress(), 2, 230);
  sprite.pushSprite(0, 0);
}

void Menu::drawConnectMenu() {
  sprite.fillScreen(0x0000);
  sprite.setTextSize(2);
  if (millis() - dotTime > 500) {
    dotTime = millis();
    dots = (dots + 1) % 4;
  }

  String str = "";
  for (int i = 0; i < dots; i++) str += ".";
  sprite.drawString("Connecting" + str, 50, 110);
  sprite.pushSprite(0, 0);
}

void Menu::drawInGameMenu() {
  sprite.fillScreen(0x0000);
  sprite.pushSprite(0, 0);
}