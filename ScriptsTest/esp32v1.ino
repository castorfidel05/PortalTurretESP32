  // === Bibliothèques ===
  #include <Adafruit_NeoPixel.h>
  #include <DFRobotDFPlayerMini.h>
  #include <HardwareSerial.h>
  #include <ESP32Servo.h>
  #include <WiFi.h>
  #include <WebServer.h>
  
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
  const int ANGLE_FERME = 0;  // Angle de fermeture (130 degrés)
  const int ANGLE_OUVERT = 130;   // Angle d'ouverture complète (0 degrés)
  int currentPosition = ANGLE_FERME; // Dernière position connue (en degrés)
  
  // === Gestion du changement de comportement ===
  unsigned long dernierChangement = 0;
  int modeCourant = 1;
  const unsigned long INTERVALLE_CHANGEMENT = 10000; // 10 secondes
  const int NOMBRE_COMPORTEMENTS = 20;

  // === Noms des comportements ===
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
    Serial.begin(115200);
    mySerial.begin(9600, SERIAL_8N1, 16, 17);
    if (!dfplayer.begin(mySerial)) {
      Serial.println("Erreur DFPlayer");
    }
    dfplayer.volume(20);
  
    leds.begin();
    leds.show();
  
    brasServo.attach(SERVO_PIN);
    // Position initiale : fermé (100%) à vitesse 100%
    setServoPourcentage(100.0, 100.0);
    delay(500);
    brasServo.detach();
  
    randomSeed(analogRead(0)); // Initialiser l'aléa
    modeCourant = random(1, NOMBRE_COMPORTEMENTS + 1); // de 1 à 20 inclus
    Serial.print("Mode initial: ");
    Serial.println(nomsComportements[modeCourant]);
    jouerComportement(modeCourant);
    dernierChangement = millis();
  }

  // Fonction loop obligatoire pour Arduino/ESP32
  void loop() {
    unsigned long maintenant = millis();
    if (maintenant - dernierChangement >= INTERVALLE_CHANGEMENT) {
      int nouveauMode = random(1, NOMBRE_COMPORTEMENTS + 1); // de 1 à 20 inclus
      // Pour éviter de répéter le même comportement deux fois de suite
      while (nouveauMode == modeCourant) {
        nouveauMode = random(1, NOMBRE_COMPORTEMENTS + 1);
      }
      modeCourant = nouveauMode;
      Serial.print("Nouveau mode: ");
      Serial.println(nomsComportements[modeCourant]);
      jouerComportement(modeCourant);
      dernierChangement = maintenant;
    }
  }

  // === Fonction refactorisée pour le contrôle du servo ===
  void setServoPourcentage(float pourcentage, float vitesse) {
    // Clamp du pourcentage et de la vitesse
    pourcentage = constrain(pourcentage, 0.0, 100.0);
    vitesse = constrain(vitesse, 1.0, 100.0); // éviter 0 pour ne pas bloquer

    int targetAngle = map(pourcentage, 0, 100, ANGLE_OUVERT, ANGLE_FERME);
    int stepDelay = map(vitesse, 1, 100, 30, 1); // plus vitesse haute, moins de delay

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


  // Nouvelle version de jouerSon
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
      case 1: // Mode Accueil
        Serial.println("Mode Accueil");
        setEyeColor(WHITE);
        setServoPourcentage(0.0, 50.0); // Ouverture complète à vitesse moyenne
        setBarrelLeds(WHITE);
        jouerSon(2); // 0002 - Bonjour mon ami
        delay(1000);
        setServoPourcentage(100.0, 75.0); // Fermeture complète à vitesse moyenne-élevée
        setBarrelLeds(OFF);
        break;
        
      case 2: // Mode En Garde
        Serial.println("Mode En Garde");
        setEyeColor(RED);
        setServoPourcentage(0.0, 100.0); // Ouverture rapide
        // Effet de tir simulé avec les LEDs
        for (int i = 0; i < 4; i++) {
          barrelFlash(RED, 1, 50);
          delay(100);
        }
        jouerSonAvecEffet(17, RED, RED, true, true); // 0017 - Je vous vois
        delay(1500);
        setServoPourcentage(100.0, 100.0); // Fermeture rapide
        setBarrelLeds(OFF);
        break;
        
      case 3: // Mode Sieste
        Serial.println("Mode Sieste");
        setEyeColor(LIGHT_BLUE);
        setServoPourcentage(50.0, 30.0); // 50% ouverture à vitesse lente
        // Effet d'oeil qui s'ouvre et se ferme simulé avec les LEDs
        eyeBlink(LIGHT_BLUE, 3, 200);
        jouerSon(19); // 0019 - Mode sieste
        eyePulse(LIGHT_BLUE, 2);
        setServoPourcentage(100.0, 25.0); // Fermeture lente complète
        break;
        
      case 4: // Mode Furie
        Serial.println("Mode Furie");
        setEyeColor(WHITE);
        for (int i = 0; i < 3; i++) {
          setServoPourcentage(0.0, 100.0); // Ouverture rapide
          eyeBlink(WHITE, 1, 50);
          barrelFlash(WHITE, 1, 50);
          setServoPourcentage(100.0, 100.0); // Fermeture rapide
          delay(200);
        }
        jouerSonAvecEffet(13, WHITE, WHITE, true, true, 1, 50); // 0013 - Imitation tir avec effet visuel
        delay(1000);
        // Effet visuel synchronisé avec le son de tir
        setServoPourcentage(0.0, 100.0); // Ouverture rapide
        jouerSonAvecEffet(100, WHITE, WHITE, true, true, 4, 50); // 0100 - Tir_4x avec effets visuels
        setServoPourcentage(100.0, 100.0); // Fermeture rapide
        break;
        
      case 5: // Mode Confus
        Serial.println("Mode Confus");
        setServoPourcentage(30.0, 50.0); // 70% ouverture à vitesse moyenne
        for (int i = 0; i < 5; i++) {
          setEyeColor(RED);
          delay(100);
          setEyeColor(WHITE);
          delay(150);
        }
        // Oscillation simulée par des mouvements rapides
        for (int i = 0; i < 2; i++) {
          setServoPourcentage(20.0, 80.0);
          delay(300);
          setServoPourcentage(40.0, 80.0);
          delay(300);
        }
        barrelFlash(WHITE, 1, 200);
        jouerSon(9); // 0009 - Erreur critique
        setServoPourcentage(100.0, 25.0); // Fermeture lente complète
        break;
        
      case 6: // Mode Patrouille
        Serial.println("Mode Patrouille");
        setEyeColor(RED);
        setServoPourcentage(0.0, 30.0); // Ouverture lente complète
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
        setServoPourcentage(100.0, 50.0); // Fermeture à vitesse moyenne
        break;
        
      case 7: // Mode Détente
        Serial.println("Mode Détente");
        setEyeColor(GREEN);
        setServoPourcentage(25.0, 40.0); // 75% ouverture à vitesse moyenne-lente
        // Oscillation simulée
        for (int i = 0; i < 2; i++) {
          setServoPourcentage(20.0, 60.0);
          delay(300);
          setServoPourcentage(30.0, 60.0);
          delay(300);
        }
        jouerSon(25); // 0025 - Repos
        setServoPourcentage(100.0, 50.0); // Fermeture à vitesse moyenne
        break;
        
      case 8: // Mode Alert
        Serial.println("Mode Alert");
        setEyeColor(RED);
        setServoPourcentage(0.0, 100.0); // Ouverture rapide
        eyeBlink(RED, 4, 50);
        barrelFlash(RED, 4, 50);
        jouerSon(103); // 0103 - Alert
        setServoPourcentage(100.0, 100.0); // Fermeture rapide
        break;
        
      case 9: // Mode Gentil
        Serial.println("Mode Gentil");
        setEyeColor(PINK);
        setServoPourcentage(0.0, 40.0); // Ouverture douce
        barrelFlash(PINK, 1, 300);
        jouerSon(16); // 0016 - Je peux vous aider
        setServoPourcentage(100.0, 40.0); // Fermeture douce
        break;
        
      case 10: // Mode Déploiement
        Serial.println("Mode Déploiement");
        // Effet de déploiement simulé avec les LEDs
        setEyeColor(WHITE);
        delay(500);
        // Jouer le son pendant l'ouverture pour synchroniser
        jouerSon(7); // 0007 - Déploiement (voix)
        setServoPourcentage(0.0, 40.0); // Ouverture à vitesse moyenne
        setEyeColor(YELLOW);
        // Effet sonore de déploiement avec effet visuel synchronisé
        jouerSonAvecEffet(101, YELLOW, YELLOW, false, true, 2, 100); // 0101 - Deploiement (effet sonore)
        barrelSequence(YELLOW, 100);
        delay(1000);
        setServoPourcentage(100.0, 100.0); // Fermeture rapide
        break;
        
      case 11: // Mode Sentinelle
        Serial.println("Mode Sentinelle");
        setEyeColor(RED);
        setServoPourcentage(0.0, 40.0); // Ouverture douce
        for (int i = 0; i < 3; i++) {
          barrelFlash(RED, 1, 500); // Clignotement lent
          delay(500);
        }
        jouerSon(18); // 0018 - Mode Sentinel activé
        delay(1000);
        setServoPourcentage(50.0, 25.0); // Fermeture partielle (50%) à vitesse lente
        break;
        
      case 12: // Mode Rêveur
        Serial.println("Mode Rêveur");
        setEyeColor(LIGHT_BLUE);
        setServoPourcentage(50.0, 30.0); // 50% ouverture à vitesse lente
        eyePulse(LIGHT_BLUE, 2, 200, 40); // Pulsation lente
        jouerSon(20); // 0020 - Mode veille
        delay(1000);
        setServoPourcentage(100.0, 40.0); // Fermeture douce
        break;
        
      case 13: // Mode Provocateur
        Serial.println("Mode Provocateur");
        setEyeColor(YELLOW);
        setServoPourcentage(0.0, 100.0); // Ouverture rapide
        eyeBlink(YELLOW, 3, 80);
        barrelFlash(YELLOW, 2, 100);
        jouerSon(15); // 0015 - Je ne vous hais pas
        setServoPourcentage(50.0, 100.0); // Fermeture brusque (50%)
        break;
        
      case 14: // Mode Détecté
        Serial.println("Mode Détecté");
        setServoPourcentage(0.0, 30.0); // Ouverture lente
        setEyeColor(RED);
        delay(500);
        setEyeColor(ORANGE);
        eyeBlink(ORANGE, 3, 80);
        // Secousse simulée
        for (int i = 0; i < 3; i++) {
          setServoPourcentage(5.0, 100.0);
          delay(50);
          setServoPourcentage(0.0, 100.0);
          delay(50);
        }
        
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
        setServoPourcentage(100.0, 50.0); // Fermeture à vitesse moyenne
        break;
        
      case 15: // Mode Apaisement
        Serial.println("Mode Apaisement");
        setEyeColor(WARM_WHITE);
        setServoPourcentage(0.0, 30.0); // Ouverture lente
        barrelSequence(WARM_WHITE, 200);
        delay(500);
        setBarrelLeds(OFF);
        jouerSon(27); // 0027 - Sans rancune
        setServoPourcentage(50.0, 20.0); // Fermeture très douce (50%)
        break;
        
      case 16: // Mode Flirt
        Serial.println("Mode Flirt");
        setServoPourcentage(60.0, 40.0); // Ouverture timide (40%)
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
        setServoPourcentage(100.0, 100.0); // Fermeture rapide
        break;
        
      case 17: // Mode Jalousie
        Serial.println("Mode Jalousie");
        setEyeColor(GREEN);
        setServoPourcentage(0.0, 80.0); // Ouverture agressive
        delay(200);
        setEyeColor(RED);
        
        // Synchroniser le son avec l'effet visuel
        jouerSonAvecEffet(29, RED, RED, false, true, 3, 80); // 0029 - Vérification avec battement rapide
        delay(1000);
        
        // Synchroniser le son "Cible perdu" avec la secousse
        jouerSon(5); // 0005 - Cible perdu
        // Secousse plus intense
        for (int i = 0; i < 3; i++) {
          setServoPourcentage(5.0, 100.0);
          delay(50);
          setServoPourcentage(0.0, 100.0);
          delay(50);
        }
        barrelFlash(RED, 1, 100); // Flash rapide
        setServoPourcentage(50.0, 40.0); // Fermeture cassée (50%)
        break;
        
      case 18: // Mode Sadique
        Serial.println("Mode Sadique");
        setEyeColor(WHITE);
        setServoPourcentage(20.0, 30.0); // Ouverture lente (80%)
        delay(300);
        setServoPourcentage(0.0, 100.0); // Snap à ouverture maximale
        
        // Synchroniser le premier son avec l'effet visuel
        jouerSonAvecEffet(14, RED, RED, false, false); // 0014 - J'ai tout vu c'était un accident
        delay(1000);
        
        // Synchroniser le deuxième son avec un effet visuel
        jouerSonAvecEffet(8, RED, RED, true, true, 2, 100); // 0008 - Désolé avec clignotement
        setServoPourcentage(100.0, 50.0); // Fermeture à vitesse moyenne
        setBarrelLeds(OFF);
        break;
        
      case 19: // Mode Politesse Trompeuse
        Serial.println("Mode Politesse Trompeuse");
        setEyeColor(WHITE);
        setServoPourcentage(0.0, 100.0); // Ouverture rapide
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
        setServoPourcentage(100.0, 40.0); // Fermeture douce
        break;
        
      case 20: // Mode Concert (Easter Egg)
        Serial.println("Mode Concert (Easter Egg)");
        setServoPourcentage(0.0, 30.0); // Ouverture lente
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
        setServoPourcentage(100.0, 40.0); // Fermeture douce
        break;
        
      default:
        // Mode par défaut si le mode n'est pas reconnu
        setEyeColor(BLUE);
        setServoPourcentage(0.0, 50.0); // Ouverture à vitesse moyenne
        delay(1000);
        setServoPourcentage(100.0, 50.0); // Fermeture à vitesse moyenne
        break;
    }
    
    // Détacher le servo après utilisation pour économiser l'énergie
    delay(500);
    brasServo.detach();
  }
