/*
 * PortalTurretESP32 - Projet de tourelle Portal
 * Compatible avec ESP32 WROOM via Arduino IDE 2.3.5
 */

// === Bibliothèques ===
#include <Adafruit_NeoPixel.h>
#include <DFRobotDFPlayerMini.h>
#include <HardwareSerial.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>

// === Inclusions des modules ===
#include "config.h"

// Déclaration des variables globales
int modeCourant = 1;
unsigned long dernierChangement = 0;

// Variables du système d'humeur
int humeurActuelle = 50;        // 0-100 (0=très hostile, 100=très amical)
int niveauEnergie = 80;         // 0-100 (0=fatigué, 100=énergique)
int niveauVigilance = 30;       // 0-100 (0=détendu, 100=hypervigilant)
unsigned long dernierComportement = 0;
bool interruptionActive = false;
float positionBrasActuelle = 100.0;  // Position actuelle des bras (0-100%)

// Variables pour la respiration LED
unsigned long dernierBreathing = 0;
const unsigned long intervalleBreathing = 3000; // Respiration toutes les 3 secondes

// Noms des états émotionnels (pour le débogage)
const char* nomsEtats[NOMBRE_ETATS_EMOTIONNELS+1] = {
  "NONE",
  "NEUTRE",
  "AMICAL", 
  "VIGILANT",
  "HOSTILE",
  "LUDIQUE"
};

// Initialisation de l'objet NeoPixel
Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Initialisation du servo
Servo brasServo;

// Initialisation du DFPlayer
HardwareSerial mySoftwareSerial(1);
DFRobotDFPlayerMini myDFPlayer;

// Inclusions des fichiers d'en-tête
#include "hardware/leds.h"
#include "hardware/servo.h"
#include "hardware/dfplayer.h"
#include "behaviors/behaviors.h"

// === Setup ===
void setup() {
  Serial.begin(115200);
  Serial.println("Initialisation de la tourelle Portal...");
  
  // Initialisation des composants
  initLeds();
  initServo();
  initDFPlayer();

  randomSeed(analogRead(0)); // Initialiser l'aléa
  
  // Initialisation du système émotionnel
  Serial.println("Système émotionnel activé!");
  Serial.print("Humeur initiale: "); Serial.println(humeurActuelle);
  Serial.print("Énergie initiale: "); Serial.println(niveauEnergie);
  Serial.print("Vigilance initiale: "); Serial.println(niveauVigilance);
  
  // Premier comportement émotionnel
  jouerComportementEmotionnel();
  dernierChangement = millis();
  
  Serial.println("Initialisation terminée!");
}

// === Loop ===
void loop() {
  unsigned long maintenant = millis();
  
  // Utiliser le nouveau système émotionnel avec des intervalles variables
  unsigned long intervalleVariable = random(8000, 15000); // 8-15 secondes (plus naturel)
  
  if (maintenant - dernierChangement >= intervalleVariable) {
    // Afficher l'état émotionnel actuel
    Serial.print("[Humeur: "); Serial.print(humeurActuelle);
    Serial.print(", Énergie: "); Serial.print(niveauEnergie);
    Serial.print(", Vigilance: "); Serial.print(niveauVigilance); Serial.println("]");
    
    // Exécuter le comportement émotionnel
    jouerComportementEmotionnel();
    dernierChangement = maintenant;
  }
  
  // Effet de respiration LED entre les comportements
  if (maintenant - dernierBreathing >= intervalleBreathing) {
    breathingEffect();
    dernierBreathing = maintenant;
  }
  
  // Petite pause pour éviter la surcharge du processeur
  delay(100);
}
