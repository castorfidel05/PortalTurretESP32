// === Portal Turret ESP32 v1.0 ===
// Version simplifiée avec 5 modes et séparation des opérations audio/moteur
// pour éviter les problèmes d'alimentation

// === Bibliothèques ===
#include <Adafruit_NeoPixel.h>
#include <DFRobotDFPlayerMini.h>
#include <HardwareSerial.h>
#include <ESP32Servo.h>

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
const unsigned long INTERVALLE_CHANGEMENT = 10000; // 10 secondes
const int NOMBRE_COMPORTEMENTS = 5; // Réduit à 5 modes

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

// Variables globales pour la séparation des opérations
int dernierSonJoue = -1;
bool sonEnCours = false;
unsigned long debutSon = 0;
const unsigned long DELAI_ENTRE_OPERATIONS = 500; // 500ms entre son et mouvement

// === Setup ===
void setup() {
  Serial.begin(9600);
  
  // Initialisation des LEDs
  leds.begin();
  leds.clear();
  leds.show();
  
  // Initialisation du servo
  brasServo.attach(SERVO_PIN);
  brasServo.write(ANGLE_FERME); // Position initiale fermée
  
  // Initialisation du DFPlayer avec délai pour stabiliser
  delay(500);
  mySerial.begin(9600, SERIAL_8N1, 16, 17);
  if (!dfplayer.begin(mySerial)) {
    Serial.println("Erreur DFPlayer");
  }
  dfplayer.volume(20);
  delay(500);
  
  randomSeed(analogRead(0)); // Initialiser l'aléa
  modeCourant = random(1, NOMBRE_COMPORTEMENTS + 1); // de 1 à 5 inclus
  
  Serial.println("Turret v1.0 initialisée");
  Serial.print("Mode initial: ");
  Serial.println(modeCourant);
  
  // Jouer le comportement initial
  jouerComportement(modeCourant);
  dernierChangement = millis();
}

// Fonction loop obligatoire pour Arduino/ESP32
void loop() {
  unsigned long maintenant = millis();
  
  // Changement de comportement après l'intervalle
  if (maintenant - dernierChangement >= INTERVALLE_CHANGEMENT) {
    int nouveauMode = random(1, NOMBRE_COMPORTEMENTS + 1);
    // Pour éviter de répéter le même comportement deux fois de suite
    while (nouveauMode == modeCourant) {
      nouveauMode = random(1, NOMBRE_COMPORTEMENTS + 1);
    }
    modeCourant = nouveauMode;
    
    Serial.print("Nouveau mode: ");
    Serial.println(modeCourant);
    
    jouerComportement(modeCourant);
    dernierChangement = maintenant;
  }
}

// === Fonctions d'animation servo ===
void ouvertureDouce(int angleMin = -1, int delai = 15) {
  if (angleMin == -1) angleMin = ANGLE_OUVERT; // Utiliser l'angle d'ouverture complet par défaut
  int angleActuel = brasServo.read();
  Serial.println("Ouverture douce");
  for (int angle = angleActuel; angle >= angleMin; angle--) {
    brasServo.write(angle);
    delay(delai);
  }
}

void fermetureDouce(int angleMax = -1, int delai = 15) {
  if (angleMax == -1) angleMax = ANGLE_FERME; // Utiliser l'angle de fermeture par défaut
  int angleActuel = brasServo.read();
  Serial.println("Fermeture douce");
  for (int angle = angleActuel; angle <= angleMax; angle++) {
    brasServo.write(angle);
    delay(delai);
  }
}

void ouvertureRapide(int angleMin = -1, int delai = 5) {
  if (angleMin == -1) angleMin = ANGLE_OUVERT; // Utiliser l'angle d'ouverture complet par défaut
  int angleActuel = brasServo.read();
  Serial.println("Ouverture rapide");
  for (int angle = angleActuel; angle >= angleMin; angle--) {
    brasServo.write(angle);
    delay(delai);
  }
}

void fermetureRapide(int angleMax = -1, int delai = 5) {
  if (angleMax == -1) angleMax = ANGLE_FERME; // Utiliser l'angle de fermeture par défaut
  int angleActuel = brasServo.read();
  Serial.println("Fermeture rapide");
  for (int angle = angleActuel; angle <= angleMax; angle++) {
    brasServo.write(angle);
    delay(delai);
  }
}

