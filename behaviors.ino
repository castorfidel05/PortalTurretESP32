#include "behaviors/behaviors.h"
#include "hardware/dfplayer.h"

// === SYSTÈME DE COMPORTEMENTS ÉMOTIONNELS ===

// Fonction principale pour jouer un comportement émotionnel
void jouerComportementEmotionnel() {
  // Vérifier les interruptions avant de commencer
  verifierInterruptions();
  
  if (interruptionActive) {
    reagirInterruption();
    return;
  }
  
  // Mettre à jour l'humeur
  mettreAJourHumeur();
  
  // Déterminer l'état émotionnel actuel
  int etatActuel = determinerEtatEmotionnel();
  
  // Exécuter le comportement correspondant à l'état
  transitionVersEtat(etatActuel);
  
  // Marquer le temps du dernier comportement
  dernierComportement = millis();
}

// Mise à jour de l'humeur basée sur le temps et les événements
void mettreAJourHumeur() {
  unsigned long maintenant = millis();
  
  // Évolution naturelle de l'humeur avec le temps
  if (maintenant - dernierComportement > 30000) { // 30 secondes
    // Tendance vers la neutralité avec le temps
    if (humeurActuelle > 50) {
      humeurActuelle = max(humeurActuelle - (int)random(1, 4), 50);
    } else if (humeurActuelle < 50) {
      humeurActuelle = min(humeurActuelle + (int)random(1, 4), 50);
    }
  }
  
  // Évolution de l'énergie (diminue avec le temps)
  if (maintenant - dernierComportement > 60000) { // 1 minute
    niveauEnergie = max(niveauEnergie - (int)random(1, 3), 10);
  }
  
  // Évolution de la vigilance (diminue lentement)
  if (maintenant - dernierComportement > 45000) { // 45 secondes
    niveauVigilance = max(niveauVigilance - (int)random(1, 2), 0);
  }
  
  // Contraindre les valeurs
  humeurActuelle = constrain(humeurActuelle, 0, 100);
  niveauEnergie = constrain(niveauEnergie, 0, 100);
  niveauVigilance = constrain(niveauVigilance, 0, 100);
}

// Vérifier s'il y a des interruptions (simule la détection)
void verifierInterruptions() {
  // Simulation d'une détection aléatoire (5% de chance)
  if (random(100) < 5) {
    interruptionActive = true;
    niveauVigilance = min(niveauVigilance + (int)random(10, 21), 100);
    
    // Ajuster l'humeur selon la vigilance
    if (niveauVigilance > 70) {
      humeurActuelle = max(humeurActuelle - (int)random(5, 11), 0);
    }
  } else {
    interruptionActive = false;
  }
}

// Déterminer l'état émotionnel basé sur l'humeur et les niveaux
// Probabilités cibles : Hostile 15%, Vigilant 15%, Amical 20%, Ludique 25%, Neutre 25%
// Valeurs initiales : humeur=50, énergie=80, vigilance=30
int determinerEtatEmotionnel() {
  // État ludique : énergie élevée ET humeur positive (25%)
  if (niveauEnergie > 70 && humeurActuelle > 55) {
    return ETAT_LUDIQUE;
  }
  
  // État amical : humeur élevée ET énergie modérée (20%)
  if (humeurActuelle > 60 && niveauEnergie > 50 && niveauEnergie <= 70) {
    return ETAT_AMICAL;
  }
  
  // État hostile : humeur basse ET (énergie élevée OU vigilance élevée) (15%)
  if (humeurActuelle < 35 && (niveauEnergie > 75 || niveauVigilance > 60)) {
    return ETAT_HOSTILE;
  }
  
  // État vigilant : vigilance élevée OU (humeur moyenne ET énergie moyenne) (15%)
  if (niveauVigilance > 45 || 
      (humeurActuelle >= 40 && humeurActuelle <= 65 && niveauEnergie >= 40 && niveauEnergie <= 75)) {
    return ETAT_VIGILANT;
  }
  
  // État neutre : tous les autres cas (25%)
  return ETAT_NEUTRE;
}

// Transition fluide vers un nouvel état
void transitionVersEtat(int nouvelEtat) {
  // Réinitialiser les LEDs
  leds.clear();
  leds.show();
  // Le servo sera attaché automatiquement dans setServoPourcentage() quand nécessaire
  
  // Exécuter le comportement selon l'état
  switch (nouvelEtat) {
    case ETAT_NEUTRE:
      executerEtatNeutre();
      break;
    case ETAT_AMICAL:
      executerEtatAmical();
      break;
    case ETAT_VIGILANT:
      executerEtatVigilant();
      break;
    case ETAT_HOSTILE:
      executerEtatHostile();
      break;
    case ETAT_LUDIQUE:
      executerEtatLudique();
      break;
    default:
      executerEtatNeutre();
      break;
  }
  
  // Fermeture intelligente : ne fermer que si les bras ne sont pas déjà fermés
  // Serial.print("[DEBUG] Fin de comportement - Position actuelle: ");
  // Serial.print(positionBrasActuelle);
  // Serial.print("% - État: ");
  // Serial.println(nouvelEtat);
  
  if (positionBrasActuelle < 95.0) {  // Seuil de tolérance pour éviter les micro-ajustements
    // Serial.print("Fermeture sécuritaire : position actuelle ");
    // Serial.print(positionBrasActuelle);
    // Serial.println("% -> 100%");
    setServoPourcentage(100.0, 60.0);
  } else {
    // Serial.println("Bras déjà fermés, pas de fermeture redondante");
  }
  delay(500);
  detachServo();
}

