#include <Arduino_MKRIoTCarrier.h>
#include <Arduino.h>
#include "pitches.h"  // Añadir esta línea para incluir las notas

MKRIoTCarrier carrier;

// Estados posibles para los LEDs
enum LedState { OFF, BLINKING, ON };
LedState ledStates[5];

// Colores de los LEDs con intensidad reducida (valor 128 en lugar de 255)
uint32_t colors[3] = {
  carrier.leds.Color(128, 0, 0),   // Rojo con intensidad reducida
  carrier.leds.Color(0, 128, 0),   // Verde con intensidad reducida
  carrier.leds.Color(0, 0, 128)    // Azul con intensidad reducida
};

int currentColor[5] = {0, 1, 2, 0, 1}; // Color inicial de cada LED

unsigned long lastStateChange[5];
unsigned long stateDuration = 1000; // Duración de parpadeo

int score = 0;
bool colorMatched[3] = {false, false, false}; // Colores ya puntuados

bool gameWon = false; // Variable para saber si el juego ha sido ganado
unsigned long gameWonTime = 0; // Tiempo en que el juego fue ganado
unsigned long lastBlinkTime = 0;  // Última vez que se hizo el parpadeo aleatorio

// Notas de la melodía para "YOU WIN"
int melodyWin[] = { NOTE_G4, NOTE_A4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4 };
int noteDurationsWin[] = { 4, 8, 8, 4, 4, 4, 4, 8, 8, 4, 4 };

// Función para reproducir la melodía
void playMelody(int melody[], int noteDurations[], int length) {
  for (int thisNote = 0; thisNote < length; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    carrier.Buzzer.sound(melody[thisNote]);  // Reproduce la nueva nota
    delay(noteDuration);
    delay(noteDuration * 0.30); // Pausa entre notas
    carrier.Buzzer.noSound();
  }
}

void setup() {
  if (!carrier.begin()) {
    while (1); // Detiene el programa si no se inicializa correctamente
  }

  carrier.leds.fill(0);
  carrier.leds.show();

  carrier.display.fillScreen(ST77XX_BLACK); // Limpia la pantalla
  carrier.display.setTextSize(2);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setCursor(20, 60);
  carrier.display.println("INICIA EL JUEGO!");
  delay(2000);

  for (int i = 0; i < 5; i++) {
    ledStates[i] = OFF;
    lastStateChange[i] = millis();
  }
}

void loop() {
  carrier.Buttons.update();

  for (int i = 0; i < 5; i++) {
    // Manejar botón asociado al LED
    if (carrier.Buttons.getTouch((touchButtons)i)) {
      ledStates[i] = (LedState)((ledStates[i] + 1) % 3); // Cambia al siguiente estado (OFF -> BLINKING -> ON)

      if (ledStates[i] == OFF) { // Cambia el color en el estado OFF
        // Después de pasar por los 3 estados, cambia el color
        currentColor[i] = (currentColor[i] + 1) % 3;
      }
    }

    // Lógica para manejar cada estado
    switch (ledStates[i]) {
      case OFF:
        carrier.leds.setPixelColor(i, 0); // Apagado
        break;
      case BLINKING:
        if ((millis() / 250) % 2 == 0) {
          carrier.leds.setPixelColor(i, colors[currentColor[i]]);
        } else {
          carrier.leds.setPixelColor(i, 0); // Apaga el LED cuando no está parpadeando
        }
        break;
      case ON:
        carrier.leds.setPixelColor(i, colors[currentColor[i]]); // Encendido
        break;
    }
  }

  carrier.leds.show();

  // Verificar si todos los LEDs están en el mismo color y en estado ON
  if (checkWinCondition()) {
    score++;
    playPointBuzzer(); // Reproducir el buzzer cuando se gana un punto

    carrier.display.fillScreen(ST77XX_BLACK);
    carrier.display.setCursor(20, 60);
    carrier.display.setTextSize(2);

    if (score >= 3) {
      carrier.display.println("YOU WIN!");
      playWinBuzzer(); // Reproducir un buzzer especial al ganar el juego
      playMelody(melodyWin, noteDurationsWin, 11); // Reproduce la melodía de "YOU WIN!"
      delay(3000);  // Espera antes de reiniciar el juego
      resetGame();  // Reinicia el juego
    } else {
      carrier.display.print("PUNTO: ");
      carrier.display.println(score);
      delay(2000);
    }
  }
}

// Función para verificar si todos los LEDs tienen el mismo color y están encendidos
bool checkWinCondition() {
  int targetColor = currentColor[0];
  for (int i = 0; i < 5; i++) {
    if (ledStates[i] != ON || currentColor[i] != targetColor) {
      return false;
    }
  }
  // Verificar si el color ya fue puntuado
  if (colorMatched[targetColor]) {
    return false;
  }
  colorMatched[targetColor] = true;
  return true;
}

// Función para tocar el buzzer cuando se gana un punto
void playPointBuzzer() {
  carrier.Buzzer.sound(2000); // Tono de 2000 Hz
  delay(300);
  carrier.Buzzer.noSound();
}

// Función para tocar el buzzer cuando se gana el juego
void playWinBuzzer() {
  carrier.Buzzer.sound(1000); // Tono de 1000 Hz
  delay(500);
  carrier.Buzzer.noSound();
}

// Función para reiniciar el juego
void resetGame() {
  score = 0;
  gameWon = false;
  for (int i = 0; i < 3; i++) {
    colorMatched[i] = false;
  }
  for (int i = 0; i < 5; i++) {
    ledStates[i] = OFF;
    currentColor[i] = i % 3;
  }
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setCursor(20, 60);
  carrier.display.println("INICIA EL JUEGO!"); // Vuelve a mostrar la frase inicial
  delay(2000);
}
