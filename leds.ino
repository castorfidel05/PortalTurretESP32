#include "hardware/leds.h"

void initLeds() {
  leds.begin();
  leds.show();
}

void setEyeColor(uint32_t color) {
  leds.setPixelColor(EYE, color);
  leds.show();
}

void setBarrelLeds(uint32_t color) {
  leds.setPixelColor(CANON_L1, color);
  leds.setPixelColor(CANON_L2, color);
  leds.setPixelColor(CANON_R1, color);
  leds.setPixelColor(CANON_R2, color);
  leds.show();
}

void eyeBlink(uint32_t color, int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    leds.setPixelColor(EYE, color);
    leds.show();
    delay(delayMs);
    leds.setPixelColor(EYE, OFF);
    leds.show();
    delay(delayMs);
  }
  leds.setPixelColor(EYE, color);
  leds.show();
}

void barrelFlash(uint32_t color, int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    setBarrelLeds(color);
    delay(delayMs);
    setBarrelLeds(OFF);
    delay(delayMs);
  }
}

void barrelSequence(uint32_t color, int delayMs) {
  leds.setPixelColor(CANON_L1, color);
  leds.show();
  delay(delayMs);
  leds.setPixelColor(CANON_L2, color);
  leds.show();
  delay(delayMs);
  leds.setPixelColor(CANON_R1, color);
  leds.show();
  delay(delayMs);
  leds.setPixelColor(CANON_R2, color);
  leds.show();
  delay(delayMs);
}

void barrelWave(uint32_t color, int cycles, int delayMs) {
  for (int c = 0; c < cycles; c++) {
    // Gauche à droite
    leds.setPixelColor(CANON_L1, color);
    leds.show();
    delay(delayMs);
    leds.setPixelColor(CANON_L2, color);
    leds.show();
    delay(delayMs);
    leds.setPixelColor(CANON_R1, color);
    leds.show();
    delay(delayMs);
    leds.setPixelColor(CANON_R2, color);
    leds.show();
    delay(delayMs);
    
    // Extinction
    leds.setPixelColor(CANON_L1, OFF);
    leds.show();
    delay(delayMs);
    leds.setPixelColor(CANON_L2, OFF);
    leds.show();
    delay(delayMs);
    leds.setPixelColor(CANON_R1, OFF);
    leds.show();
    delay(delayMs);
    leds.setPixelColor(CANON_R2, OFF);
    leds.show();
    delay(delayMs);
  }
}

void eyePulse(uint32_t color, int cycles, int maxBrightness, int stepDelay) {
  for (int c = 0; c < cycles; c++) {
    for (int brightness = 10; brightness <= maxBrightness; brightness += 5) {
      uint8_t r = (uint8_t)((color >> 16) & 0xFF) * brightness / 255;
      uint8_t g = (uint8_t)((color >> 8) & 0xFF) * brightness / 255;
      uint8_t b = (uint8_t)(color & 0xFF) * brightness / 255;
      leds.setPixelColor(EYE, leds.Color(r, g, b));
      leds.show();
      delay(stepDelay);
    }
    
    for (int brightness = maxBrightness; brightness >= 10; brightness -= 5) {
      uint8_t r = (uint8_t)((color >> 16) & 0xFF) * brightness / 255;
      uint8_t g = (uint8_t)((color >> 8) & 0xFF) * brightness / 255;
      uint8_t b = (uint8_t)(color & 0xFF) * brightness / 255;
      leds.setPixelColor(EYE, leds.Color(r, g, b));
      leds.show();
      delay(stepDelay);
    }
  }
}

void rainbowEye(int cycles, int delayMs) {
  for (int c = 0; c < cycles; c++) {
    for (int i = 0; i < 255; i++) {
      int r, g, b;
      if (i < 85) {
        r = i * 3;
        g = 255 - i * 3;
        b = 0;
      } else if (i < 170) {
        i -= 85;
        r = 255 - i * 3;
        g = 0;
        b = i * 3;
      } else {
        i -= 170;
        r = 0;
        g = i * 3;
        b = 255 - i * 3;
      }
      leds.setPixelColor(EYE, leds.Color(r, g, b));
      leds.show();
      delay(delayMs);
    }
  }
}