// === IMPLÉMENTATION DES ÉTATS ÉMOTIONNELS ===

void executerEtatNeutre() {
  Serial.println("État: NEUTRE");
  
  // Variations aléatoires dans l'état neutre
  int variation = random(1, 4);
  uint32_t couleur = (niveauEnergie > 50) ? WHITE : LIGHT_BLUE;
  
  setEyeColor(couleur);
  
  switch (variation) {
    case 1: // Observation calme
      setServoPourcentage(random(20, 61), random(30, 51));
      if (random(2)) eyeBlink(couleur, random(1, 3), random(150, 301));
      jouerSon(random(2) ? 20 : 25); // Mode veille ou Repos
      break;
      
    case 2: // Mouvement léger
      setServoPourcentage(0.0, random(40, 71));
      delay(random(500, 1001));
      setServoPourcentage(random(30, 71), random(30, 51));
      if (random(3) == 0) jouerSon(29); // Vérification
      break;
      
    case 3: // Pulsation douce
      eyePulse(couleur, random(1, 3), random(150, 256), random(30, 61));
      setServoPourcentage(random(40, 81), random(25, 41));
      break;
  }
  
  setServoPourcentage(100.0, random(30, 51));
}

void executerEtatAmical() {
  Serial.println("État: AMICAL");
  
  int variation = random(1, 4);
  uint32_t couleur = (random(2)) ? WHITE : PINK;
  
  setEyeColor(couleur);
  setServoPourcentage(0.0, random(40, 61));
  
  switch (variation) {
    case 1: // Accueil chaleureux
      setBarrelLeds(couleur);
      jouerSonAvecEffet(random(2) ? 2 : 16, couleur, couleur, false, true, 1, random(100, 201));
      delay(random(800, 1201));
      setBarrelLeds(OFF);
      break;
      
    case 2: // Mouvement amical
      for (int i = 0; i < random(2, 4); i++) {
        eyeBlink(couleur, 1, random(100, 201));
        delay(random(200, 401));
      }
      jouerSon(random(2) ? 26 : 23); // Salut ou Ravie d'avoir travaillé
      break;
      
    case 3: // Effet lumineux doux
      barrelSequence(couleur, random(150, 251));
      jouerSon(random(3) ? 16 : (random(2) ? 21 : 27)); // Aide, Pardon, Sans rancune
      break;
  }
  
  setServoPourcentage(100.0, random(40, 61));
}

void executerEtatVigilant() {
  Serial.println("État: VIGILANT");
  
  int variation = random(1, 4);
  uint32_t couleur = (niveauVigilance > 70) ? RED : ORANGE;
  
  setEyeColor(couleur);
  setServoPourcentage(0.0, random(60, 101));
  
  switch (variation) {
    case 1: // Balayage vigilant
      for (int i = 0; i < random(2, 4); i++) {
        barrelFlash(couleur, 1, random(300, 501));
        delay(random(400, 601));
      }
      jouerSon(random(2) ? 24 : 33); // Recherche ou Mode Sentinel
      break;
      
    case 2: // Alerte modérée
      eyeBlink(couleur, random(3, 6), random(80, 151));
      setBarrelLeds(couleur);
      delay(random(300, 501));
      setBarrelLeds(OFF);
      jouerSon(random(3) ? 17 : (random(2) ? 30 : 22)); // Je vous vois, Vous êtes tjrs là, Qui va là
      break;
      
    case 3: // Mouvement de surveillance
      setServoPourcentage(random(10, 31), random(70, 101));
      delay(random(200, 401));
      setServoPourcentage(0.0, random(80, 101));
      jouerSon(31); // Vous voilà
      break;
  }
  
  setServoPourcentage(random(0, 71), random(50, 81));
}

