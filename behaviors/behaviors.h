#ifndef BEHAVIORS_H
#define BEHAVIORS_H

#include "../config.h"
#include "../hardware/leds.h"
#include "../hardware/servo.h"
#include "../hardware/dfplayer.h"

// États émotionnels principaux
#define ETAT_NEUTRE 1      // Calme, observateur
#define ETAT_AMICAL 2      // Accueillant, bienveillant
#define ETAT_VIGILANT 3    // Alerte, méfiant
#define ETAT_HOSTILE 4     // Agressif, défensif
#define ETAT_LUDIQUE 5     // Joueur, espiègle

// Variables globales du système d'humeur
#ifndef MAIN_INO
extern int humeurActuelle;        // 0-100 (0=très hostile, 100=très amical)
extern int niveauEnergie;         // 0-100 (0=fatigué, 100=énergique)
extern int niveauVigilance;       // 0-100 (0=détendu, 100=hypervigilant)
extern unsigned long dernierComportement;
extern bool interruptionActive;
extern float positionBrasActuelle;  // Position actuelle des bras (0-100%)
extern unsigned long dernierChangement;
extern int modeCourant;
extern const char* nomsEtats[NOMBRE_ETATS_EMOTIONNELS+1];
#endif

// Déclaration anticipée pour éviter les dépendances circulaires
class DFRobotDFPlayerMini;
void jouerSon(int index);
void jouerSonAvecEffet(int index, uint32_t couleurOeil, uint32_t couleurCanon, 
                      bool clignoteOeil, bool clignoteCanon, 
                      int nombreClignements, int delaiClignement);

// Fonctions de comportement
void jouerComportement(int mode);
void jouerComportementEmotionnel();
void mettreAJourHumeur();
void verifierInterruptions();
int determinerEtatEmotionnel();
void executerEtatNeutre();
void executerEtatAmical();
void executerEtatVigilant();
void executerEtatHostile();
void executerEtatLudique();
void transitionVersEtat(int nouvelEtat);
void reagirInterruption();

#endif // BEHAVIORS_H
