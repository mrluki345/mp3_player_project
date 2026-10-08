#include <Arduino.h>
#include <U8g2lib.h>
#include <string.h>
#include "display_ui.h"

// U8X8 uses no memory buffer. It draws text directly to the glass.
// clock=13, data=11, cs=10, dc=9, reset=12
U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, 13, 11, 10, 9, 12);

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
void drawPlayerScreen2(const char* songTitle, const char* Artist, int volume, int progress) {
  u8g2.clearBuffer();
    
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);

  //volume container
  u8g2.drawLine(18, 35, 88, 35);
  u8g2.drawLine(89, 35, 89, 32);
  //volume bar
  u8g2.drawBox(18, 32, map(volume, 0, 100, 0, 71), 3);
  //volume level
  u8g2.setFont(u8g2_font_5x7_tr);
  u8g2.drawStr(92, 37, String(volume).c_str());

  //song info
  u8g2.setFont(u8g2_font_6x12_tr);
  String songInfo = String(songTitle) + " - " + Artist;
  scrollingText(
        u8g2,
        songInfo.c_str(),
        10, 10,       // Box position
        108, 16,      // Box width and height
        (35.0f),        // Speed in pixels/second
        (0),         // Pause duration (ms)
        (0)              // Gap between repetitions
    );

  // Draw the dithered background for the progress bar
  drawDither(0, 54, 3, 128);
  //Filled progress bar
  u8g2.drawBox(0, 54, map(progress, 0, 100, 0, 128), 3);

  //song timestamps
  u8g2.setFont(u8g2_font_4x6_tr);
  u8g2.drawStr(0, 64, "0:00");  // Replace with actual timestamps if available
  u8g2.drawLine(17, 61, 20, 61); // Separator line
  u8g2.drawStr(23, 64, "3:24");  // Replace with actual timestamps if available

  //next song indicator
  u8g2.drawStr(43, 64, ">>");
  u8g2.drawStr(52, 64, "Next Song Title..."); // Replace with actual next song title if available

  u8g2.sendBuffer();
}

// --- DESIGN 2: THE MENU ---
void drawMenuScreenTest(const char* songTitle, const char* Artist, int volume, bool isPlaying) {
  u8g2.clearBuffer();

    u8g2.setBitmapMode(1);

    // top ribbon line
    u8g2.drawLine(0, 10, 128, 10);

    //menu list separators
    u8g2.drawLine(0, 20, 128, 20);
    u8g2.drawLine(0, 30, 128, 30);
    u8g2.drawLine(0, 40, 128, 40);
    u8g2.drawLine(0, 50, 128, 50);
    u8g2.drawLine(0, 60, 128, 60);

    //battery percent (top right)
    u8g2.setFont(u8g2_font_t0_12b_mr);
    char volumeText[12];
    snprintf(volumeText, sizeof(volumeText), "%d%%", volume);
    int volumeX = (volume >= 103) ? 105 : ((volume > 9) ? 111 : 117);
    u8g2.drawStr(volumeX, 8, volumeText);

  u8g2.sendBuffer();
}

void drawDither(int startX, int startY, int height, int width) {

  for (int y = startY; y < startY + height; y++) {
    for (int x = startX; x < startX + width; x++) {
      
      // The modulo math creates a 50% checkerboard dither
      if ((x + y) % 2 == 0) { 
        u8g2.drawPixel(x, y);
      }
    }
  }
}

void scrollingText(
    U8G2& u8g2,
    const char* text,
    int x, int y,
    int w, int h,
    float speed,
    unsigned long pauseMs,
    int gap
) {
    static float scrollX = 0;
    static unsigned long lastTime = 0;
    static unsigned long pauseStart = 0;
    static bool initialized = false;
    static bool scrolling = true;
    static const char* previousText = nullptr;

    unsigned long now = millis();
    int textW = u8g2.getStrWidth(text);

    if (!initialized || previousText != text) {
        scrollX = w;
        lastTime = now;
        initialized = true;
        scrolling = true;
        previousText = text;
    }

    if (scrolling) {
        float elapsed = (now - lastTime) / 1000.0f;
        scrollX -= speed * elapsed;
        lastTime = now;

        // One complete pass finished
        if (scrollX <= -textW) {
            scrolling = false;
            pauseStart = now;
        }
    } else {
        // Wait, then restart from the right
        if (now - pauseStart >= pauseMs) {
            scrollX = w;
            scrolling = true;
            lastTime = now;
        }
    }

    int baseline = y + (h + u8g2.getAscent()
                        - u8g2.getDescent()) / 2;

    u8g2.setClipWindow(x, y, x + w - 1, y + h - 1);

    u8g2.drawStr(x + (int)scrollX, baseline, text);

    u8g2.setMaxClipWindow();
}