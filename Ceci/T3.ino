
#include <Arduino.h>
#include <Arduino_MKRIoTCarrier.h>
#include <Arduino_APDS9960.h>
#include "pitches.h"

MKRIoTCarrier carrier;

int currentMenu = 0;

void setup() {
  carrier.begin();
  carrier.display.fillScreen(ST77XX_BLACK);

  if (!APDS.begin()) {
    carrier.display.fillScreen(ST77XX_RED);
    carrier.display.setCursor(10, 60);
    carrier.display.setTextSize(2);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.println("Error APDS!");
    while (true);
  }
  carrier.leds.begin();
  displayMainMenu();
}

void loop() {
  carrier.Buttons.update();
  if (carrier.Buttons.getTouch(TOUCH1)) {
    detectGestures();
  } else if (carrier.Buttons.getTouch(TOUCH2)) {
    detectColor(); 
  } else if (carrier.Buttons.getTouch(TOUCH4)) {
    detectProximity();
  } else if (carrier.Buttons.getTouch(TOUCH3)) {
    displayMainMenu();
  }
}

void displayMainMenu() {
  currentMenu = 0;
  carrier.display.fillScreen(ST77XX_ORANGE);
  carrier.display.setTextColor(ST77XX_CYAN);
  carrier.display.setTextSize(5);
  carrier.display.setCursor(50, 50);
  carrier.display.println("MENU:");

  carrier.display.setTextSize(2);
  carrier.display.setCursor(40, 110);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.println("Gestos: 01 ");
  carrier.display.setCursor(40, 130);
  carrier.display.setTextColor(ST77XX_MAGENTA);
  carrier.display.println("Colores: 02");
  carrier.display.setCursor(40, 150);
  carrier.display.setTextColor(ST77XX_BLUE);
  carrier.display.println("Proximidad: 04");
  carrier.display.setCursor(40, 170);
  carrier.display.setTextColor(ST77XX_RED);
  carrier.display.println("Regresar: 03");
}

void detectGestures() {
  currentMenu = 1;
  carrier.display.fillScreen(ST77XX_WHITE);
  carrier.display.setTextSize(4);
  carrier.display.setTextColor(ST77XX_BLACK);
  carrier.display.setCursor(50, 100);
  carrier.display.println("GESTOS");

  while (true) {
    carrier.Buttons.update();

    if (carrier.Buttons.getTouch(TOUCH3)) {
      displayMainMenu();
      return;
    }

    if (APDS.gestureAvailable()) {
      int gesture = APDS.readGesture();
      carrier.display.fillScreen(ST77XX_WHITE);

      // Dibujar flechas según el gesto detectado
      switch (gesture) {
        case GESTURE_UP: // Gesto hacia arriba
          carrier.display.fillTriangle(60, 120, 120, 80, 120, 160, ST77XX_YELLOW);
          carrier.display.fillRect(120, 100, 60, 40, ST77XX_YELLOW);
          playTone1(NOTE_B0, NOTE_C1);
          break;

        case GESTURE_DOWN: // Gesto hacia abajo
          carrier.display.fillTriangle(180, 120, 120, 80, 120, 160, ST77XX_BLUE);
          carrier.display.fillRect(60, 100, 60, 40, ST77XX_BLUE);
          playTone1(NOTE_B0, NOTE_C1);
          break;

        case GESTURE_LEFT: // Gesto hacia la izquierda
          carrier.display.fillTriangle(120, 180, 80, 120, 160, 120, ST77XX_MAGENTA);
          carrier.display.fillRect(100, 60, 40, 60, ST77XX_MAGENTA);
          playTone1(NOTE_B0, NOTE_C1);
          break;

        case GESTURE_RIGHT: // Gesto hacia la derecha
          carrier.display.fillTriangle(120, 60, 80, 120, 160, 120, ST77XX_RED);
          carrier.display.fillRect(100, 120, 40, 60, ST77XX_RED); 
          playTone1(NOTE_B0, NOTE_C1);      
          break;

        default:
          carrier.display.setCursor(80, 60);
          carrier.display.setTextColor(ST77XX_BLACK);
          carrier.display.println("NO DETECTADO");
          break;
      }

      Serial.print("Gesto detectado: ");
      Serial.println(gesture);

      delay(1000);
      carrier.display.fillScreen(ST77XX_WHITE);
      carrier.display.setTextColor(ST77XX_BLACK);
      carrier.display.setTextSize(4);
      carrier.display.setCursor(50, 100);
      carrier.display.println("GESTOS");
    }
  }
}

