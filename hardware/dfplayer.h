#ifndef DFPLAYER_H
#define DFPLAYER_H

#include <DFRobotDFPlayerMini.h>
#include <HardwareSerial.h>
#include "../config.h"

// Initialisation du DFPlayer
void initDFPlayer();

// Fonctions de contrôle audio
void jouerSon(int index);
void jouerSonAvecEffet(int index, uint32_t couleurOeil = 0, uint32_t couleurCanon = 0, 
                       bool clignoteOeil = false, bool clignoteCanon = false, 
                       int nombreClignements = 1, int delaiClignement = 100);

// Variables externes pour accéder au DFPlayer
#ifndef MAIN_INO
extern HardwareSerial mySoftwareSerial;
extern DFRobotDFPlayerMini myDFPlayer;
#endif
extern int dernierSonJoue;

#endif // DFPLAYER_H
