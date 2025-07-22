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

void detachServo() {
  brasServo.detach();
}
