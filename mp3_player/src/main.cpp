#include <Arduino.h>
#include <Bounce2.h>
#include "display_ui.h"

// Pins
const int PIN_PLAY = 2;
const int PIN_NEXT = 3;
const int PIN_PREV = 4;
const int PIN_MENU = 5;
const int PIN_SCREEN = 6;

// Buttons
Bounce btn_play = Bounce();
Bounce btn_next = Bounce();
Bounce btn_prev = Bounce();
Bounce btn_menu = Bounce();
Bounce btn_screen = Bounce();

// The Brain
enum DeviceState { STATE_PLAYER, STATE_MENU };
DeviceState currentState = STATE_PLAYER;

bool Song_playing = false;

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for you to open the Serial Monitor

  initDisplay(); // Wake up the OLED!

  Serial.println("--- SYSTEM BOOT ---");
  Serial.println("Current State: PLAYER");

  btn_play.attach(PIN_PLAY, INPUT_PULLUP);
  btn_play.interval(10);
  btn_next.attach(PIN_NEXT, INPUT_PULLUP);
  btn_next.interval(10);
  btn_prev.attach(PIN_PREV, INPUT_PULLUP);
  btn_prev.interval(10);
  btn_menu.attach(PIN_MENU, INPUT_PULLUP);
  btn_menu.interval(10);
  btn_screen.attach(PIN_SCREEN, INPUT_PULLUP);
  btn_screen.interval(10);
}

void loop() {
  btn_play.update();
  btn_next.update();
  btn_prev.update();
  btn_menu.update();
  btn_screen.update();

  switch (currentState) {
    
    // -----------------------------------
    // IF WE ARE ON THE MUSIC PLAYER PAGE
    // -----------------------------------
    case STATE_PLAYER:
    drawPlayerScreen("Song Title", 75, Song_playing);

      if (btn_play.fell()) {
        Serial.println("[PLAYER] Action: Toggled Play/Pause");
        Song_playing = !Song_playing;
      }
      if (btn_next.fell()) {
        Serial.println("[PLAYER] Action: Skipped Song");
      }
      if (btn_prev.fell()) {
        Serial.println("[PLAYER] Action: Rewound Song");
      }
      if (btn_menu.fell()) {
        Serial.println("[TRANSITION] Leaving Player -> Entering Menu");
        currentState = STATE_MENU;
      }
      break;

    // -----------------------------------
    // IF WE ARE ON THE MENU PAGE
    // -----------------------------------
    case STATE_MENU:
      if (btn_next.fell()) {
        Serial.println("[MENU] Action: Scrolled Down");
      }
      if (btn_play.fell()) {
        Serial.println("[MENU] Action: Selected Folder");
      }
      if (btn_menu.fell()) {
        Serial.println("[TRANSITION] Leaving Menu -> Entering Player");
        currentState = STATE_PLAYER;
      }
      if (btn_prev.fell()) {
        Serial.println("[MENU] Action: Exiting Folder");
      }
      break;
  
  }
  if (btn_screen.fell()) {
      Serial.println("[SCREEN] Action: Screen Toggled");
    }
}