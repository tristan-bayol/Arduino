#include <Arduino.h>

int button_count = 0;
int previous_button_state = HIGH; // État précédent du bouton
bool clignoter_led = false; // Indique si la LED est en train de clignoter
bool allumer_led = false;

void setup() {
  pinMode(19, OUTPUT);
  pinMode(13, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Setup");
}

void loop() {
  int button_state = digitalRead(13);

  // Vérifier si le bouton a été enfoncé
  if (button_state == LOW && previous_button_state == HIGH) {
    button_count++; // Incrémenter button_count après chaque appui sur le bouton

    // Gérer les actions en fonction de la valeur de button_count
    if (button_count == 1) {
      Serial.println(button_count);
      digitalWrite(19, HIGH);
      delay(500);
      digitalWrite(19, LOW);
    }
    else if (button_count == 2) { 
      Serial.println(button_count);
      allumer_led = true; // Allume la LED et la laisse allumée
      clignoter_led = false; // Assure que la LED ne clignote pas
    }
    else if (button_count == 3) { 
      Serial.println(button_count);
      clignoter_led = true; // Fait Clignoter la LED
      allumer_led = false; // Assure que la LED est éteinte
    }
    else if (button_count >= 4) {
      button_count = 0;
      Serial.println(button_count);
      clignoter_led = false; // Assure que la LED ne clignote pas
      allumer_led = false; // Assure que la LED est éteinte
    }
  }

  if (allumer_led) {
    digitalWrite(19, HIGH);
  }

  if (clignoter_led) {
    digitalWrite(19, HIGH);
    delay(500);
    digitalWrite(19, LOW);
    delay (500);
  }
  
  // Mettre à jour l'état précédent du bouton
  previous_button_state = button_state;
}
