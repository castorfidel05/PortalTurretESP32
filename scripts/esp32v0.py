// === Bibliothèques ===
#include <Adafruit_NeoPixel.h>
#include <DFRobotDFPlayerMini.h>
#include <HardwareSerial.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>
 
// === OLED ===
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
// === LED ===
#define LED_PIN 4
#define NUM_LEDS 5
Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
#define EYE 0
#define CANON_L1 1
#define CANON_L2 2
#define CANON_R1 3
#define CANON_R2 4
 
// === DFPlayer ===
HardwareSerial mySerial(2); // RX=16, TX=17
DFRobotDFPlayerMini dfplayer;
 
// === Servo ===
Servo brasServo;
const int SERVO_PIN = 23;
const int ANGLE_FERME = 130;  // Angle de fermeture (130 degrés)
const int ANGLE_OUVERT = 0;   // Angle d'ouverture complète (0 degrés)
 
// === Gestion du changement de comportement ===
unsigned long dernierChangement = 0;
int modeCourant = 1;
const unsigned long INTERVALLE_CHANGEMENT = 10000; // 20 secondes
const int NOMBRE_COMPORTEMENTS = 20;

// === Couleurs ===
#define RED leds.Color(255, 0, 0)
#define GREEN leds.Color(0, 255, 0)
#define BLUE leds.Color(0, 0, 255)
#define WHITE leds.Color(255, 255, 255)
#define YELLOW leds.Color(255, 255, 0)
#define ORANGE leds.Color(255, 128, 0)
#define VIOLET leds.Color(128, 0, 255)
#define PINK leds.Color(255, 105, 180)
#define LIGHT_BLUE leds.Color(135, 206, 250)
#define WARM_WHITE leds.Color(255, 223, 186)
#define OFF leds.Color(0, 0, 0)
#define BLANC_FROID leds.Color(200, 200, 255)
 

int dernierSonJoue = -1;

// === Setup ===
void setup() {
  Serial.begin(9600);
  mySerial.begin(9600, SERIAL_8N1, 16, 17);
  if (!dfplayer.begin(mySerial)) {
    Serial.println("Erreur DFPlayer");
  }
  dfplayer.volume(20);
 
  leds.begin();
  leds.show();
 
  brasServo.attach(SERVO_PIN);
  brasServo.write(ANGLE_FERME); // Position initiale fermée (130 degrés)
 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
 
  randomSeed(analogRead(0)); // Initialiser l'aléa
  modeCourant = random(1, 21); // de 1 à 20 inclus
  afficherComportementOLED(modeCourant, (dernierSonJoue != -1) ? getNomSon(dernierSonJoue) : "");
  jouerComportement(modeCourant);
  dernierChangement = millis();
}
 // Fonction loop obligatoire pour Arduino/ESP32
void loop() {
  unsigned long maintenant = millis();
  if (maintenant - dernierChangement >= INTERVALLE_CHANGEMENT) {
    int nouveauMode = random(1, 21); // de 1 à 20 inclus
    // Pour éviter de répéter le même comportement deux fois de suite
    while (nouveauMode == modeCourant) {
      nouveauMode = random(1, 21);
    }
    modeCourant = nouveauMode;
    afficherComportementOLED(modeCourant, (dernierSonJoue != -1) ? getNomSon(dernierSonJoue) : "");
    jouerComportement(modeCourant);
    dernierChangement = maintenant;
  }
}

// === Affichage OLED du comportement ===
const char* nomsComportements[NOMBRE_COMPORTEMENTS+1] = {
  "",
  "Mode Accueil",
  "Mode En Garde",
  "Mode Sieste",
  "Mode Furie",
  "Mode Confus",
  "Mode Patrouille",
  "Mode Détente",
  "Mode Alert",
  "Mode Gentil",
  "Mode Déploiement",
  "Mode Sentinelle",
  "Mode Rêveur",
  "Mode Provocateur",
  "Mode Détecté",
  "Mode Apaisement",
  "Mode Flirt",
  "Mode Jalousie",
  "Mode Sadique",
  "Mode Politesse Trompeuse",
  "Mode Concert"
};

