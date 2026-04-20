#include <Arduino.h>
#include <U8g2lib.h>
#include "display_ui.h"

// U8X8 uses no memory buffer. It draws text directly to the glass.
// clock=13, data=11, cs=10, dc=9, reset=12
U8G2_SH1106_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, 13, 11, 10, 9, 12);

void initDisplay() {
  // 2. The Manual Reset Hammer (Wakes up the SSD1306)
  pinMode(12, OUTPUT);       
  digitalWrite(12, LOW);     
  delay(50);                
  digitalWrite(8, HIGH);    
  delay(50);                
  
  // 3. The Speed Limit (Stops the Teensy from crashing the screen)
  u8g2.setBusClock(4000000); 
  
  // 4. Finally, start the screen
  u8g2.begin();
  u8g2.setContrast(255);
  
  // Enable dithering for better grayscale effects
  u8g2.setDrawColor(1);  // Set draw color to white
}

void toggleScreenPower(bool turnOn) {
  if (turnOn) u8g2.setPowerSave(0);
  else u8g2.setPowerSave(1);
}

// --- DESIGN 1: THE PLAYER ---
void drawPlayerScreen(const char* songTitle, int volume, bool isPlaying) {
  u8g2.clearBuffer();
  
  // Song Title
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(0, 15, songTitle);
  
  // Play/Pause Status
  u8g2.setFont(u8g2_font_ncenB08_tr);
  if (isPlaying) {
    u8g2.drawStr(45, 35, "> PLAYING");
  } else {
    u8g2.drawStr(45, 35, "|| PAUSED");
  }

  // Volume Bar
  u8g2.drawFrame(0, 50, 124, 10);
  int barWidth = map(volume, 0, 99, 0, 122);
  u8g2.drawBox(0, 50, barWidth, 10);
  
  u8g2.sendBuffer();
}

// --- DESIGN 2: THE MENU ---
void drawMenuScreen(int numberMenuItems, int selectedItem) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  // A dummy list of folders
  const char* menuItems[] = {"1. Playlists", "2. Artists", "3. Albums", "4. Songs", "5. Genres", "6. Settings", "7. About", "8. Help", "9. Exit"};
  int maxItems = sizeof(menuItems) / sizeof(menuItems[0]);
  if (numberMenuItems > maxItems) numberMenuItems = maxItems; // Prevent out-of-bounds

  // Draw the list, and put an arrow next to the selected one
  for (int i = 0; i < numberMenuItems; i++) {
    int yPos = 20 + (i * 15); // Space them out vertically
    
    if (i == selectedItem){
      u8g2.drawStr(0, yPos, "->"); // The selector arrow
    }
    
    u8g2.drawStr(20, yPos, menuItems[i]);
  }

  u8g2.sendBuffer();
}