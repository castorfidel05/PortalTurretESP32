#ifndef SERVO_H
#define SERVO_H

#include <ESP32Servo.h>
#include "../config.h"

// Initialisation du servo
void initServo();

// Contrôle du servo
void setServoPourcentage(float pourcentage, float vitesse);

// Détacher le servo pour économiser l'énergie
void detachServo();

// Variable externe pour accéder au servo
#ifndef MAIN_INO
extern Servo brasServo;
#endif
extern int currentPosition;

#endif // SERVO_H