String getNomSon(int index) {
  // Mapping voix (0001-0034)
  switch(index) {
    case 1: return "Bonjour mon ami.mp3";
    case 2: return "Bonjour.mp3";
    case 3: return "Bonne nuit.mp3";
    case 5: return "Cible perdu.mp3";
    case 6: return "Cible verrouille.mp3";
    case 7: return "Deploiement.mp3";
    case 8: return "Désolé.mp3";
    case 9: return "Erreur Critique.mp3";
    case 10: return "Escusez moi.mp3";
    case 11: return "Fermeture.mp3";
    case 12: return "Il y a quelqun.mp3";
    case 13: return "Imitation tir.mp3";
    case 14: return "Accident.mp3";
    case 15: return "Je ne vous hais pas.mp3";
    case 16: return "Je peux aider.mp3";
    case 17: return "Je vous voie.mp3";
    case 18: return "Sentinel active.mp3";
    case 19: return "Mode sieste.mp3";
    case 20: return "Mode veille.mp3";
    case 21: return "Pardon.mp3";
    case 22: return "Qui va la.mp3";
    case 23: return "Ravie travail.mp3";
    case 24: return "Recherche.mp3";
    case 25: return "Repos.mp3";
    case 26: return "Salut.mp3";
    case 27: return "Sans rancune.mp3";
    case 28: return "Trouve.mp3";
    case 29: return "Verification.mp3";
    case 30: return "Vous tjs la.mp3";
    case 31: return "Vous voila.mp3";
    case 32: return "Désolé.mp3";
    case 33: return "Sentinel active.mp3";
    case 34: return "Trouve.mp3";
    // Sound effects
    case 100: return "Tir_4x.mp3";
    case 101: return "Deploiement.mp3";
    case 102: return "Retract.mp3";
    case 103: return "Alert.mp3";
    case 104: return "Deploy.mp3";
    // Music
    case 200: return "Still alive.mp3";
    default: return String(index) + ".mp3";
  }
}


void afficherComportementOLED(int num, String nomSon) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("Comportement:");
  display.setCursor(0,16);
  display.print(num);
  display.print("/");
  display.print(NOMBRE_COMPORTEMENTS);
  display.setCursor(0,32);
  if(num >= 1 && num <= NOMBRE_COMPORTEMENTS) {
    display.print(nomsComportements[num]);
  } else {
    display.print("?");
  }
  display.setCursor(0,48);
  display.print(nomSon);
  display.display();
}

// Nouvelle version de jouerSon
void jouerSon(int index) {
  dfplayer.play(index);
  dernierSonJoue = index;
  afficherComportementOLED(modeCourant, getNomSon(index));
}

// === Animations OLED ===

// Animation d'œil de tourelle qui s'ouvre et se ferme
void animateEye() {
  // Effacer l'écran
  display.clearDisplay();
  
  // Animation d'ouverture
  for (int i = 0; i <= 10; i++) {
    display.clearDisplay();
    display.fillCircle(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, i * 2, WHITE);
    display.display();
    delay(50);
  }
  
  // Maintenir l'œil ouvert
  display.clearDisplay();
  display.fillCircle(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 20, WHITE);
  // Ajouter une pupille rouge (simulée en inversant un cercle plus petit)
  display.fillCircle(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 8, BLACK);
  display.display();
  delay(500);
  
  // Animation de fermeture
  for (int i = 10; i >= 0; i--) {
    display.clearDisplay();
    display.fillCircle(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, i * 2, WHITE);
    display.display();
    delay(50);
  }
  
  display.clearDisplay();
  display.display();
}

