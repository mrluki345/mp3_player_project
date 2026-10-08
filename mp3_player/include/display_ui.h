#pragma once

void initDisplay();
void toggleScreenPower(bool turnOn);

// Our two new dedicated screen tools!
void drawPlayerScreen2(const char* songTitle, const char* Artist, int volume, int progress);

void drawMenuScreenTest(const char* songTitle, const char* Artist, int volume, bool isPlaying);

void drawDither(int startX, int startY, int height, int width);
void scrollingText(
    U8G2& u8g2,
    const char* text,
    int x, int y,
    int w, int h,
    float speed = (35.0f),
    unsigned long pauseMs = (1000),
    int gap = 20
);  