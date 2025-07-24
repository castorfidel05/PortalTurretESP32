#include "hardware/servo.h"

// Initialisation des variables
int currentPosition = ANGLE_FERME; // Dernière position connue (en degrés)

void initServo() {
  brasServo.attach(SERVO_PIN);
  // Position initiale : fermé (100%) à vitesse 100%
  setServoPourcentage(100.0, 100.0);
  delay(500);
  brasServo.detach();
}

void setServoPourcentage(float pourcentage, float vitesse) {
  // Clamp du pourcentage et de la vitesse
  pourcentage = constrain(pourcentage, 0.0, 100.0);
  vitesse = constrain(vitesse, 1.0, 100.0); // éviter 0 pour ne pas bloquer

  int targetAngle = map(pourcentage, 0, 100, ANGLE_OUVERT, ANGLE_FERME);
  int stepDelay = map(vitesse, 1, 100, 30, 1); // plus vitesse haute, moins de delay

  // Debug amélioré
  // Serial.print("[SERVO] Position actuelle: ");
  // Serial.print(positionBrasActuelle);
  // Serial.print("% -> Nouvelle: ");
  // Serial.print(pourcentage);
  // Serial.print("% (");
  if (pourcentage > positionBrasActuelle) {
    // Serial.println("FERMETURE)");
  } else if (pourcentage < positionBrasActuelle) {
    // Serial.println("OUVERTURE)");
  } else {
    // Serial.println("AUCUN MOUVEMENT)");
  }

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
  positionBrasActuelle = pourcentage;  // Mettre à jour la position trackée
  
  // Log de sortie
  // Serial.print("[SERVO] Position finale: ");
  // Serial.print(positionBrasActuelle);
  // Serial.println("%");
}

void detachServo() {
  brasServo.detach();
}
