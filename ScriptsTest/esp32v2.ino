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
int currentPosition = ANGLE_FERME; // Dernière position connue (en degrés)
// Note: La logique a été inversée, maintenant 0% = fermé, 100% = ouvert

// === Gestion du changement de comportement ===
unsigned long dernierChangement = 0;
int modeCourant = 1;
const unsigned long INTERVALLE_CHANGEMENT = 5000; // 15 secondes
const int NOMBRE_COMPORTEMENTS = 3; // Seulement 3 comportements

// === Définition des sons ===
// Voix
#define SON_BONJOUR 3
#define SON_ERREUR_CRITIQUE 9
#define SON_IL_Y_A_QUELQUN 12
#define SON_JE_VOUS_VOIE 17
#define SON_QUI_VA_LA 22
#define SON_VOUS_ETES_TJS_LA 30

// Effets sonores
#define SON_ALERT 103
#define SON_BIP 105  // Nouveau son de bip pour le mode Idle

// === Noms des comportements ===
const char* nomsComportements[NOMBRE_COMPORTEMENTS+1] = {
  "",
  "Mode Idle",          // Comportement 1: Repos actif / Sentinelle passive
  "Mode Stressé",      // Comportement 2: Alerte confuse / Détection erratique
  "Mode Curieuse"       // Comportement 3: En éveil / Mode interrogation
};

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
#define PALE_BLUE leds.Color(173, 216, 230) // Bleu pâle pour le mode Idle
#define WARM_WHITE leds.Color(255, 223, 186)
#define OFF leds.Color(0, 0, 0)
#define CYAN leds.Color(0, 255, 255)        // Cyan pour le mode Curieuse
#define PALE_VIOLET leds.Color(200, 162, 200) // Violet pâle pour le mode Curieuse

int dernierSonJoue = -1;

// === Setup ===
void setup() {
  Serial.begin(115200);
  Serial.println("=== Tourelle Portal - 3 Comportements ===");
  
  mySerial.begin(9600, SERIAL_8N1, 16, 17);
  if (!dfplayer.begin(mySerial)) {
    Serial.println("Erreur DFPlayer");
  } else {
    Serial.println("DFPlayer initialisé");
  }
  dfplayer.volume(20);
  Serial.println("Volume réglé à 20");

  leds.begin();
  leds.show();
  Serial.println("LEDs initialisées");

  brasServo.attach(SERVO_PIN);
  setServoPourcentage(0.0, 100.0); // Position initiale : fermé (0%) à vitesse 100%
  delay(500);
  brasServo.detach();
  Serial.println("Servo initialisé en position fermée");

  randomSeed(analogRead(0)); // Initialiser l'aléa
  modeCourant = 1; // Démarrer avec le mode Idle
  Serial.print("Mode initial: ");
  Serial.println(nomsComportements[modeCourant]);
  jouerComportement(modeCourant);
  dernierChangement = millis();
}

// Fonction loop obligatoire pour Arduino/ESP32
void loop() {
  unsigned long maintenant = millis();
  if (maintenant - dernierChangement >= INTERVALLE_CHANGEMENT) {
    // int nouveauMode = random(1, NOMBRE_COMPORTEMENTS + 1); // de 1 à 3 inclus
    // // Pour éviter de répéter le même comportement deux fois de suite
    // while (nouveauMode == modeCourant) {
    //   nouveauMode = random(1, NOMBRE_COMPORTEMENTS + 1);
    // }
    // modeCourant = nouveauMode;
    // Serial.print("Nouveau mode: ");
    // Serial.println(nomsComportements[modeCourant]);
    // jouerComportement(modeCourant);
    // dernierChangement = maintenant;

    setServoPourcentage(0.0, 100.0);
    Serial.println("Position : fermé (0%) à vitesse 100%");
    delay(1500);
    setServoPourcentage(50.0, 100.0);
    Serial.println("Position : fermé (50%) à vitesse 100%");
    delay(1500);
    setServoPourcentage(100.0, 100.0);
    Serial.println("Position initiale : fermé (100%) à vitesse 100%");
    delay(5000);
  }
  
  // Si en mode Idle, continuer l'effet de respiration
  if (modeCourant == 1) {
    // Appliquer l'effet de respiration continue pour le mode Idle
    breatheEyeEffect();
  }
}

