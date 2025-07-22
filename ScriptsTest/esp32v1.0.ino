// === Portal Turret ESP32 v1.0 - Fonction avancée de contrôle ===

// === Bibliothèques nécessaires ===
#include <Adafruit_NeoPixel.h>
#include <DFRobotDFPlayerMini.h>
#include <HardwareSerial.h>
#include <ESP32Servo.h>

// === Définitions des broches et constantes ===
#define LED_PIN 4
#define NUM_LEDS 5
#define SERVO_PIN 23
#define ANGLE_FERME 140
#define ANGLE_OUVERT 0
#define DELAI_ENTRE_OPERATIONS 500

// === Objets ===
Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
HardwareSerial mySerial(2); // RX=16, TX=17
DFRobotDFPlayerMini dfplayer;
Servo brasServo;

// === Définition des index LED ===
#define EYE 0
#define CANON_L1 1
#define CANON_L2 2
#define CANON_R1 3
#define CANON_R2 4

// === Couleurs prédéfinies ===
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

// === Structure pour une étape de mouvement ===
struct TurretMovementStep {
  int percentage;      // Pourcentage d'ouverture (0-100%)
  int speed;           // Vitesse du mouvement (1-100): 1 = très lent, 100 = instantané
  int delayAfter;      // Délai après ce mouvement (en ms)
  bool detachAfter;    // Détacher le servo après ce mouvement?
  int soundToPlay;     // Son à jouer (-1 = aucun son)
  uint32_t eyeColor;   // Couleur de l'œil (-1 = ne pas changer)
  uint32_t barrelColor; // Couleur des canons (-1 = ne pas changer)
  bool barrelFlash;    // Faire clignoter les canons
  int flashCount;      // Nombre de clignotements
  int flashDelay;      // Délai entre les clignotements
};

// === Séquences de mouvements prédéfinies (variables globales) ===
// Séquence principale - alerte et tir
TurretMovementStep complexeSequence[] = {
  // Ouverture rapide à 20%, avec œil rouge, son d'alerte
  {20, 100, 500, false, 103, RED, RED, true, 2, 100},
  
  // Ouverture lente à 80%, avec œil rouge, son "Je vous vois"
  {80, 10, 1000, false, 17, RED, RED, false, 0, 0},
  
  // Maintien avec clignotement des canons et son de tir
  {80, 100, 1000, false, 100, RED, WHITE, true, 4, 100},
  
  // Fermeture partielle avec son "Sans rancune"
  {10, 80, 2000, true, 27, BLUE, OFF, false, 0, 0}
};

// Séquence alternative - détection et veille
TurretMovementStep detectionSequence[] = {
  // Ouverture lente avec œil orange
  {30, 30, 500, false, 28, ORANGE, ORANGE, false, 0, 0},
  
  // Oscillation avec son "Vous voilà"
  {50, 20, 1000, false, 31, ORANGE, ORANGE, true, 2, 200},
  
  // Retour en mode veille
  {20, 40, 1000, true, 20, LIGHT_BLUE, OFF, false, 0, 0}
};

// === Fonction pour convertir un pourcentage en angle ===
int percentageToAngle(int percentage) {
  // Assurer que le pourcentage est entre 0 et 100
  percentage = constrain(percentage, 0, 100);
  // Convertir le pourcentage en angle (0% = ANGLE_FERME, 100% = ANGLE_OUVERT)
  // 0% = 140 degrés (fermé), 100% = 0 degré (ouvert)
  return ANGLE_FERME - (percentage * (ANGLE_FERME - ANGLE_OUVERT) / 100);
}

