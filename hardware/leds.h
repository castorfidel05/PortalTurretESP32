#ifndef LEDS_H
#define LEDS_H

#include <Adafruit_NeoPixel.h>
#include "../config.h"

// Initialisation des LEDs
void initLeds();

// Fonctions de base pour les LEDs
void setEyeColor(uint32_t color);
void setBarrelLeds(uint32_t color);

// Effets d'animation pour les LEDs
void eyeBlink(uint32_t color, int times = 1, int delayMs = 100);
void barrelFlash(uint32_t color, int times = 1, int delayMs = 100);
void barrelSequence(uint32_t color, int delayMs = 100);
void barrelWave(uint32_t color, int cycles = 1, int delayMs = 50);
void eyePulse(uint32_t color, int cycles = 3, int maxBrightness = 255, int stepDelay = 20);
void rainbowEye(int cycles = 1, int delayMs = 20);

// Variable externe pour accéder aux LEDs
#ifndef MAIN_INO
extern Adafruit_NeoPixel leds;
#endif

#endif // LEDS_H
