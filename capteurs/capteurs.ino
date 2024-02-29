#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "SHCSR04.h"
#include <SPI.h>//https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>//https://github.com/miguelbalboa/rfid

// Lecteur Badge
#define SS_PIN 5
#define RST_PIN 4

// Ecran
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Bandeau Led
#define LED_PIN     33
#define LED_COUNT  16
#define BRIGHTNESS 50
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Capteur Météo
Adafruit_BME280 bme;
// Gyroscope
Adafruit_MPU6050 mpu;
// Capteur de distance
SHCSR04 hcsr04;

//Lecteur Badges
//Parameters
const int ipaddress[4] = {103, 97, 67, 25};
//Variables
byte nuidPICC[4] = {0, 0, 0, 0};
MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);


enum DisplayMode {
  TEMPERATURE,
  HUMIDITY,
  PRESSURE
};

DisplayMode currentMode = TEMPERATURE;

void setup() {
  Wire.begin();
  SPI.begin();
  rfid.PCD_Init();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1);
  }
  Serial.println("MPU6050 Found!");
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();
  Serial.begin(9600);  // Initialisation de la communication série pour le capteur de distance
}

void loop() {
  // Mesure de la distance avec le capteur HC-SR04
  unsigned int distance = hcsr04.read(32, 35); // Assurez-vous d'utiliser les broches appropriées pour le déclencheur (trigger) et l'écho (echo)
  // Vérifier si la distance est inférieure à 10 cm
  if (distance < 10) {
    // Obtenir les données gyroscopiques
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Calculer l'angle par rapport à l'horizontale
    float angle = atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180.0 / PI;

    if (abs(angle) <= 10) { // Si l'appareil est plat
      displayAll(distance, angle);

      switch (currentMode) {
        case TEMPERATURE:
          controlLEDs(bme.readTemperature(), 20, 30, 255, 0, 0); // Rouge pour la température (plage de 20 à 30)
          break;
        case HUMIDITY:
          controlLEDs(bme.readHumidity(), 0, 100, 0, 0, 255); // Bleu pour l'humidité (plage de 0 à 100)
          break;
        case PRESSURE:
          controlLEDs(bme.readPressure() / 100.0F, 900, 1050, 0, 255, 0); // Vert pour la pression (plage de 900 à 1100)
          break;
      }
    } else { // Si l'appareil n'est pas plat
      display.setRotation(0); // Orientation de l'écran par défaut
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.clearDisplay();
      display.setCursor(0, 16);
      display.println("POSE CA DESUITE");
      display.display();
      display.print("Angle: ");
      display.print(angle);
      display.println(" degrees");
      display.display();
      controlLEDs(0, 0, 0, 0, 0, 0); // Éteindre les LEDs
    }
  } else {
    display.clearDisplay();
    display.display(); // Éteindre l'écran
    controlLEDs(0, 0, 0, 0, 0, 0); // Éteindre les LEDs
  }

  readRFID(); // appel la fonction du lecteur de badge
  
  // Passer à la prochaine mode d'affichage
  currentMode = static_cast<DisplayMode>((currentMode + 1) % 3);

  delay(1000); // Attendre 1 seconde avant la prochaine itération
}

void displayAll(unsigned int distance, float angle) {
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Temperature: ");
  display.print(bme.readTemperature());
  display.println(" C");
  display.print("Humidity: ");
  display.print(bme.readHumidity());
  display.println(" %");
  display.print("Pressure: ");
  display.print(bme.readPressure() / 100.0F);
  display.println(" hPa");
  display.print("Distance: ");
  display.print(distance);
  display.println(" cm");
  display.print("Angle: ");
  display.print(angle);
  display.println(" degrees");
  display.display();
}

void controlLEDs(float value, float minValue, float maxValue, uint8_t red, uint8_t green, uint8_t blue) {
  int pixelsToLight = map(value, minValue, maxValue, 0, LED_COUNT);
  for (int i = 0; i < LED_COUNT; i++) {
    if (i < pixelsToLight) {
      strip.setPixelColor(i, strip.Color(red, green, blue));
    } else {
      strip.setPixelColor(i, 0); // Éteindre les LEDs non utilisées
    }
  }
  strip.show();

}

// Variable globale pour stocker l'UID
unsigned long storedUID = 0;

void readRFID(void ) { /* function readRFID */
  ////Read RFID card

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  // Look for new 1 cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if (  !rfid.PICC_ReadCardSerial())
    return;

  // Store NUID into nuidPICC array
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }

  // Store the UID into storedUID variable
  storedUID = ((unsigned long)rfid.uid.uidByte[0] << 24) |
              ((unsigned long)rfid.uid.uidByte[1] << 16) |
              ((unsigned long)rfid.uid.uidByte[2] << 8) |
              rfid.uid.uidByte[3];

  Serial.print(F("RFID In dec: "));
  printDec(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();

  // Display the UID on the LCD
  displayUID(storedUID);

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();

}


/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
   Helper routine to dump a byte array as dec values to Serial.
*/
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

// Fonction pour afficher l'UID sur l'écran LCD et contrôler les LED en fonction de l'UID
void displayUID(unsigned long uid) {
  if (uid == 1394986999) { // Exemple d'UID à vérifier
    controlLEDs(15, 0, 16, 255, 255, 0); // Jaune
    // Afficher un message spécifique sur l'écran pour cet UID
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Acces AUTORISE:");
    display.println("Bonjour Aicha :)");
    display.display();
  } 
 else if (uid == 2474243602) { // Exemple d'UID à vérifier
    controlLEDs(8, 0, 16, 127, 0, 255); // Violet
    // Afficher un message spécifique sur l'écran pour cet UID
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Acces AUTORISE:");
    display.println("Bonjour Laurent");
    display.display();
  } 
  else if (uid == 1667189267) { // Exemple d'UID à vérifier
    controlLEDs(12, 0, 16, 112, 240, 35); // Vert Olive
    // Afficher un message spécifique sur l'écran pour cet UID
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Acces AUTORISE:");
    display.println("Bonjour SenseiTyTy");
    display.display();
  }

  else {
    controlLEDs(16, 0, 16, 255, 255, 255); // Blanc
    // Afficher un message par défaut sur l'écran pour les autres UID
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("UID non autorise");
    display.println(uid); // Afficher l'UID non autorisé
    display.display();
  }
  delay(5000); // Attendre 5 secondes avant de continuer
}