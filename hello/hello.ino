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

  // Affichage de "Hello, world!" sur l'écran à la position (0,0)
  display.setCursor(25,0);
  display.print("Hello, world!");

  // Affichage de l'ensemble du contenu sur l'écran
  display.display();
}

void loop() {
  // On ne fait rien dans la boucle loop() car nous voulons simplement afficher "Hello World" une seule fois.
}
