# PortalTurretESP32

## Objectif du projet

Créer une réplique fonctionnelle de la tourelle de Portal 2 à base de composants électroniques abordables et faciles à trouver.

## 📦 Matériel utilisé

- ESP32 pour la logique et le contrôle
- LEDs NeoPixel x5 : 1 œil central + 4 canons
- DFPlayer Mini pour jouer des fichiers audio (MP3)
- Servo 9g partagé pour animer l'ouverture/fermeture des deux bras

## 🔌 Plan de Connectique

### Connexions ESP32
```
ESP32 GPIO 4  → Entrée Data des LEDs NeoPixel
ESP32 GPIO 16 → RX du DFPlayer Mini
ESP32 GPIO 17 → TX du DFPlayer Mini
ESP32 GPIO 23 → Signal du servo moteur
```

### Connexions DFPlayer Mini
```
VCC → 5V
GND → GND commun
RX  → GPIO 16 de l'ESP32
TX  → GPIO 17 de l'ESP32
SPK+ → Haut-parleur +
SPK- → Haut-parleur -
```
Une carte microSD doit être insérée avec les fichiers audio numérotés selon le mapping ci-dessous.

### Connexions NeoPixel
```
VIN → 5V
GND → GND commun
DIN → GPIO 4 de l'ESP32
```
Configuration en série : La sortie DOUT de la première LED va au DIN de la suivante.

### Connexions Servo Moteur
```
Signal (fil orange/jaune) → GPIO 23 de l'ESP32
VCC (fil rouge) → 5V
GND (fil marron/noir) → GND commun
```

### Recommandations

## 📁 Structure du Code

Le code a été partitionné en modules pour une meilleure maintenance :

```
PortalTurretESP32/
├── PortalTurretESP32.ino      # Fichier principal (setup, loop)
├── config.h                   # Configuration et constantes
├── hardware/
│   ├── leds.h                 # Déclarations pour les LEDs
│   ├── leds.cpp               # Implémentation des fonctions LED
│   ├── servo.h                # Déclarations pour le servo
│   ├── servo.cpp              # Implémentation des fonctions servo
│   ├── dfplayer.h             # Déclarations pour le DFPlayer
│   └── dfplayer.cpp           # Implémentation des fonctions audio
└── behaviors/
    ├── behaviors.h            # Déclarations des comportements
    └── behaviors.cpp          # Implémentation des comportements
```

### Modules

- **config.h** : Contient toutes les constantes et définitions
- **hardware/** : Contient les modules pour chaque composant matériel
  - **leds** : Gestion des LEDs NeoPixel et effets visuels
  - **servo** : Contrôle du servo moteur pour l'ouverture/fermeture
  - **dfplayer** : Gestion de l'audio via le DFPlayer Mini
- **behaviors/** : Contient la logique des différents comportements de la tourelle
1. **Alimentation** : Utilisez une source 5V/1A minimum avec condensateurs de découplage
2. **Protection** : Ajoutez une résistance de 330-500 ohms sur la ligne de données des NeoPixels
3. **Haut-parleur** : 8 ohms, 3W minimum pour une bonne qualité sonore

## 🎵 Sons disponibles

# Voices
0001 = Aurevoir
0002 = Bonjour mon ami
0003 = Bonjour
0004 = Bonne nuit
0005 = Cible perdu
0006 = Cible verrouille
0007 = Deploiement
0008 = Désolé
0009 = Erreur Critique
0010 = Escusez moi
0011 = Fermeture
0012 = Il y a quelqun
0013 = Imitation tir
0014 = J'ai tout vu c'etait un accident
0015 = Je ne vous hais pas
0016 = Je peux vous aider
0017 = Je vous voie
0018 = Mode Sentinel activé
0019 = Mode sieste
0020 = Mode veille
0021 = Pardon
0022 = Qui va la
0023 = Ravie d'avoir travailler avec vous
0024 = Recherche
0025 = Repos
0026 = Salut
0027 = Sans rancune
0028 = Trouvé
0029 = Verification
0030 = Vous etes tjs la
0031 = Vous voila
0032 = Désolé
0033 = Mode Sentinel activé
0034 = Trouvé

# Sounds Effects
0100 = Tir_4x
0101 = Deploiement
0102 = Retract
0103 = Alert
0104 = Deploy

# Music
0200 = Still alive


✅ Système de Comportements Émotionnels Implémenté
1. 5 États Émotionnels Principaux ✅
NEUTRE : Calme, observateur
AMICAL : Accueillant, bienveillant
VIGILANT : Alerte, méfiant
HOSTILE : Agressif, défensif
LUDIQUE : Joueur, espiègle
2. Système d'Humeur Évolutif ✅
humeurActuelle (0-100) : de très hostile à très amical
niveauEnergie (0-100) : de fatigué à énergique
niveauVigilance (0-100) : de détendu à hypervigilant
Évolution automatique avec le temps vers la neutralité
3. Variations Aléatoires ✅
Chaque état a 3-4 variations différentes qui sont choisies aléatoirement :

Neutre : Observation calme, mouvement léger, pulsation douce
Amical : Accueil chaleureux, mouvement amical, effet lumineux doux
Vigilant : Balayage vigilant, alerte modérée, surveillance
Hostile : Menace directe, tir simulé, colère contenue
Ludique : Arc-en-ciel, clin d'œil espiègle, show lumineux (+ easter egg)
4. Transitions Fluides ✅
La fonction 
determinerEtatEmotionnel()
 analyse l'humeur et les niveaux pour choisir l'état approprié
transitionVersEtat()
 gère le passage d'un état à l'autre de manière fluide
Les états évoluent naturellement selon les conditions
5. Interruptions Occasionnelles ✅
5% de chance d'interruption à chaque cycle (simule la détection)
reagirInterruption()
 déclenche une réaction immédiate
Ajuste automatiquement l'humeur et la vigilance après une interruption
Avantages du Nouveau Système
Plus Humain : Les comportements varient selon l'état émotionnel et évoluent avec le temps
Moins Prévisible : Variations aléatoires et interruptions occasionnelles
Réactif : Réagit aux "événements" et ajuste son comportement
Évolutif : L'humeur change progressivement, créant une personnalité dynamique
Modulaire : Facile d'ajouter de nouveaux états ou de modifier les existants
Utilisation
Le système peut être utilisé de deux façons :

jouerComportementEmotionnel()
c:\Users\pierr\Documents\Projects\PortalTurretESP32\behaviors\behaviors.h
 : Nouveau système émotionnel
jouerComportement(mode)
 : Compatibilité avec l'ancien système (redirige vers le nouveau)