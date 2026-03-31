#include <Arduino.h>

// 1. Define your States
enum DeviceState {
  STATE_PLAYER,
  STATE_MENU
};

// Start the device in the Player screen
DeviceState currentState = STATE_PLAYER;

void setup() {
  Serial.begin(9600);
  
  // Initialize your hardware here (Buttons, Screen, SD Card, Audio Shield)
  // pinMode(buttonPin, INPUT_PULLUP);
  // display.begin();
  // etc...
}

void loop() {
  // 2. Read all your inputs FIRST, every single loop
  // int sliderValue = analogRead(sliderPin);
  // bool playPressed = digitalRead(playButtonPin);
  // bool menuPressed = digitalRead(menuButtonPin);

  // 3. Handle state switching
  /* Example:
  if (menuPressed) {
    if (currentState == STATE_PLAYER) {
      currentState = STATE_MENU;
    } else {
      currentState = STATE_PLAYER;
    }
  }
  */

  // 4. Execute logic based on the CURRENT state
  switch (currentState) {
    
    case STATE_PLAYER:
      // What happens in the Player?
      // - Slider controls the Audio chip volume
      // - Screen displays current track name
      // - Next/Prev buttons skip tracks
      break;

    case STATE_MENU:
      // What happens in the Menu?
      // - Slider controls the highlighted list item (with soft-takeover!)
      // - Screen displays the SD card folder list
      // - Next/Prev buttons scroll up and down
      break;
  }
  
  // 5. Update the physical screen LAST
  // updateScreen(); 
}