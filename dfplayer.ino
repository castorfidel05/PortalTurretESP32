#include "hardware/dfplayer.h"
#include "hardware/leds.h"
#include "behaviors/behaviors.h"

// Initialisation des variables
int dernierSonJoue = -1;

void initDFPlayer() {
  mySoftwareSerial.begin(9600, SERIAL_8N1, 16, 17);
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println("Erreur DFPlayer");
  }
  myDFPlayer.volume(14);
}

void jouerSon(int index) {
  // Utiliser la fonction centralisée pour éviter la duplication de logique
  extern int determinerEtatEmotionnel();
  int etatEmotionnel = determinerEtatEmotionnel();
  
  uint32_t couleurEtat = getCouleurEtat();
  
  Serial.print("Son joué: ");
  Serial.print(index);
  Serial.print(" - État: ");
  Serial.print(nomsEtats[etatEmotionnel]);
  Serial.print(" (");
  Serial.print(etatEmotionnel);
  Serial.println(")");
  
  // Jouer le son
  myDFPlayer.play(index);
  dernierSonJoue = index;
  
  // Démarrer l'effet LED vocal en parallèle
  int dureeEstimee = 3000; // 3 secondes par défaut
  
  // Ajuster la durée selon le type de son
  if (index >= 100) {
    dureeEstimee = 5000; // Sons longs (musique, etc.)
  } else if (index <= 10) {
    dureeEstimee = 1500; // Sons courts (bips, alertes)
  }
  
  // Lancer l'effet LED vocal
  voiceLedEffect(couleurEtat, dureeEstimee, etatEmotionnel);
  
  // Vérifier si le DFPlayer est toujours en train de jouer
  if (myDFPlayer.available()) {
    delay(500);
  }
}

void jouerSonAvecEffet(int index, uint32_t couleurOeil, uint32_t couleurCanon, 
                       bool clignoteOeil, bool clignoteCanon, 
                       int nombreClignements, int delaiClignement) {
  // Appliquer les couleurs de base
  if (couleurOeil != OFF) {
    setEyeColor(couleurOeil);
  }
  
  // Jouer le son
  myDFPlayer.play(index);
  dernierSonJoue = index;
  
  // Utiliser la fonction centralisée pour éviter la duplication de logique
  extern int determinerEtatEmotionnel();
  int etatEmotionnel = determinerEtatEmotionnel();
  
  Serial.print("Son joué avec effet: ");
  Serial.print(index);
  Serial.print(" - État: ");
  Serial.print(nomsEtats[etatEmotionnel]);
  Serial.print(" (");
  Serial.print(etatEmotionnel);
  Serial.println(")");
  
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
  if (myDFPlayer.available()) {
    delay(1000);
  }
}