void ouverturePartielle(int pourcentage, int delai = 15) {
  // Convertir le pourcentage en angle réel
  int angleTarget = ANGLE_FERME - (pourcentage * (ANGLE_FERME - ANGLE_OUVERT) / 100);
  int angleActuel = brasServo.read();
  
  Serial.print("Ouverture partielle ");
  Serial.print(pourcentage);
  Serial.println("%");
  
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
  
  Serial.println("Oscillation");
  
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

// === Fonctions audio ===
void jouerSon(int index) {
  Serial.print("Jouer son: ");
  Serial.println(index);
  dfplayer.play(index);
  dernierSonJoue = index;
  sonEnCours = true;
  debutSon = millis();
}

// === Comportements ===
void jouerComportement(int mode) {
  // Réinitialiser les LEDs
  leds.clear();
  leds.show();
  
  switch (mode) {
    case 1: // Mode Accueil
      Serial.println("Mode Accueil");
      
      // Étape 1: Effets visuels et ouverture
      setEyeColor(WHITE);
      ouvertureDouce();
      setBarrelLeds(WHITE);
      
      // Étape 2: Son (après délai pour stabiliser)
      delay(DELAI_ENTRE_OPERATIONS);
      jouerSon(2); // 0002 - Bonjour mon ami
      delay(1500);
      
      // Étape 3: Fermeture
      fermetureDouce();
      setBarrelLeds(OFF);
      break;
      
    case 2: // Mode En Garde
      Serial.println("Mode En Garde");
      
      // Étape 1: Effets visuels et ouverture
      setEyeColor(RED);
      ouvertureRapide();
      eyeBlink(RED, 2, 100);
      
      // Étape 2: Son (après délai pour stabiliser)
      delay(DELAI_ENTRE_OPERATIONS);
      jouerSon(17); // 0017 - Je vous vois
      
      // Étape 3: Effets visuels pendant le son
      barrelFlash(RED, 2, 200);
      delay(1000);
      
      // Étape 4: Fermeture
      fermetureRapide();
      setBarrelLeds(OFF);
      break;
      
    case 3: // Mode Tir
      Serial.println("Mode Tir");
      
      // Étape 1: Effets visuels et ouverture
      setEyeColor(WHITE);
      ouvertureRapide();
      eyeBlink(WHITE, 3, 50);
      
      // Étape 2: Son d'imitation tir
      delay(DELAI_ENTRE_OPERATIONS);
      jouerSon(13); // 0013 - Imitation tir
      delay(500);
      
      // Étape 3: Effets visuels de tir
      barrelFlash(WHITE, 4, 100);
      
      // Étape 4: Son de tir
      delay(DELAI_ENTRE_OPERATIONS);
      jouerSon(100); // 0100 - Tir_4x
      
      // Étape 5: Fermeture
      delay(1000);
      fermetureRapide();
      break;
      
    case 4: // Mode Détection
      Serial.println("Mode Détection");
      
      // Étape 1: Effets visuels et ouverture
      setEyeColor(ORANGE);
      ouvertureDouce();
      eyeBlink(ORANGE, 2, 100);
      
      // Étape 2: Son de détection
      delay(DELAI_ENTRE_OPERATIONS);
      jouerSon(28); // 0028 - Trouvé
      
      // Étape 3: Effets visuels pendant le son
      barrelSequence(ORANGE, 150);
      
      // Étape 4: Son supplémentaire
      delay(DELAI_ENTRE_OPERATIONS);
      jouerSon(31); // 0031 - Vous voilà
      
      // Étape 5: Fermeture
      delay(1000);
      fermetureDouce();
      break;
      
    case 5: // Mode Veille
      Serial.println("Mode Veille");
      
      // Étape 1: Effets visuels et ouverture partielle
      setEyeColor(LIGHT_BLUE);
      ouverturePartielle(50);
      
      // Étape 2: Son de veille
      delay(DELAI_ENTRE_OPERATIONS);
      jouerSon(20); // 0020 - Mode veille
      
      // Étape 3: Effets visuels pendant le son
      for (int i = 0; i < 3; i++) {
        leds.setPixelColor(EYE, LIGHT_BLUE);
        leds.show();
        delay(300);
        leds.setPixelColor(EYE, leds.Color(30, 50, 80)); // Bleu plus foncé
        leds.show();
        delay(300);
      }
      
      // Étape 4: Fermeture
      fermetureDouce();
      break;
      
    default:
      // Mode par défaut si le mode n'est pas reconnu
      Serial.println("Mode par défaut");
      setEyeColor(BLUE);
      ouvertureDouce();
      delay(DELAI_ENTRE_OPERATIONS);
      jouerSon(3); // 0003 - Bonjour
      delay(1000);
      fermetureDouce();
      break;
  }
}

// Fonction pour obtenir le nom du son (pour débogage)
String getNomSon(int index) {
  // Mapping voix (0001-0034)
  switch(index) {
    case 1: return "Aurevoir";
    case 2: return "Bonjour mon ami";
    case 3: return "Bonjour";
    case 4: return "Bonne nuit";
    case 5: return "Cible perdu";
    case 6: return "Cible verrouille";
    case 7: return "Deploiement";
    case 8: return "Désolé";
    case 9: return "Erreur Critique";
    case 10: return "Escusez moi";
    case 11: return "Fermeture";
    case 12: return "Il y a quelqun";
    case 13: return "Imitation tir";
    case 14: return "Accident";
    case 15: return "Je ne vous hais pas";
    case 16: return "Je peux aider";
    case 17: return "Je vous voie";
    case 18: return "Sentinel active";
    case 19: return "Mode sieste";
    case 20: return "Mode veille";
    case 21: return "Pardon";
    case 22: return "Qui va la";
    case 23: return "Ravie travail";
    case 24: return "Recherche";
    case 25: return "Repos";
    case 26: return "Salut";
    case 27: return "Sans rancune";
    case 28: return "Trouve";
    case 29: return "Verification";
    case 30: return "Vous tjs la";
    case 31: return "Vous voila";
    case 32: return "Désolé";
    case 33: return "Sentinel active";
    case 34: return "Trouve";
    // Sound effects
    case 100: return "Tir_4x";
    case 101: return "Deploiement";
    case 102: return "Retract";
    case 103: return "Alert";
    case 104: return "Deploy";
    // Music
    case 200: return "Still alive";
    default: return String(index);
  }
}