// Animation de déploiement de tourelle
void animateTurretDeploy() {
  display.clearDisplay();
  
  // Phase 1: Tourelle fermée (boîte)
  display.fillRect(SCREEN_WIDTH/2 - 8, SCREEN_HEIGHT/2 - 8, 16, 16, WHITE);
  display.display();
  delay(500);
  
  // Phase 2: Début du déploiement - ouverture légère
  for (int i = 1; i <= 5; i++) {
    display.clearDisplay();
    // Corps central
    display.fillRect(SCREEN_WIDTH/2 - 8, SCREEN_HEIGHT/2 - 8 - i, 16, 16 + i*2, WHITE);
    // Œil qui s'allume progressivement
    if (i > 2) {
      display.fillCircle(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, i-2, BLACK);
      display.drawCircle(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, i-2, WHITE);
    }
    display.display();
    delay(100);
  }
  
  // Phase 3: Déploiement des bras latéraux
  for (int i = 1; i <= 10; i++) {
    display.clearDisplay();
    
    // Corps central
    display.fillRect(SCREEN_WIDTH/2 - 8, SCREEN_HEIGHT/2 - 18, 16, 36, WHITE);
    
    // Œil
    display.fillCircle(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 3, BLACK);
    display.drawCircle(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 3, WHITE);
    
    // Bras qui s'ouvrent progressivement
    display.fillRect(SCREEN_WIDTH/2 - 8 - i*2, SCREEN_HEIGHT/2 - 10, i*2, 6, WHITE); // Bras gauche
    display.fillRect(SCREEN_WIDTH/2 + 8, SCREEN_HEIGHT/2 - 10, i*2, 6, WHITE);       // Bras droit
    
    // Canons aux extrémités des bras
    if (i > 5) {
      display.fillCircle(SCREEN_WIDTH/2 - 8 - i*2, SCREEN_HEIGHT/2 - 7, 3, WHITE); // Canon gauche
      display.fillCircle(SCREEN_WIDTH/2 + 8 + i*2, SCREEN_HEIGHT/2 - 7, 3, WHITE); // Canon droit
    }
    
    display.display();
    delay(80);
  }
  
  // Phase 4: Activation complète - œil qui clignote
  for (int i = 0; i < 3; i++) {
    // Œil éteint
    display.fillCircle(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 3, BLACK);
    display.display();
    delay(100);
    
    // Œil allumé
    display.fillCircle(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 3, WHITE);
    display.display();
    delay(100);
  }
  
  // État final - tourelle déployée et active
  display.fillCircle(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 3, BLACK);
  display.drawCircle(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 3, WHITE);
  display.display();
  delay(500);
}

// === Fonctions d'animation servo ===
void ouvertureDouce(int angleMin = -1, int delai = 2) {
  if (angleMin == -1) angleMin = ANGLE_OUVERT; // Utiliser l'angle d'ouverture complet par défaut
  int angleActuel = brasServo.read();
  for (int angle = angleActuel; angle >= angleMin; angle--) {
    brasServo.write(angle);
    delay(delai);
  }
}

void fermetureDouce(int angleMax = -1, int delai = 15) {
  if (angleMax == -1) angleMax = ANGLE_FERME; // Utiliser l'angle de fermeture par défaut
  int angleActuel = brasServo.read();
  for (int angle = angleActuel; angle <= angleMax; angle++) {
    brasServo.write(angle);
    delay(delai);
  }
}

void ouvertureRapide(int angleMin = -1, int delai = 5) {
  if (angleMin == -1) angleMin = ANGLE_OUVERT; // Utiliser l'angle d'ouverture complet par défaut
  int angleActuel = brasServo.read();
  for (int angle = angleActuel; angle >= angleMin; angle--) {
    brasServo.write(angle);
    delay(delai);
  }
}

void fermetureRapide(int angleMax = -1, int delai = 5) {
  if (angleMax == -1) angleMax = ANGLE_FERME; // Utiliser l'angle de fermeture par défaut
  int angleActuel = brasServo.read();
  for (int angle = angleActuel; angle <= angleMax; angle++) {
    brasServo.write(angle);
    delay(delai);
  }
}

void ouverturePartielle(int pourcentage, int delai = 15) {
  // Convertir le pourcentage en angle réel
  int angleTarget = ANGLE_FERME - (pourcentage * (ANGLE_FERME - ANGLE_OUVERT) / 100);
  int angleActuel = brasServo.read();
  
  if (angleActuel > angleTarget) {
    // Ouvrir (diminuer l'angle)
    for (int angle = angleActuel; angle >= angleTarget; angle--) {
      brasServo.write(angle);
      delay(delai);
    }
  } else {
    // Fermer (augmenter l'angle)
    for (int angle = angleActuel; angle <= angleTarget; angle++) {
      brasServo.write(angle);
      delay(delai);
    }
  }
}

void oscillation(int cycles = 3, int amplitude = 20, int delai = 10) {
  int angleActuel = brasServo.read();
  int angleBase = angleActuel;
  
  for (int i = 0; i < cycles; i++) {
    // Avec le moteur inversé, ajouter à l'angle ferme plus et ouvre moins
    for (int j = 0; j <= amplitude; j++) {
      brasServo.write(angleBase + j); // Ferme un peu plus
      delay(delai);
    }
    for (int j = amplitude; j >= -amplitude; j--) {
      brasServo.write(angleBase + j); // De fermeture à ouverture
      delay(delai);
    }
    for (int j = -amplitude; j <= 0; j++) {
      brasServo.write(angleBase + j); // Retour à la position de base
      delay(delai);
    }
  }
}