// === Fonction pour déplacer le servo à une position avec une vitesse contrôlée ===
void moveServoWithSpeed(int currentAngle, int targetAngle, int speed) {
  // Assurer que la vitesse est entre 1 et 100
  speed = constrain(speed, 1, 100);
  
  // Calculer le délai entre chaque pas en fonction de la vitesse
  // Vitesse 1 = délai de 50ms (très lent)
  // Vitesse 100 = délai de 1ms (très rapide)
  int stepDelay = map(speed, 1, 100, 50, 1);
  
  // Calculer la taille du pas en fonction de la vitesse
  // Vitesse 1 = pas de 1 degré
  // Vitesse 100 = pas de 10 degrés
  int stepSize = map(speed, 1, 100, 1, 10);
  // Assurer un pas minimum de 1
  stepSize = max(1, stepSize);
  
  // Déterminer la direction du mouvement
  int step = (currentAngle < targetAngle) ? stepSize : -stepSize;
  
  Serial.print("Déplacement de ");
  Serial.print(currentAngle);
  Serial.print(" à ");
  Serial.print(targetAngle);
  Serial.print(" degrés avec vitesse ");
  Serial.println(speed);
  
  // Si la vitesse est maximale, déplacer directement à la position cible
  if (speed >= 95) {
    brasServo.write(targetAngle);
    delay(100); // Court délai pour stabilisation
    return;
  }
  
  // Sinon, déplacer progressivement
  for (int angle = currentAngle; (step > 0) ? (angle <= targetAngle) : (angle >= targetAngle); angle += step) {
    // S'assurer de ne pas dépasser la cible
    if ((step > 0 && angle > targetAngle) || (step < 0 && angle < targetAngle)) {
      angle = targetAngle;
    }
    
    brasServo.write(angle);
    delay(stepDelay);
  }
  
  // S'assurer que l'angle final est exactement celui demandé
  brasServo.write(targetAngle);
}

// === Fonctions pour contrôler les LEDs ===
void setEyeColor(uint32_t color) {
  if (color != (uint32_t)-1) { // Ne change pas si -1
    leds.setPixelColor(EYE, color);
    leds.show();
  }
}

void setBarrelLeds(uint32_t color) {
  if (color != (uint32_t)-1) { // Ne change pas si -1
    leds.setPixelColor(CANON_L1, color);
    leds.setPixelColor(CANON_L2, color);
    leds.setPixelColor(CANON_R1, color);
    leds.setPixelColor(CANON_R2, color);
    leds.show();
  }
}

void barrelFlash(uint32_t color, int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    setBarrelLeds(color);
    delay(delayMs);
    setBarrelLeds(OFF);
    delay(delayMs);
  }
  // Remettre la couleur à la fin
  setBarrelLeds(color);
}

// === Fonction pour jouer un son ===
void playSoundById(int soundId) {
  if (soundId > 0) {
    Serial.print("Jouer son: ");
    Serial.println(soundId);
    dfplayer.play(soundId);
    
    // Délai basé sur la durée du son (à ajuster selon vos besoins)
    if (soundId >= 1 && soundId <= 34) {
      // Voix - délai court
      delay(300);
    } else if (soundId >= 100 && soundId <= 104) {
      // Effets sonores - délai court
      delay(200);
    } else if (soundId == 200) {
      // Musique - délai plus long
      delay(500);
    }
  }
}