void detectColor() {
  currentMenu = 2;
  carrier.display.fillScreen(ST77XX_MAGENTA);
  carrier.display.setTextSize(4);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setCursor(30, 100);
  carrier.display.println("COLORES");

  while (true) {
    carrier.Buttons.update();
    if (carrier.Buttons.getTouch(TOUCH3)) {
      displayMainMenu();
      return;
    }
    if (APDS.colorAvailable()) {
      int r, g, b;
      APDS.readColor(r, g, b);

      // Detectar color blanco con los valores originales
      if (r > 200 && g > 200 && b > 200 && abs(r - g) < 50 && abs(g - b) < 50 && abs(r - b) < 50) {
        carrier.display.fillScreen(ST77XX_MAGENTA);
        carrier.display.setCursor(15, 80);
        carrier.display.setTextColor(ST77XX_WHITE);
        carrier.display.setTextSize(4);
        carrier.display.println("DETECTADO");
      } else {
        // Mostrar los valores RGB originales
        carrier.display.fillScreen(ST77XX_MAGENTA);
        carrier.display.setCursor(60, 60);
        carrier.display.setTextSize(3);
        carrier.display.println("COLORES");
        carrier.display.setCursor(80, 100);
        carrier.display.setTextSize(2);
        carrier.display.print("R: ");
        carrier.display.println(r);
        carrier.display.setCursor(80, 120);
        carrier.display.setTextSize(2);
        carrier.display.print("G: ");
        carrier.display.println(g);
        carrier.display.setCursor(80, 140);
        carrier.display.setTextSize(2);
        carrier.display.print("B: ");
        carrier.display.println(b);
      }

      delay(500);
    }
  }
}


void detectProximity() {
  currentMenu = 3;

  if (!APDS.begin()) {
    carrier.display.fillScreen(ST77XX_BLACK);
    carrier.display.setCursor(10, 60);
    carrier.display.setTextSize(2);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.println("Error APDS!");
    return;
  }

  carrier.display.fillScreen(ST77XX_BLUE);
  carrier.display.setTextSize(3);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setCursor(20, 100);
  carrier.display.println("PROXIMIDAD");

  while (true) {
    carrier.Buttons.update();

    if (carrier.Buttons.getTouch(TOUCH3)) {
      carrier.leds.setBrightness(0); 
      carrier.leds.show();
      displayMainMenu();
      return;
    }

    if (APDS.proximityAvailable()) {
      int proximity = APDS.readProximity();
      carrier.display.fillScreen(ST77XX_RED);
      carrier.display.setCursor(30, 90);
      carrier.display.setTextSize(3);
      carrier.display.println("PROXIMIDAD");
      carrier.display.setCursor(30, 130);
      carrier.display.setTextSize(2);
      carrier.display.print("Distancia: ");
      carrier.display.print(proximity);
      carrier.display.setTextSize(2);
      carrier.display.println(" mm");

      carrier.leds.setBrightness(50);
      if (proximity > 200) {
        carrier.leds.fill(carrier.leds.Color(0, 255, 0));
         playTone1(NOTE_G1, NOTE_A1);// Verde
      } else if (proximity > 120) {
        carrier.leds.fill(carrier.leds.Color(255, 255, 0));
         playTone2(NOTE_D5, NOTE_B4);// Amarillo
      } else {
        carrier.leds.fill(carrier.leds.Color(255, 0, 0)); 
         playTone3(NOTE_F2, NOTE_E2);// Rojo
      }
      carrier.leds.show();
    }

    delay(100);
  }
}

void playGestureTone(int firstNote, int secondNote) {
  carrier.Buzzer.sound(firstNote);
  delay(50);
  carrier.Buzzer.noSound();
  delay(10);
  carrier.Buzzer.sound(secondNote);
  delay(50);
  carrier.Buzzer.noSound();
}

void playTone1(int firstNote, int secondNote) {
  carrier.Buzzer.sound(firstNote);
  delay(50); 
  carrier.Buzzer.noSound();
  delay(10);
  carrier.Buzzer.sound(secondNote);
  delay(50); 
  carrier.Buzzer.noSound();
}

void playTone2(int firstNote, int secondNote) {
  carrier.Buzzer.sound(firstNote);
  delay(30); 
  carrier.Buzzer.noSound();
  delay(10);
  carrier.Buzzer.sound(secondNote);
  delay(30); 
  carrier.Buzzer.noSound();
}

void playTone3(int firstNote, int secondNote) {
  carrier.Buzzer.sound(firstNote);
  delay(150); 
  carrier.Buzzer.noSound();
  delay(10);
  carrier.Buzzer.sound(secondNote);
  delay(15); 
  carrier.Buzzer.noSound();
}
