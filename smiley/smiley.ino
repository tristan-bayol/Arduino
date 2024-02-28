#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// Déclaration de l'objet pour contrôler l'écran OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  // Initialisation de la communication I2C
  Wire.begin();

  // Initialisation de l'écran OLED avec l'adresse I2C 0x3C (par défaut)
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  // Effacement de l'écran
  display.clearDisplay();

  // Définition de la taille du texte
  display.setTextSize(1);

  // Définition de la couleur du texte (blanc)
  display.setTextColor(SSD1306_WHITE);

  // Dessin du smiley
  drawSmiley();

  // Affichage de l'ensemble du contenu sur l'écran
  display.display();
}

void loop() {
  // On ne fait rien dans la boucle loop() car nous voulons simplement afficher "Hello World" une seule fois.
}

void drawSmiley() {
  // Dessin du cercle extérieur du visage
  display.drawCircle(65, 20, 10, SSD1306_WHITE);

  // Dessin des yeux
  display.drawPixel(62, 17, SSD1306_WHITE); // Yeux gauche
  display.drawPixel(68, 17, SSD1306_WHITE); // Yeux droit

  // Dessin de la bouche (sourire)
  display.drawPixel(61, 22, SSD1306_WHITE);
  display.drawPixel(62, 23, SSD1306_WHITE);
  display.drawPixel(63, 24, SSD1306_WHITE);
  display.drawPixel(64, 25, SSD1306_WHITE);
  display.drawPixel(65, 25, SSD1306_WHITE);
  display.drawPixel(66, 24, SSD1306_WHITE);
  display.drawPixel(67, 23, SSD1306_WHITE);
  display.drawPixel(68, 22, SSD1306_WHITE);
}