// === Fonction principale pour exécuter une séquence de mouvements ===
void executeTurretSequence(TurretMovementStep sequence[], int numSteps) {
  Serial.println("Début de la séquence de mouvements");
  
  // S'assurer que le servo est attaché
  if (!brasServo.attached()) {
    brasServo.attach(SERVO_PIN);
    Serial.println("Servo attaché");
  }
  
  // S'assurer que les bras sont fermés au début de la séquence
  int currentAngle = brasServo.read();
  if (currentAngle != ANGLE_FERME) {
    Serial.println("Retour à la position fermée avant de commencer la séquence");
    moveServoWithSpeed(currentAngle, ANGLE_FERME, 50);
    delay(500); // Court délai pour stabiliser
  }
  
  for (int i = 0; i < numSteps; i++) {
    // Récupérer les informations de l'étape actuelle
    int percentage = sequence[i].percentage;
    int speed = sequence[i].speed;
    int delayAfter = sequence[i].delayAfter;
    bool detachAfter = sequence[i].detachAfter;
    int soundToPlay = sequence[i].soundToPlay;
    uint32_t eyeColor = sequence[i].eyeColor;
    uint32_t barrelColor = sequence[i].barrelColor;
    bool barrelFlashFlag = sequence[i].barrelFlash;
    int flashCount = sequence[i].flashCount;
    int flashDelay = sequence[i].flashDelay;
    
    // Convertir le pourcentage en angle
    int targetAngle = percentageToAngle(percentage);
    
    // Attacher le servo s'il n'est pas déjà attaché
    if (!brasServo.attached()) {
      brasServo.attach(SERVO_PIN);
      Serial.println("Servo attaché");
    }
    
    // Obtenir l'angle actuel
    currentAngle = brasServo.read();
    
    // Afficher les informations de l'étape
    Serial.print("Etape ");
    Serial.print(i + 1);
    Serial.print("/");
    Serial.print(numSteps);
    Serial.print(": Positionnement à ");
    Serial.print(percentage);
    Serial.print("% (");
    Serial.print(targetAngle);
    Serial.println(" degrés)");
    
    // Définir la couleur de l'œil avant le mouvement
    setEyeColor(eyeColor);
    
    // Définir la couleur des canons avant le mouvement
    setBarrelLeds(barrelColor);
    
    // Déplacer le servo avec la vitesse spécifiée
    moveServoWithSpeed(currentAngle, targetAngle, speed);
    
    // Faire clignoter les canons si demandé
    if (barrelFlashFlag) {
      barrelFlash(barrelColor, flashCount, flashDelay);
    }
    
    // Jouer le son après un court délai pour stabiliser
    if (soundToPlay > 0) {
      delay(DELAI_ENTRE_OPERATIONS);
      playSoundById(soundToPlay);
    }
    
    // Détacher le servo si demandé
    if (detachAfter) {
      brasServo.detach();
      Serial.println("Servo détaché pour réduire les tremblements");
    }
    
    // Attendre le délai spécifié
    delay(delayAfter);
  }
  
  // S'assurer que les bras sont fermés à la fin de la séquence
  if (!brasServo.attached()) {
    brasServo.attach(SERVO_PIN);
    Serial.println("Servo attaché pour fermeture finale");
  }
  
  currentAngle = brasServo.read();
  if (currentAngle != ANGLE_FERME) {
    Serial.println("Retour à la position fermée à la fin de la séquence");
    moveServoWithSpeed(currentAngle, ANGLE_FERME, 50);
    delay(500); // Court délai pour stabiliser
  }
  
  // Détacher le servo après la fermeture pour éviter les tremblements
  brasServo.detach();
  Serial.println("Servo détaché après fermeture finale");
  
  Serial.println("Fin de la séquence de mouvements");
}

// === Exemple d'utilisation dans setup() ===
void setup() {
  Serial.begin(9600);
  
  // Initialisation des LEDs
  leds.begin();
  leds.clear();
  leds.show();
  
  // Initialisation du servo
  brasServo.attach(SERVO_PIN);
  
  // Sécurité: Écrire directement la position fermée avant de lire
  brasServo.write(ANGLE_FERME);
  delay(100); // Court délai pour stabilisation
  
  // Maintenant on peut lire la position actuelle en toute sécurité
  int currentAngle = brasServo.read();
  
  // Vérification de la validité de l'angle lu (entre 0 et 180 degrés)
  if (currentAngle < 0 || currentAngle > 180) {
    Serial.print("Position initiale invalide: ");
    Serial.print(currentAngle);
    Serial.println(" degrés - Correction appliquée");
    currentAngle = ANGLE_FERME; // Utiliser une valeur sûre
  } else {
    Serial.print("Position initiale du servo: ");
    Serial.print(currentAngle);
    Serial.println(" degrés");
  }
  
  // Déplacement progressif vers la position fermée si nécessaire
  if (currentAngle != ANGLE_FERME) {
    moveServoWithSpeed(currentAngle, ANGLE_FERME, 50);
  }
  Serial.println("Servo initialisé en position fermée");
  
  // Initialisation du DFPlayer avec délai pour stabiliser
  delay(500);
  mySerial.begin(9600, SERIAL_8N1, 16, 17);
  if (!dfplayer.begin(mySerial)) {
    Serial.println("Erreur DFPlayer");
  }
  dfplayer.volume(20);
  delay(500);
  
  Serial.println("Turret v1.0 initialisée");
  
    // Exécuter la séquence
  executeTurretSequence(complexeSequence, 4);
}

void loop() {
  // Exemple d'alternance entre deux séquences différentes
  static bool alterneSequence = false;
  
  if (alterneSequence) {
    // Séquence alternative - détection et veille
    executeTurretSequence(detectionSequence, 3);
  } else {
    // Séquence principale - alerte et tir
    executeTurretSequence(complexeSequence, 4);
  }
  
  // Alterner pour la prochaine itération
  alterneSequence = !alterneSequence;
  
  // Attendre avant la prochaine séquence
  delay(3000);
}