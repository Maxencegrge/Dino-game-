#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>

// --- Définition des broches ---
#define TFT_DC D8
#define TFT_CS D10
#define TFT_RST D7
#define BOUTON USER_BTN   // Bouton pour faire sauter le dino

// --- Création de l’objet écran ---
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// --- Constantes ---
const int SOL_Y = 200;
const int DINO_X = 30;
const int DINO_HAUTEUR = 16;
const int DINO_LARGEUR = 16;
const int VITESSE_SOL = 5;
const int OBSTACLE_LARGEUR = 8;
const int OBSTACLE_HAUTEUR = 12;
const int OBSTACLE_SOL = SOL_Y - OBSTACLE_HAUTEUR;

// --- Variables ---
bool enSaut = false;
int posY_Dino = SOL_Y - DINO_HAUTEUR;
int vitesseSaut = 0;
unsigned long dernierTemps = 0;
unsigned long dernierObstacle = 0;
int obstacleX = 320; // Position de l’obstacle
bool obstacleActif = false;
int score = 0;

// --- Sprite du Dino (simple 16x16 monochrome) ---
const unsigned char dinoBitmap[] PROGMEM = {
  0x00, 0xE0,
  0x01, 0xF0,
  0x03, 0xF8,
  0x07, 0xF8,
  0x07, 0xFC,
  0x07, 0xF8,
  0x03, 0xF8,
  0x03, 0xF8,
  0x03, 0xF8,
  0x03, 0xF0,
  0x03, 0xE0,
  0x03, 0xC0,
  0x03, 0xE0,
  0x03, 0xE0,
  0x00, 0x00,
  0x00, 0x00
};

// --- Fonctions ---
void initAfficheur();
void initSol();
void afficherDino(int x, int y);
void majJeu();
void sautDino();
void majObstacle();
void afficherScore();

void setup() {
  Serial.begin(115200);
  pinMode(BOUTON, INPUT_PULLUP);
  
  tft.begin();
  tft.setRotation(3);
  initAfficheur();

  // Écran d’accueil
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(50, 80);
  tft.print("Projet Dino !");
  tft.setCursor(60, 120);
  tft.print("BOUMADI");
  tft.setCursor(60, 150);
  tft.print("GREGOIRE");
  delay(1500);

  // Efface et affiche le sol
  tft.fillScreen(ILI9341_BLACK);
  initSol();

  afficherDino(DINO_X, posY_Dino);
  afficherScore();
}

void loop() {
  majJeu();
}

// ----------------------
//      FONCTIONS
// ----------------------

void initAfficheur() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
}

void initSol() {
  tft.drawLine(0, SOL_Y, 320, SOL_Y, ILI9341_WHITE);
}

void afficherDino(int x, int y) {
  tft.drawBitmap(x, y, dinoBitmap, 16, 16, ILI9341_WHITE);
}

void effacerDino(int x, int y) {
  tft.fillRect(x, y, 16, 16, ILI9341_BLACK);
}

void majJeu() {
  uint long maintenant = millis();

  // Lecture du bouton
  if (!digitalRead(BOUTON) && !enSaut) {
    sautDino();
  }

  // Mise à jour du saut
  if (enSaut) {
    effacerDino(DINO_X, posY_Dino);
    posY_Dino += vitesseSaut;
    vitesseSaut += 1; // gravité
    if (posY_Dino >= SOL_Y - DINO_HAUTEUR) {
      posY_Dino = SOL_Y - DINO_HAUTEUR;
      enSaut = false;
    }
    afficherDino(DINO_X, posY_Dino);
  }

  // Apparition d’un obstacle
  if (!obstacleActif && (maintenant - dernierObstacle > 1500)) {
    obstacleX = 320;
    obstacleActif = true;
    dernierObstacle = maintenant;
  }

  // Mise à jour de l’obstacle
  if (obstacleActif) {
    tft.fillRect(obstacleX, OBSTACLE_SOL, OBSTACLE_LARGEUR, OBSTACLE_HAUTEUR, ILI9341_BLACK);
    obstacleX -= VITESSE_SOL;

    if (obstacleX + OBSTACLE_LARGEUR < 0) {
      obstacleActif = false;
      score++;
      afficherScore();
    } else {
      tft.fillRect(obstacleX, OBSTACLE_SOL, OBSTACLE_LARGEUR, OBSTACLE_HAUTEUR, ILI9341_WHITE);
    }

    // Détection de collision
    if (DINO_X + DINO_LARGEUR > obstacleX &&
        DINO_X < obstacleX + OBSTACLE_LARGEUR &&
        posY_Dino + DINO_HAUTEUR > OBSTACLE_SOL) {

      tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
      tft.setTextSize(3);
      tft.setCursor(90, 100);
      tft.print("GAME OVER");
      while (1); // stop le jeu
    }
  }

  delay(20);
}

void sautDino() {
  enSaut = true;
  vitesseSaut = -12; // impulsion vers le haut
}

void afficherScore() {
  tft.fillRect(240, 10, 70, 20, ILI9341_BLACK);
  tft.setCursor(240, 10);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_YELLOW);
  tft.print("Score:");
  tft.print(score);
}