// === Fonction refactorisée pour le contrôle du servo ===
void setServoPourcentage(float pourcentage, float vitesse) {
  // Clamp du pourcentage et de la vitesse
  pourcentage = constrain(pourcentage, 0.0, 100.0);
  vitesse = constrain(vitesse, 1.0, 100.0); // éviter 0 pour ne pas bloquer

  // LOGIQUE INVERSÉE: 0% = fermé (ANGLE_FERME), 100% = ouvert (ANGLE_OUVERT)
  int targetAngle = map(pourcentage, 0, 100, ANGLE_FERME, ANGLE_OUVERT);
  int stepDelay = map(vitesse, 1, 100, 30, 1); // plus vitesse haute, moins de delay

  // Log pour indiquer si on ouvre ou ferme
  if (pourcentage < 50) {
    Serial.println("FERMETURE en cours...");
  } else {
    Serial.println("OUVERTURE en cours...");
  }
  
  Serial.print("Déplacement vers ");
  Serial.print(pourcentage);
  Serial.print("% (");
  Serial.print(targetAngle);
  Serial.print("°) à ");
  Serial.print(vitesse);
  Serial.println("% de vitesse");

  brasServo.attach(SERVO_PIN);

  // Mouvement progressif du servo
  if (currentPosition < targetAngle) {
    for (int pos = currentPosition; pos <= targetAngle; pos++) {
      brasServo.write(pos);
      delay(stepDelay);
    }
  } else {
    for (int pos = currentPosition; pos >= targetAngle; pos--) {
      brasServo.write(pos);
      delay(stepDelay);
    }
  }

  currentPosition = targetAngle;
}

