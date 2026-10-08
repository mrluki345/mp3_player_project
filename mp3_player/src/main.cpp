#include <Arduino.h>
#include <SPI.h>
#include <Bounce2.h>
#include <U8g2lib.h>
#include <AudioStream.h>

#include "display_ui.h"
#include "hardware_io.h"
#include "audio_engine.h"
#include "core_pins.h"

// Pins
const int PIN_PLAY = 2;
const int PIN_NEXT = 3;
const int PIN_PREV = 4;
const int PIN_MENU = 5;
const int PIN_SCREEN = 6;
const int PIN_SLIDER = 14;
const int PIN_BATTERY = 15;

// Buttons
Bounce btn_play = Bounce();
Bounce btn_next = Bounce();
Bounce btn_prev = Bounce();
Bounce btn_menu = Bounce();
Bounce btn_screen = Bounce();

// The Brain
enum DeviceState { STATE_PLAYER, STATE_MENU };
DeviceState currentState = STATE_PLAYER;

float BatteryLevel = 0;
float LastBatteryLevel = 0;
unsigned long lastBatteryCheck = 0;

int currentVolume = 0;
unsigned long lastVolRead = 0; 

bool songPlaying = false;
int SongTime = 0; // in seconds, used for demo purposes to show how the menu navigation would work (e.g. navigating through folders and songs would change this variable to reflect the current song time)

int Menu_selected_item = 0;
int numberMenuItems = 3; // number of intems in the menu, used to limit the slider input for menu navigation

bool isScreenOn = true;
unsigned long lastDisplayUpdate = 0;

unsigned long lastUpdate = 0;
int currentFrequency = 100;

unsigned long loopCounter = 0;
unsigned long lastHeartbeat = 0;

void setup() {

  initDisplay(); // Wake up the OLED!
  initAudioEngine(); // Wake up the Audio Shield!

  Serial.begin(115200);
  //while (!Serial); // Wait for you to open the Serial Monitor
  Serial.println("DISPLAY OK");
  Serial.println("--- SYSTEM BOOT ---");
  Serial.println("--- BATTERY SENSOR TEST ---");
  

  Serial.println("Current State: PLAYER");

  // Initialize buttons with their respective pins and debounce intervals
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

//loopCounter++;
//  if (millis() - lastHeartbeat > 1000) { // Print exactly once per second
//    Serial.print("Loops per second: ");
//    Serial.println(loopCounter);
//    loopCounter = 0;
//    lastHeartbeat = millis();
//  }
  
  btn_play.update();
  btn_next.update();
  btn_prev.update();
  btn_menu.update();
  btn_screen.update();
  
  if(millis() - lastBatteryCheck > 1000) { // Check battery level every 1 second
    BatteryLevel = BatteryLevelFinder(PIN_BATTERY);
    if(BatteryLevel < LastBatteryLevel - 2 || BatteryLevel > LastBatteryLevel + 2) { // Only print if the battery level changed by at least 2%
      Serial.print("Battery: ");
      Serial.print(BatteryLevel);
      Serial.println("%");
      LastBatteryLevel = BatteryLevel;
    }
    lastBatteryCheck = millis();
  }

  // TODO: Move this to hardware abstraction layer [and make it more efficient by only reading when the slider is moved (i.e. when the value changes by a certain threshold)?]
  // Read the volume every 100 milliseconds so we don't spam the processor
  if (millis() - lastVolRead > 100) {
    int rawValue = analogRead(PIN_SLIDER);
    
    // map(value, fromLow, fromHigh, toLow, toHigh)
    int mappedVolume = map(rawValue, 0, 1024, 0, 100);

    // Only print if the volume actually changed!
    if (mappedVolume < currentVolume - 2 || mappedVolume > currentVolume + 2) { // Adding a threshold of 5% to avoid spamming the serial monitor with minor changes
      currentVolume = mappedVolume;
      Serial.print("Volume: ");
      Serial.print(currentVolume);
      Serial.println("%");
    }
    lastVolRead = millis();
  }

  int SliderMenuSelection = map(analogRead(PIN_SLIDER), 0, 1024, 0, numberMenuItems-1);
  Menu_selected_item = SliderMenuSelection;

  if (millis() - lastUpdate > 100) {
    lastUpdate = millis();
    
    currentFrequency = SongTime * 10 + 100; // Just for demo purposes, the frequency will increase as the song "progresses"
    
    // If it gets too high, reset back to a low bass note
    if (currentFrequency > 2000) {
      currentFrequency = 100;
    }
    
    // Tell the synth to change pitch ONLY right now
    //playSineWave(currentFrequency, currentVolume / 100.0); // volume is a percentage, so we divide by 100 to get a value between 0 and 1 for the amplitude

// Print the CURRENT diagnostics (No reset required)
    //Serial.print("CPU Load: ");
    //Serial.print(AudioProcessorUsage());
    //Serial.print("% | Memory Buckets Used: ");
    //Serial.println(AudioMemoryUsage());
  }

// --- 5. UI LOGIC (Now respects the power button!) ---
  if (isScreenOn == true) { // ONLY calculate and send data if the screen is awake!
    
    if (millis() - lastDisplayUpdate > 50) { 
      lastDisplayUpdate = millis();

      switch (currentState) {
        case STATE_PLAYER:
          drawPlayerScreen2("Brainstorm", "Artic Monkeys", currentVolume, SongTime);
          break;
        case STATE_MENU:
          drawMenuScreenTest("Brainstorm", "Artic Monkeys", currentVolume, songPlaying);
          break;
      }
    }
    
  }

  // -----------------------------------
  // BUTTON LOGIC (Runs as fast as possible, outside the timer!)
  // -----------------------------------
  if (currentState == STATE_PLAYER) {
    if (btn_play.fell()) {
        songPlaying = !songPlaying;
        
        if (songPlaying) {
          Serial.println("[PLAYER] Action: PLAY");
          playMusicFile("test.wav"); // The exact name on your SD card
        } else {
          Serial.println("[PLAYER] Action: PAUSE");
          // If you don't have a pause function, stop it for now
          stopMusic(); 
        }
      }
    if (btn_next.fell()) {
      Serial.println("[PLAYER] Action: Skipped Song (song time = " + String(SongTime) + "s)");
      SongTime = SongTime + 10;
    }
    if (btn_prev.fell()) {
      Serial.println("[PLAYER] Action: Rewound Song (song time = " + String(SongTime) + "s)");
      SongTime = SongTime - 10;
    }
    if (btn_menu.fell()) {
      Serial.println("[TRANSITION] Leaving Player -> Entering Menu");
      currentState = STATE_MENU;
    }
  } 
  else if (currentState == STATE_MENU) {
    if (btn_next.fell()) {
      Serial.println("[MENU] Action: Entered Folder");
    }
    if (btn_prev.fell()) {
      Serial.println("[MENU] Action: Exited folder");
    }
    if (btn_play.fell()) {
      Serial.println("[MENU] Action: Selected");
    }
    if (btn_menu.fell()) {
      Serial.println("[TRANSITION] Leaving Menu -> Entering Player");
      currentState = STATE_PLAYER;
    }
  }

  // Screen toogle and debug
  if (btn_screen.fell()) {
    isScreenOn = !isScreenOn;
    toggleScreenPower(isScreenOn);
    if (isScreenOn) {
      Serial.println("[SCREEN] Action: Screen Turned ON");
    } else {
      Serial.println("[SCREEN] Action: Screen Turned OFF");
    }
  }

}