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