// Fonction pour jouer un son simple
void jouerSon(int index) {
  dfplayer.play(index);
  dernierSonJoue = index;
  Serial.print("Son joué: ");
  Serial.print(index);
  Serial.print(" - Mode: ");
  Serial.println(nomsComportements[modeCourant]);
  
  // Attendre que le son soit terminé (environ 3 secondes pour la plupart des sons)
  delay(3000);
  
  // Vérifier si le DFPlayer est toujours en train de jouer
  // et attendre un peu plus si nécessaire
  if (dfplayer.available()) {
    delay(1000);
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

// Variables pour l'effet de respiration continue
unsigned long dernierBreath = 0;
const unsigned long INTERVALLE_BREATH = 50; // Intervalle entre chaque étape de respiration
int breathDirection = 1; // 1 = augmentation, -1 = diminution
int breathBrightness = 10; // Luminosité actuelle (de 10 à 255)

// Fonction pour l'effet de respiration continue (utilisée en mode Idle)
void breatheEyeEffect() {
  unsigned long maintenant = millis();
  
  // Vérifier si c'est le moment de mettre à jour la luminosité
  if (maintenant - dernierBreath >= INTERVALLE_BREATH) {
    // Mettre à jour la luminosité en fonction de la direction
    breathBrightness += (3 * breathDirection); // Incrément/décrément de 3 pour une vitesse modérée
    
    // Inverser la direction si on atteint les limites
    if (breathBrightness >= 255) {
      breathBrightness = 255;
      breathDirection = -1;
    } else if (breathBrightness <= 10) {
      breathBrightness = 10;
      breathDirection = 1;
    }
    
    // Calculer la couleur entre bleu pâle et blanc
    uint8_t r = map(breathBrightness, 10, 255, 173, 255); // De PALE_BLUE à WHITE
    uint8_t g = map(breathBrightness, 10, 255, 216, 255);
    uint8_t b = map(breathBrightness, 10, 255, 230, 255);
    
    // Appliquer la couleur
    leds.setPixelColor(EYE, leds.Color(r, g, b));
    leds.show();
    
    // Mettre à jour le temps
    dernierBreath = maintenant;
  }
}

// Nouvelle fonction pour alterner les couleurs des canons
void barrelAlternate(uint32_t color1, uint32_t color2, int cycles = 3, int delayMs = 100) {
  for (int c = 0; c < cycles; c++) {
    // Configuration 1
    leds.setPixelColor(CANON_L1, color1);
    leds.setPixelColor(CANON_L2, color2);
    leds.setPixelColor(CANON_R1, color1);
    leds.setPixelColor(CANON_R2, color2);
    leds.show();
    delay(delayMs);
    
    // Configuration 2
    leds.setPixelColor(CANON_L1, color2);
    leds.setPixelColor(CANON_L2, color1);
    leds.setPixelColor(CANON_R1, color2);
    leds.setPixelColor(CANON_R2, color1);
    leds.show();
    delay(delayMs);
  }
}

// Joue un son avec un effet visuel synchronisé
void jouerSonAvecEffet(int index, uint32_t couleurOeil = OFF, uint32_t couleurCanon = OFF, bool clignoteOeil = false, bool clignoteCanon = false, int nombreClignements = 1, int delaiClignement = 100) {
  // Appliquer les couleurs de base
  if (couleurOeil != OFF) {
    setEyeColor(couleurOeil);
  }
  
  // Jouer le son
  dfplayer.play(index);
  dernierSonJoue = index;
  Serial.print("Son joué avec effet: ");
  Serial.print(index);
  Serial.print(" - Mode: ");
  Serial.println(nomsComportements[modeCourant]);
  
  // Appliquer les effets visuels synchronisés avec le son
  if (clignoteOeil) {
    eyeBlink(couleurOeil, nombreClignements, delaiClignement);
  }
  
  if (clignoteCanon) {
    barrelFlash(couleurCanon, nombreClignements, delaiClignement);
  } else if (couleurCanon != OFF) {
    setBarrelLeds(couleurCanon);
  }
  
  // Attendre que le son soit terminé (environ 3 secondes pour la plupart des sons)
  delay(3000);
  
  // Vérifier si le DFPlayer est toujours en train de jouer
  // et attendre un peu plus si nécessaire
  if (dfplayer.available()) {
    delay(1000);
  }
}

// === Comportements refactorisés ===
void jouerComportement(int mode) {
  // Réinitialiser les LEDs
  leds.clear();
  leds.show();
  brasServo.attach(SERVO_PIN);
  
  switch (mode) {
    case 1: // Mode Idle (repos actif / sentinelle passive)
      Serial.println("Mode Idle (repos actif / sentinelle passive)");
      
      // Initialiser l'effet de respiration
      breathBrightness = 50;
      breathDirection = 1;
      
      // Configurer les LEDs
      setEyeColor(PALE_BLUE);
      setBarrelLeds(OFF); // Canons éteints
      
      // Fermer les bras complètement
      setServoPourcentage(100.0, 30.0); // Fermeture lente et douce
      
      // Jouer un bip discret occasionnellement (1 chance sur 3)
      // if (random(3) == 0) {
      //   dfplayer.play(SON_BIP);
      //   delay(500);
      // }
      
      // L'effet de respiration continue est géré dans la fonction loop()
      break;

    case 2: // Mode Stressé (alerte confuse)
      Serial.println("Mode Stressé (alerte confuse)");
      
      // Configurer l'oeil en rouge
      setEyeColor(RED);
      
      // Clignotement rapide de l'oeil (rouge-blanc)
      for (int i = 0; i < 6; i++) {
        setEyeColor(RED);
        delay(100);
        setEyeColor(WHITE);
        delay(100);
      }
      
      // Mouvements saccadés du servo (jitter)
      // Position de base à 30%
      setServoPourcentage(70.0, 80.0);
      
      // Jouer le son d'alerte
      // jouerSon(SON_ALERT);
      
      // Mouvements nerveux aléatoires
      for (int i = 0; i < 5; i++) {
        int position = random(40, 80); // Variations autour de 30%
        setServoPourcentage(position, 100.0); // Vitesse rapide
        
        // Flash des canons en rouge
        barrelFlash(RED, 2, 50);
        delay(200);
      }
      
      // Jouer un son de panique
      // jouerSon(SON_ERREUR_CRITIQUE);
      
      // Clignotement final rapide
      eyeBlink(RED, 5, 80);
      break;

    case 3: // Mode Curieuse (mode interrogation)
      Serial.println("Mode Curieuse (mode interrogation)");
      
      // Configurer l'oeil avec une couleur douce
      setEyeColor(PALE_VIOLET);
      
      // Pulsation lente de l'oeil
      eyePulse(PALE_VIOLET, 2, 200, 300); // Pulsation plus lente
      
      // Ouverture progressive des bras en plusieurs étapes
      // Étape 1: Ouverture légère
      setServoPourcentage(80.0, 80.0); // Ouverture lente
      delay(500);
      
      // Allumer progressivement les LEDs des canons
      for (int i = 0; i < 256; i += 5) {
        float ratio = i / 255.0;
        // Mélange entre violet pâle et cyan
        uint8_t r = 200 - (int)(200 * ratio);
        uint8_t g = 162 + (int)(93 * ratio);
        uint8_t b = 200 + (int)(55 * ratio);
        
        leds.setPixelColor(CANON_L1, leds.Color(r, g, b));
        leds.setPixelColor(CANON_L2, leds.Color(r, g, b));
        leds.setPixelColor(CANON_R1, leds.Color(r, g, b));
        leds.setPixelColor(CANON_R2, leds.Color(r, g, b));
        leds.show();
        delay(10);
      }
      
      // Jouer un son interrogatif
      jouerSon(SON_QUI_VA_LA);
      
      // Étape 2: Ouverture plus large
      setServoPourcentage(50.0, 30.0);
      delay(1000);
      
      // Pulsation des LEDs des canons
      for (int i = 0; i < 3; i++) {
        // Diminuer l'intensité
        for (int j = 255; j > 50; j -= 5) {
          setBarrelLeds(leds.Color(0, j, j)); // Cyan avec intensité variable
          delay(10);
        }
        // Augmenter l'intensité
        for (int j = 50; j < 255; j += 5) {
          setBarrelLeds(leds.Color(0, j, j));
          delay(10);
        }
      }
      
      // Jouer un autre son
      jouerSon(SON_IL_Y_A_QUELQUN);
      
      // Étape 3: Ouverture complète
      setServoPourcentage(80.0, 20.0);
      delay(1500);
      
      // Pulsation finale de l'oeil
      eyePulse(CYAN, 2, 200, 200);
      
      // Jouer un dernier son
      jouerSon(SON_VOUS_ETES_TJS_LA);
      
      // Fermeture lente
      setServoPourcentage(0.0, 20.0);
      break;

    default:
      // Mode par défaut (Idle)
      Serial.println("Mode par défaut (Idle)");
      setEyeColor(PALE_BLUE);
      setServoPourcentage(0.0, 50.0);
      setBarrelLeds(OFF);
      break;
  }

  // Détacher le servo après utilisation pour économiser l'énergie
  delay(500);
  brasServo.detach();
}
