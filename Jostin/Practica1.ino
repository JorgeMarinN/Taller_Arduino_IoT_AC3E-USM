#include <Arduino_MKRIoTCarrier.h>
#include <Arduino.h>
#include "pitches.h"

MKRIoTCarrier carrier;

enum LedState { OFF, BLINKING, ON, FIXED };
LedState ledStates[5];

uint32_t colors[3] = {
  carrier.leds.Color(255, 0, 0),   // Rojo
  carrier.leds.Color(0, 255, 0),   // Verde
  carrier.leds.Color(0, 0, 255)    // Azul
};

int ledColors[5];
unsigned long lastStateChange[5];
unsigned long stateDuration = 1000; 
unsigned long buttonPressStart[5] = {0}; 

int stage = 0; 
uint32_t stageColors[3] = {0}; 
bool colorSelected = false; 

int messageX = 60;
int messageY = 120;

int melody[] = { NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5 };
int noteDurations[] = { 8, 8, 8, 4 };

void setup() {
  if (!carrier.begin()) {
    while (1); 
  }

  carrier.leds.fill(0);
  carrier.leds.show();

  for (int i = 0; i < 5; i++) {
    ledStates[i] = (LedState)random(0, 3);
    ledColors[i] = random(0, 3);
    lastStateChange[i] = millis();
  }

  carrier.display.fillScreen(ST77XX_BLACK); 
}

void loop() {
  if (stage >= 3) {
    stopGame(); 
    return;
  }

  carrier.Buttons.update();

  int fixedCount = 0;

  for (int i = 0; i < 5; i++) {
    if (carrier.Buttons.getTouch((touchButtons)i)) {
      if (buttonPressStart[i] == 0) { 
        buttonPressStart[i] = millis();
      } else if (millis() - buttonPressStart[i] >= 3000) { 
        if (ledStates[i] == FIXED) {
          ledStates[i] = (LedState)random(0, 3); 
        }
        buttonPressStart[i] = 0; 
      }
    } else {
      if (buttonPressStart[i] > 0 && millis() - buttonPressStart[i] < 3000 && ledStates[i] == ON) {
        uint32_t currentColor = colors[ledColors[i]];
        if (!colorSelected) {
          if (stage == 0 || (stage == 1 && currentColor != stageColors[0]) || 
              (stage == 2 && currentColor != stageColors[0] && currentColor != stageColors[1])) {
            stageColors[stage] = currentColor; // Selecciona el color para la etapa
            colorSelected = true;
          }
        }

        if (currentColor == stageColors[stage]) {
          ledStates[i] = FIXED;
        }
      }
      buttonPressStart[i] = 0;
    }

    if (ledStates[i] == FIXED && colorsMatch(stageColors[stage], i)) {
      fixedCount++;
    } else if (ledStates[i] != FIXED && millis() - lastStateChange[i] >= stateDuration) {
      lastStateChange[i] = millis();
      ledStates[i] = (LedState)random(0, 3);
      ledColors[i] = random(0, 3);
    }

    switch (ledStates[i]) {
      case OFF:
        carrier.leds.setPixelColor(i, 0);
        break;
      case BLINKING:
        if ((millis() / 250) % 2 == 0) {
          carrier.leds.setPixelColor(i, colors[ledColors[i]]);
        } else {
          carrier.leds.setPixelColor(i, 0);
        }
        break;
      case ON:
        carrier.leds.setPixelColor(i, colors[ledColors[i]]);
        break;
      case FIXED:
        carrier.leds.setPixelColor(i, stageColors[stage]);
        break;
    }
  }

  carrier.leds.show();

  if (fixedCount == 5) { 
    advanceStage();
  }
}

bool colorsMatch(uint32_t targetColor, int ledIndex) {
  return colors[ledColors[ledIndex]] == targetColor;
}

void advanceStage() {
  stage++;
  colorSelected = false; 
  carrier.display.fillScreen(ST77XX_BLACK); 
  carrier.display.setTextSize(2); 
  carrier.display.setTextColor(ST77XX_WHITE);

  if (stage == 1) {
    carrier.display.setCursor(messageX, messageY);
    carrier.display.print("ETAPA 1 WIN");
    playMelody();
  } else if (stage == 2) {
    carrier.display.setCursor(messageX, messageY);
    carrier.display.print("ETAPA 2 WIN WIN");
    playMelody();
  } else if (stage == 3) {
    carrier.display.setCursor(messageX, messageY);
    carrier.display.print("YOU WIN");
    playMelody();
  }

  for (int i = 0; i < 5; i++) {
    ledStates[i] = (LedState)random(0, 3); 
    ledColors[i] = random(0, 3);
    lastStateChange[i] = millis();
  }
}


void playMelody() {
  for (int thisNote = 0; thisNote < 4; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    carrier.Buzzer.sound(melody[thisNote]);
    delay(noteDuration);
    delay(noteDuration * 0.30); // Pausa entre notas
    carrier.Buzzer.noSound();
  }
}

void stopGame() {
  carrier.leds.fill(0); 
  carrier.leds.show();
  while (1); 
}
