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

// === NOUVELLES FONCTIONS POUR LA RESPIRATION ET EFFETS VOCAUX ===

// Obtenir la couleur selon l'état émotionnel
uint32_t getCouleurEtat() {
  extern int humeurActuelle, niveauEnergie, niveauVigilance;
  
  // Déterminer l'état émotionnel actuel
  if (humeurActuelle < 30 && (niveauVigilance > 60 || niveauEnergie > 70)) {
    return RED; // HOSTILE
  }
  if (niveauVigilance > 50 || (humeurActuelle < 50 && niveauEnergie > 40)) {
    return ORANGE; // VIGILANT
  }
  if (humeurActuelle > 70 && niveauEnergie > 50) {
    return WHITE; // AMICAL
  }
  if (humeurActuelle > 60 && niveauEnergie > 80) {
    return PINK; // LUDIQUE
  }
  return LIGHT_BLUE; // NEUTRE
}

// Respiration continue selon l'état émotionnel
void breathingEffect() {
  uint32_t couleur = getCouleurEtat();
  extern int niveauEnergie, humeurActuelle;
  
  // Adapter la vitesse selon l'énergie (plus d'énergie = respiration rapide)
  int vitesse = map(niveauEnergie, 0, 100, 40, 15);
  // Adapter l'intensité selon l'humeur
  int intensite = map(humeurActuelle, 0, 100, 120, 220);
  
  eyePulse(couleur, 1, intensite, vitesse);
}

// Pattern joueur avancé : Rainbow Dance
void rainbowDance() {
  extern Servo brasServo;
  
  // Attacher le servo pour le mouvement
  brasServo.attach(SERVO_PIN);
  
  // Phase 1: Ouverture progressive avec arc-en-ciel
  for (int step = 0; step < 20; step++) {
    // Position des bras (100% -> 0% progressivement)
    float positionBras = 100.0 - (step * 5.0);
    setServoPourcentage(positionBras, 30.0);
    
    // Arc-en-ciel qui traverse les 5 LEDs
    int hue = (step * 18) % 360; // 360° / 20 steps = 18° par step
    
    // LED 0-1: Yeux (2 LEDs)
    uint32_t couleur1 = leds.ColorHSV((hue * 65536L) / 360);
    leds.setPixelColor(0, couleur1);
    leds.setPixelColor(1, couleur1);
    
    // LEDs 2-4: Barrels (3 LEDs) avec décalage
    for (int i = 2; i < 5; i++) {
      int hueDecale = (hue + (i - 2) * 60) % 360; // Décalage de 60°
      uint32_t couleurBarrel = leds.ColorHSV((hueDecale * 65536L) / 360);
      leds.setPixelColor(i, couleurBarrel);
    }
    
    leds.show();
    delay(150); // Tempo pour voir l'effet
  }
  
  // Phase 2: Effet stroboscopique rapide
  for (int flash = 0; flash < 6; flash++) {
    // Couleur aléatoire pour toutes les LEDs
    uint32_t couleurFlash = leds.ColorHSV(random(65536));
    for (int i = 0; i < 5; i++) {
      leds.setPixelColor(i, couleurFlash);
    }
    leds.show();
    
    // Mouvement rapide des bras
    setServoPourcentage(random(20, 81), 80.0);
    delay(100);
    
    // Éteindre
    leds.clear();
    leds.show();
    delay(80);
  }
  
  // Phase 3: Fermeture avec vague arc-en-ciel inverse
  for (int step = 0; step < 15; step++) {
    // Position des bras (position actuelle -> 100%)
    float positionBras = (step * 6.67); // 100/15 = 6.67
    setServoPourcentage(100.0 - positionBras, 40.0);
    
    // Vague qui s'éteint progressivement
    int intensite = 255 - (step * 17); // Diminue l'intensité
    
    for (int i = 0; i < 5; i++) {
      int hue = (step * 24 + i * 72) % 360;
      uint32_t couleur = leds.ColorHSV((hue * 65536L) / 360, 255, max(intensite, 0));
      leds.setPixelColor(i, couleur);
    }
    
    leds.show();
    delay(120);
  }
  
  // Finalisation
  leds.clear();
  leds.show();
  setServoPourcentage(100.0, 50.0);
  delay(300);
}

// Effet LED pendant la lecture d'un son
void voiceLedEffect(uint32_t couleur, int dureeMs, int etatEmotionnel) {
  int pulses = dureeMs / 300; // Une pulsation toutes les 300ms
  
  switch(etatEmotionnel) {
    case 4: // ETAT_HOSTILE
      // Pulsations agressives et rapides
      for (int i = 0; i < pulses; i++) {
        barrelFlash(RED, 1, 80);
        eyeBlink(RED, 1, 60);
        delay(random(50, 150));
      }
      break;
      
    case 2: // ETAT_AMICAL
      // Doux clignotements accueillants
      for (int i = 0; i < pulses; i++) {
        eyePulse(couleur, 1, random(180, 230), random(20, 35));
        delay(random(200, 400));
      }
      break;
      
    case 5: // ETAT_LUDIQUE
      // Effet festif et coloré
      for (int i = 0; i < pulses; i++) {
        uint32_t randomColor = leds.Color(random(100, 255), random(100, 255), random(100, 255));
        setEyeColor(randomColor);
        if (random(3) == 0) barrelFlash(randomColor, 1, 100);
        delay(random(150, 350));
      }
      break;
      
    case 3: // ETAT_VIGILANT
      // Pulsations nerveuses
      for (int i = 0; i < pulses; i++) {
        eyePulse(couleur, 1, random(150, 200), random(15, 25));
        if (random(4) == 0) barrelFlash(couleur, 1, 50);
        delay(random(100, 250));
      }
      break;
      
    default: // ETAT_NEUTRE
      // Respiration calme pendant la parole
      for (int i = 0; i < pulses; i++) {
        eyePulse(couleur, 1, random(140, 190), random(25, 40));
        delay(random(250, 450));
      }
      break;
  }
}