void secousse(int amplitude = 15, int cycles = 2) {
  int angleActuel = brasServo.read();
  for (int i = 0; i < cycles; i++) {
    // Avec le moteur inversé: + ferme plus, - ouvre plus
    brasServo.write(angleActuel + amplitude); // Ferme un peu plus
    delay(50);
    brasServo.write(angleActuel - amplitude); // Ouvre un peu plus
    delay(50);
    brasServo.write(angleActuel); // Retour à la position initiale
    delay(50);
  }
}

// === Fonctions d'animation LED ===
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

void eyeBlink(uint32_t color, int times = 1, int delayMs = 100) {
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

void barrelFlash(uint32_t color, int times = 1, int delayMs = 100) {
  for (int i = 0; i < times; i++) {
    setBarrelLeds(color);
    delay(delayMs);
    setBarrelLeds(OFF);
    delay(delayMs);
  }
}

void barrelSequence(uint32_t color, int delayMs = 100) {
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

void barrelWave(uint32_t color, int cycles = 1, int delayMs = 50) {
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

void eyePulse(uint32_t color, int cycles = 3, int maxBrightness = 255, int stepDelay = 20) {
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

void rainbowEye(int cycles = 1, int delayMs = 20) {
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

// Joue un son avec un effet visuel synchronisé
void jouerSonAvecEffet(int index, uint32_t couleurOeil = OFF, uint32_t couleurCanon = OFF, bool clignoteOeil = false, bool clignoteCanon = false, int nombreClignements = 1, int delaiClignement = 100) {
  // Appliquer les couleurs de base
  if (couleurOeil != OFF) {
    setEyeColor(couleurOeil);
  }
  
  // Jouer le son immédiatement
  jouerSon(index);
  
  // Appliquer les effets visuels synchronisés avec le son
  if (clignoteOeil) {
    eyeBlink(couleurOeil, nombreClignements, delaiClignement);
  }
  
  if (clignoteCanon) {
    barrelFlash(couleurCanon, nombreClignements, delaiClignement);
  } else if (couleurCanon != OFF) {
    setBarrelLeds(couleurCanon);
  }
}

// === Comportements ===
void jouerComportement(int mode) {
  // Réinitialiser les LEDs
  leds.clear();
  leds.show();
  
  switch (mode) {
    case 1: // Mode Accueil
      afficherComportementOLED(mode);
      setEyeColor(WHITE);
      ouvertureDouce(); // Ouverture complète à 140°
      setBarrelLeds(WHITE);
      jouerSon(2); // 0002 - Bonjour mon ami
      delay(1000);
      fermetureDouce(); // Fermeture complète à 0°
      setBarrelLeds(OFF);
      break;
      
    case 2: // Mode En Garde
      afficherComportementOLED(mode);
      setEyeColor(RED);
      ouvertureRapide();
      animateTurretFiring(); // Utiliser l'animation de tir au lieu du texte statique
      jouerSonAvecEffet(17, RED, RED, true, true); // 0017 - Je vous vois
      delay(1500);
      fermetureRapide();
      setBarrelLeds(OFF);
      break;
      
    case 3: // Mode Sieste
      afficherComportementOLED(mode);
      setEyeColor(LIGHT_BLUE);
      ouverturePartielle(50); // 50% ouverture (maintenant en pourcentage)
      animateEye(); // Utiliser l'animation d'œil qui s'ouvre et se ferme
      jouerSon(19); // 0019 - Mode sieste
      eyePulse(LIGHT_BLUE, 2);
      fermetureDouce(-1, 25); // Fermeture lente complète
      break;
      
    case 4: // Mode Furie
      setEyeColor(WHITE);
      for (int i = 0; i < 3; i++) {
        ouvertureRapide(-1, 3); // Ouverture complète à 140°
        eyeBlink(WHITE, 1, 50);
        barrelFlash(WHITE, 1, 50);
        fermetureRapide(-1, 3); // Fermeture complète à 0°
        delay(200);
      }
      jouerSonAvecEffet(13, WHITE, WHITE, true, true, 1, 50); // 0013 - Imitation tir avec effet visuel
      delay(1000);
      // Effet visuel synchronisé avec le son de tir
      ouvertureRapide(-1, 3); // Ouverture complète à 140°
      jouerSonAvecEffet(100, WHITE, WHITE, true, true, 4, 50); // 0100 - Tir_4x avec effets visuels
      fermetureRapide(-1, 3); // Fermeture complète à 0°
      break;
      
    case 5: // Mode Confus
      ouverturePartielle(70); // 70% ouverture (maintenant en pourcentage)
      for (int i = 0; i < 5; i++) {
        setEyeColor(RED);
        delay(100);
        setEyeColor(WHITE);
        delay(150);
      }
      oscillation(2);
      barrelFlash(WHITE, 1, 200);
      jouerSon(9); // 0009 - Erreur critique
      fermetureDouce(-1, 25); // Fermeture lente complète
      break;
      
    case 6: // Mode Patrouille
      setEyeColor(RED);
      ouvertureDouce(-40, 15); // Ouverture lente (plus ouverte que la normale)
      for (int i = 0; i < 3; i++) {
        eyeBlink(RED, 1, 200);
        delay(200);
      }
      barrelSequence(RED, 150);
      setBarrelLeds(RED);
      delay(500);
      setBarrelLeds(OFF);
      jouerSon(24); // 0024 - Recherche
      delay(1000);
      fermetureDouce();
      break;
      
    case 7: // Mode Détente
      afficherComportementOLED(mode);
      setEyeColor(GREEN);
      ouverturePartielle(75); // 75% ouverture
      for (int i = 0; i < 2; i++) {
        oscillation(1, 10, 20);
        delay(300);
      }
      jouerSon(25); // 0025 - Repos
      fermetureDouce();
      break;
      
    case 8: // Mode Alert
      setEyeColor(RED);
      ouvertureRapide(-40, 3); // Ouverture rapide (plus ouverte que la normale)
      eyeBlink(RED, 4, 50);
      barrelFlash(RED, 4, 50);
      jouerSon(103); // 0103 - Alert
      fermetureRapide();
      break;
      
    case 9: // Mode Gentil
      setEyeColor(PINK);
      ouvertureDouce();
      barrelFlash(PINK, 1, 300);
      jouerSon(16); // 0016 - Je peux vous aider
      fermetureDouce();
      break;
      
    case 10: // Mode Déploiement
      animateTurretDeploy(); // Utiliser l'animation de déploiement au lieu du texte statique
      setEyeColor(WHITE);
      delay(500);
      // Jouer le son pendant l'ouverture pour synchroniser
      jouerSon(7); // 0007 - Déploiement (voix)
      ouvertureRapide(-1, 10); // 100% ouverture (140°)
      setEyeColor(YELLOW);
      // Effet sonore de déploiement avec effet visuel synchronisé
      jouerSonAvecEffet(101, YELLOW, YELLOW, false, true, 2, 100); // 0101 - Deploiement (effet sonore)
      barrelSequence(YELLOW, 100);
      delay(1000);
      fermetureRapide(); // Fermeture complète à 0°
      break;
      
    case 11: // Mode Sentinelle
      setEyeColor(RED);
      ouvertureDouce();
      for (int i = 0; i < 3; i++) {
        barrelFlash(RED, 1, 500); // Clignotement lent
        delay(500);
      }
      jouerSon(18); // 0018 - Mode Sentinel activé
      delay(1000);
      fermetureDouce(65, 25); // Fermeture partielle (50% fermé)
      break;
      
    case 12: // Mode Rêveur
      setEyeColor(LIGHT_BLUE);
      ouverturePartielle(50); // 50% ouverture (maintenant en pourcentage)
      eyePulse(LIGHT_BLUE, 2, 200, 40); // Pulsation lente
      jouerSon(20); // 0020 - Mode veille
      delay(1000);
      fermetureDouce(); // Fermeture complète à 0°
      break;
      
    case 13: // Mode Provocateur
      setEyeColor(YELLOW);
      ouvertureRapide();
      eyeBlink(YELLOW, 3, 80);
      barrelFlash(YELLOW, 2, 100);
      jouerSon(15); // 0015 - Je ne vous hais pas
      fermetureRapide(65, 3); // Fermeture brusque (50% fermé)
      break;
      
    case 14: // Mode Détecté
      ouvertureDouce(-40, 15); // Ouverture lente (plus ouverte que la normale)
      setEyeColor(RED);
      delay(500);
      setEyeColor(ORANGE);
      eyeBlink(ORANGE, 3, 80);
      secousse();
      
      // Synchroniser le son "Trouvé" avec l'effet visuel
      leds.setPixelColor(CANON_L1, WHITE);
      leds.setPixelColor(CANON_L2, WHITE);
      leds.show();
      jouerSon(28); // 0028 - Trouvé
      delay(300);
      
      leds.setPixelColor(CANON_L1, OFF);
      leds.setPixelColor(CANON_L2, OFF);
      leds.setPixelColor(CANON_R1, WHITE);
      leds.setPixelColor(CANON_R2, WHITE);
      leds.show();
      delay(300);
      
      // Synchroniser le son "Vous voilà" avec un effet visuel
      setBarrelLeds(OFF);
      jouerSonAvecEffet(31, ORANGE, WHITE, true, true, 2, 100); // 0031 - Vous voilà avec effet visuel
      fermetureDouce();
      break;
      
    case 15: // Mode Apaisement
      setEyeColor(WARM_WHITE);
      ouvertureDouce(-40, 15); // Ouverture lente (plus ouverte que la normale)
      barrelSequence(WARM_WHITE, 200);
      delay(500);
      setBarrelLeds(OFF);
      jouerSon(27); // 0027 - Sans rancune
      fermetureDouce(65, 30); // Fermeture très douce (50% fermé)
      break;
      
    case 16: // Mode Flirt
      ouverturePartielle(40); // Ouverture timide (40% de l'ouverture totale)
      for (int i = 0; i < 3; i++) {
        setEyeColor(PINK);
        delay(150);
        setEyeColor(OFF);
        delay(150);
      }
      setEyeColor(PINK);
      barrelWave(PINK, 1, 100); // Vague sensuelle
      jouerSon(26); // 0026 - Salut
      delay(1800);
      jouerSon(15); // 0015 - Je ne vous hais pas
      fermetureRapide(); // Fermeture complète à 0°
      break;
      
    case 17: // Mode Jalousie
      setEyeColor(GREEN);
      ouvertureRapide(-40, 5); // Ouverture agressive (plus ouverte que la normale)
      delay(200);
      setEyeColor(RED);
      
      // Synchroniser le son avec l'effet visuel
      jouerSonAvecEffet(29, RED, RED, false, true, 3, 80); // 0029 - Vérification avec battement rapide
      delay(1000);
      
      // Synchroniser le son "Cible perdu" avec la secousse
      jouerSon(5); // 0005 - Cible perdu
      secousse(15, 3); // Secousse plus intense
      barrelFlash(RED, 1, 100); // Flash rapide
      fermetureDouce(65, 10); // Fermeture cassée (50% fermé)
      break;
      
    case 18: // Mode Sadique
      setEyeColor(WHITE);
      ouvertureDouce(-20, 15); // Ouverture lente (légèrement plus ouverte que la normale)
      delay(300);
      ouvertureRapide(-40, 2); // Snap à ouverture maximale
      
      // Synchroniser le premier son avec l'effet visuel
      jouerSonAvecEffet(14, RED, RED, false, false); // 0014 - J'ai tout vu c'était un accident
      delay(1000);
      
      // Synchroniser le deuxième son avec un effet visuel
      jouerSonAvecEffet(8, RED, RED, true, true, 2, 100); // 0008 - Désolé avec clignotement
      fermetureDouce();
      setBarrelLeds(OFF);
      break;
      
    case 19: // Mode Politesse Trompeuse
      setEyeColor(WHITE);
      ouvertureRapide();
      delay(500);
      
      // Premier son avec clin d'œil synchronisé
      eyeBlink(WHITE, 1, 100); // Clin d'œil
      jouerSonAvecEffet(10, WHITE, WHITE, false, true, 1, 200); // 0010 - Excusez-moi avec flash
      delay(1000);
      
      // Deuxième son avec effet visuel
      jouerSonAvecEffet(15, YELLOW, YELLOW, true, false, 2, 100); // 0015 - Je ne vous hais pas avec clignotement
      delay(1000);
      
      // Troisième son avec effet visuel
      jouerSonAvecEffet(12, WHITE, WHITE, false, true, 3, 80); // 0012 - Il y a quelqu'un ? avec flash rapide
      fermetureDouce();
      break;
      
    case 20: // Mode Concert (Easter Egg)
      ouvertureDouce(-40, 15); // Ouverture lente (plus ouverte que la normale)
      rainbowEye(3);
      for (int i = 0; i < 5; i++) {
        barrelWave(leds.Color(random(255), random(255), random(255)), 1, 100);
      }
      jouerSon(200); // 0200 - Still Alive
      // Reste ouvert avec show lumineux
      for (int i = 0; i < 10; i++) {
        rainbowEye(1);
        barrelWave(leds.Color(random(255), random(255), random(255)), 1, 100);
      }
      fermetureDouce();
      break;
      
    default:
      // Mode par défaut si le mode n'est pas reconnu
      setEyeColor(BLUE);
      ouvertureDouce();
      delay(1000);
      fermetureDouce();
      break;
  }
}
