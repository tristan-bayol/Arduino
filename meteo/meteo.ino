#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>
#include <Adafruit_NeoPixel.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_BME280 bme;

#define LED_PIN     13
#define LED_COUNT  16
#define BRIGHTNESS 50

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

enum DisplayMode {
  TEMPERATURE,
  HUMIDITY,
  PRESSURE
};

DisplayMode currentMode = TEMPERATURE;

void setup() {
  Wire.begin();

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

  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();
}

void loop() {
  switch (currentMode) {
    case TEMPERATURE:
      displayAll();
      controlLEDs(bme.readTemperature(), 20, 30, 255, 0, 0); // Rouge pour la température (plage de 20 à 30)
      delay(10000); // Attendre 10 secondes
      break;
    case HUMIDITY:
      displayAll();
      controlLEDs(bme.readHumidity(), 0, 100, 0, 0, 255); // Bleu pour l'humidité (plage de 0 à 100)
      delay(10000); // Attendre 10 secondes
      break;
    case PRESSURE:
      displayAll();
      controlLEDs(bme.readPressure() / 100.0F, 900, 1100, 0, 255, 0); // Vert pour la pression (plage de 900 à 1100)
      delay(10000); // Attendre 10 secondes
      break;
  }

  // Passer à la prochaine mode d'affichage
  currentMode = static_cast<DisplayMode>((currentMode + 1) % 3);
}

void displayAll() {
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
