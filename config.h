#ifndef CONFIG_H
#define CONFIG_H

// === Définitions des pins ===
#define LED_PIN 4
#define SERVO_PIN 23

// === Configuration LED ===
#define NUM_LEDS 5
#define EYE 0
#define CANON_L1 1
#define CANON_L2 2
#define CANON_R1 3
#define CANON_R2 4

// === Configuration Servo ===
#define ANGLE_FERME 0
#define ANGLE_OUVERT 120

// === Configuration DFPlayer ===
// RX=16, TX=17 pour le DFPlayer

// === Gestion du changement de comportement ===
#define INTERVALLE_CHANGEMENT 10000 // 10 secondes
#define NOMBRE_ETATS_EMOTIONNELS 5

// === Couleurs prédéfinies ===
#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF
#define WHITE 0xFFFFFF
#define YELLOW 0xFFFF00
#define ORANGE 0xFF8000
#define VIOLET 0x8000FF
#define PINK 0xFF69B4
#define LIGHT_BLUE 0x87CEFA
#define WARM_WHITE 0xFFDFBA
#define OFF 0x000000
#define BLANC_FROID 0xC8C8FF

// === Noms des états émotionnels ===
extern const char* nomsEtats[NOMBRE_ETATS_EMOTIONNELS+1];

#endif // CONFIG_H
