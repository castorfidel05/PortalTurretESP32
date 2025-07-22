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
  myDFPlayer.volume(20);
}

void jouerSon(int index) {
  myDFPlayer.play(index);
  dernierSonJoue = index;
  Serial.print("Son joué: ");
  Serial.print(index);
  Serial.print(" - Mode: ");
  Serial.println(nomsComportements[modeCourant]);
  
  // Attendre que le son soit terminé (environ 3 secondes pour la plupart des sons)
  delay(3000);
  
  // Vérifier si le DFPlayer est toujours en train de jouer
  // et attendre un peu plus si nécessaire
  if (myDFPlayer.available()) {
    delay(1000);
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
  if (myDFPlayer.available()) {
    delay(1000);
  }
}
