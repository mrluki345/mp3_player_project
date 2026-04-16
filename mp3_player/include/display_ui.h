#pragma once

void initDisplay();
void toggleScreenPower(bool turnOn);

// Our two new dedicated screen tools!
void drawPlayerScreen(const char* songTitle, int volume, bool isPlaying);
void drawMenuScreen(int numberMenuItems, int selectedItem);

// Dithering demonstration function
void drawDitherDemo();