void executerEtatHostile() {
  Serial.println("État: HOSTILE");
  
  int variation = random(1, 4);
  uint32_t couleur = RED;
  
  setEyeColor(couleur);
  setServoPourcentage(0.0, 100.0);
  
  switch (variation) {
    case 1: // Menace directe
      for (int i = 0; i < random(3, 6); i++) {
        barrelFlash(couleur, 1, random(50, 101));
        delay(random(100, 201));
      }
      jouerSonAvecEffet(17, couleur, couleur, true, true, random(2, 4), random(50, 101));
      break;
      
    case 2: // Tir simulé
      eyeBlink(couleur, random(4, 7), random(30, 81));
      jouerSonAvecEffet(100, couleur, couleur, true, true, 4, 50); // Tir_4x
      for (int i = 0; i < 3; i++) {
        setServoPourcentage(random(0, 11), 100.0);
        delay(50);
        setServoPourcentage(0.0, 100.0);
        delay(50);
      }
      break;
      
    case 3: // Colère contenue
      setBarrelLeds(couleur);
      jouerSon(random(2) ? 9 : 5); // Erreur critique ou Cible perdue
      // Secousse agressive
      for (int i = 0; i < random(2, 4); i++) {
        setServoPourcentage(random(5, 16), 100.0);
        delay(random(30, 71));
        setServoPourcentage(0.0, 100.0);
        delay(random(30, 71));
      }
      setBarrelLeds(OFF);
      break;
  }
  
  setServoPourcentage(100.0, random(80, 101));
}

void executerEtatLudique() {
  Serial.println("État: LUDIQUE");
  
  int variation = random(1, 5); // Ajout du pattern Rainbow Dance
  
  switch (variation) {
    case 1: // Arc-en-ciel ludique
      rainbowEye(random(2, 4), random(10, 31));
      for (int i = 0; i < random(2, 4); i++) {
        barrelWave(leds.Color(random(255), random(255), random(255)), 1, random(80, 121));
      }
      jouerSon(random(3) ? 15 : (random(2) ? 26 : 23)); // Je ne vous hais pas, Salut, Ravie
      break;
      
    case 2: // Clin d'œil espiègle
      setEyeColor(PINK);
      setServoPourcentage(random(40, 81), random(40, 61));
      for (int i = 0; i < random(3, 6); i++) {
        eyeBlink(PINK, 1, random(100, 201));
        delay(random(150, 301));
      }
      jouerSon(random(2) ? 15 : 26); // Je ne vous hais pas ou Salut
      break;
      
    case 3: // Show lumineux
      setServoPourcentage(0.0, random(50, 81));
      for (int i = 0; i < random(3, 6); i++) {
        uint32_t couleurAleatoire = leds.Color(random(255), random(255), random(255));
        setEyeColor(couleurAleatoire);
        barrelFlash(couleurAleatoire, 1, random(100, 201));
        delay(random(200, 401));
      }
      if (random(10) == 0) { // 10% de chance pour l'easter egg
        jouerSon(200); // Still Alive
        rainbowEye(5, 20);
      } else {
        jouerSon(random(2) ? 23 : 27); // Ravie ou Sans rancune
      }
      break;
      
    case 4: // Rainbow Dance - Pattern joueur spectaculaire
      rainbowDance(); // Bras + arc-en-ciel synchronisés
      jouerSon(random(4) ? 15 : (random(3) ? 26 : (random(2) ? 23 : 200))); // Sons joyeux + easter egg
      break;
  }
  
  // Note: rainbowDance() gère déjà la fermeture des bras
  if (random(1, 5) != 4) { // Seulement si ce n'était pas Rainbow Dance
    setServoPourcentage(100.0, random(40, 71));
  }
}

// Réaction aux interruptions (détection simulée)
void reagirInterruption() {
  Serial.println("INTERRUPTION DÉTECTÉE!");
  
  // Réaction immédiate
  leds.clear();
  leds.show();
  brasServo.attach(SERVO_PIN);
  
  setEyeColor(RED);
  setServoPourcentage(0.0, 100.0); // Ouverture rapide
  
  // Effet d'alerte
  for (int i = 0; i < 3; i++) {
    barrelFlash(RED, 1, 100);
    eyeBlink(RED, 1, 100);
    delay(150);
  }
  
  // Son d'alerte selon l'humeur
  if (humeurActuelle < 40) {
    jouerSonAvecEffet(17, RED, RED, true, true, 2, 80); // Je vous vois (hostile)
  } else {
    jouerSon(22); // Qui va là (neutre)
  }
  
  // Ajuster l'état après l'interruption
  humeurActuelle = max(humeurActuelle - (int)random(5, 16), 0);
  niveauVigilance = min(niveauVigilance + (int)random(15, 26), 100);
  
  setServoPourcentage(30.0, 60.0); // Position de surveillance
  delay(500);
  detachServo();
  
  interruptionActive = false;
}

// Fonction de compatibilité avec l'ancien système
void jouerComportement(int mode) {
  // Mapper l'ancien système vers le nouveau
  if (mode == 20) { // Easter egg concert
    executerEtatLudique();
    jouerSon(200); // Still Alive
    return;
  }
  
  // Pour les autres modes, utiliser le système émotionnel
  jouerComportementEmotionnel();
}
