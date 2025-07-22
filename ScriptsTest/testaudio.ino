// === Bibliothèques ===
#include <Adafruit_NeoPixel.h>
#include <DFRobotDFPlayerMini.h>
#include <HardwareSerial.h>

// === DFPlayer ===
HardwareSerial mySerial(2); // RX=16, TX=17 
DFRobotDFPlayerMini dfplayer;

// === Définition des sons ===
// Voix
#define SON_AUREVOIR 1
#define SON_BONJOUR_MON_AMI 2
#define SON_BONJOUR 3
#define SON_BONNE_NUIT 4
#define SON_CIBLE_PERDU 5
#define SON_CIBLE_VERROUILLE 6
#define SON_DEPLOIEMENT 7
// #define SON_DESOLE 8
#define SON_ERREUR_CRITIQUE 9
#define SON_EXCUSEZ_MOI 10
#define SON_FERMETURE 11
#define SON_IL_Y_A_QUELQUN 12
#define SON_IMITATION_TIR 13
#define SON_ACCIDENT 14
#define SON_JE_NE_VOUS_HAIS_PAS 15
#define SON_JE_PEUX_VOUS_AIDER 16
#define SON_JE_VOUS_VOIE 17
// #define SON_MODE_SENTINEL 18
#define SON_MODE_SIESTE 19
#define SON_MODE_VEILLE 20
#define SON_PARDON 21
#define SON_QUI_VA_LA 22
#define SON_RAVIE_TRAVAILLER 23
#define SON_RECHERCHE 24
#define SON_REPOS 25
#define SON_SALUT 26
#define SON_SANS_RANCUNE 27
// #define SON_TROUVE 28
#define SON_VERIFICATION 29
#define SON_VOUS_ETES_TJS_LA 30
#define SON_VOUS_VOILA 31

// Effets sonores
#define SON_TIR_4X 100
#define SON_DEPLOIEMENT_SFX 101
#define SON_RETRACT 102
#define SON_ALERT 103
#define SON_DEPLOY 104

// Musique
#define SON_STILL_ALIVE 200

// Structure pour stocker les informations sur les sons
struct SoundInfo {
  int id;
  const char* name;
  int delayMs;  // Durée estimée du son en millisecondes
};

// Tableau des informations sur les sons
const SoundInfo soundList[] = {
  {SON_AUREVOIR, "Aurevoir", 3000},
  {SON_BONJOUR_MON_AMI, "Bonjour mon ami", 3000},
  {SON_BONJOUR, "Bonjour", 2000},
  {SON_BONNE_NUIT, "Bonne nuit", 2500},
  {SON_CIBLE_PERDU, "Cible perdu", 2500},
  {SON_CIBLE_VERROUILLE, "Cible verrouillée", 3000},
  {SON_DEPLOIEMENT, "Déploiement", 3000},
  // {SON_DESOLE, "Désolé", 2000},
  {SON_ERREUR_CRITIQUE, "Erreur critique", 3500},
  {SON_EXCUSEZ_MOI, "Excusez-moi", 2000},
  {SON_FERMETURE, "Fermeture", 2500},
  {SON_IL_Y_A_QUELQUN, "Il y a quelqu'un", 2500},
  {SON_IMITATION_TIR, "Imitation tir", 3000},
  {SON_ACCIDENT, "J'ai tout vu c'était un accident", 4000},
  {SON_JE_NE_VOUS_HAIS_PAS, "Je ne vous hais pas", 3000},
  {SON_JE_PEUX_VOUS_AIDER, "Je peux vous aider", 3000},
  {SON_JE_VOUS_VOIE, "Je vous vois", 2500},
  // {SON_MODE_SENTINEL, "Mode Sentinel activé", 3500},
  {SON_MODE_SIESTE, "Mode sieste", 3000},
  {SON_MODE_VEILLE, "Mode veille", 3000},
  {SON_PARDON, "Pardon", 2000},
  {SON_QUI_VA_LA, "Qui va là", 2500},
  {SON_RAVIE_TRAVAILLER, "Ravie d'avoir travaillé avec vous", 4000},
  {SON_RECHERCHE, "Recherche", 2500},
  {SON_REPOS, "Repos", 2000},
  {SON_SALUT, "Salut", 2000},
  {SON_SANS_RANCUNE, "Sans rancune", 3000},
  // {SON_TROUVE, "Trouvé", 2000},
  {SON_VERIFICATION, "Vérification", 2500},
  {SON_VOUS_ETES_TJS_LA, "Vous êtes toujours là", 3000},
  {SON_VOUS_VOILA, "Vous voilà", 2500},
  {SON_TIR_4X, "Tir 4x (effet sonore)", 4000},
  {SON_DEPLOIEMENT_SFX, "Déploiement (effet sonore)", 3500},
  {SON_RETRACT, "Retract (effet sonore)", 3000},
  {SON_ALERT, "Alert (effet sonore)", 3000},
  {SON_DEPLOY, "Deploy (effet sonore)", 3000},
  {SON_STILL_ALIVE, "Still Alive (musique)", 15000}
};

const int SOUND_COUNT = sizeof(soundList) / sizeof(soundList[0]);
int currentSoundIndex = 0;
unsigned long lastSoundTime = 0;
const unsigned long EXTRA_DELAY = 1000; // Délai supplémentaire entre les sons

void setup() {
  Serial.begin(115200);
  Serial.println("=== Test Audio Tourelle Portal ===");
  Serial.println("Initialisation du DFPlayer...");
  
  mySerial.begin(9600, SERIAL_8N1, 16, 17);
  if (!dfplayer.begin(mySerial)) {
    Serial.println("ERREUR: Impossible d'initialiser le DFPlayer!");
    while(1); // Arrêt si erreur
  }
  
  Serial.println("DFPlayer initialisé avec succès.");
  dfplayer.volume(20); // Volume à 20 (sur 30)
  Serial.println("Volume réglé à 20/30");
  
  Serial.println("Démarrage du test de tous les sons...");
  Serial.println("-----------------------------------");
  lastSoundTime = millis();
}

void loop() {
  unsigned long currentTime = millis();
  
  // Si nous avons terminé tous les sons, recommencer
  if (currentSoundIndex >= SOUND_COUNT) {
    Serial.println("\n=== Test terminé, redémarrage... ===\n");
    currentSoundIndex = 0;
    delay(5000); // Attendre 5 secondes avant de recommencer
  }
  
  // Jouer le son suivant si le délai est écoulé
  if (currentTime - lastSoundTime >= soundList[currentSoundIndex].delayMs + EXTRA_DELAY) {
    const SoundInfo& sound = soundList[currentSoundIndex];
    
    Serial.print("Test son #");
    Serial.print(currentSoundIndex + 1);
    Serial.print("/");
    Serial.print(SOUND_COUNT);
    Serial.print(" - ID: ");
    Serial.print(sound.id);
    Serial.print(" - ");
    Serial.print(sound.name);
    Serial.print(" - Durée estimée: ");
    Serial.print(sound.delayMs);
    Serial.println("ms");
    
    // Jouer le son
    dfplayer.playMp3Folder(sound.id);
    
    // Passer au son suivant
    currentSoundIndex++;
    lastSoundTime = currentTime;
  }
}