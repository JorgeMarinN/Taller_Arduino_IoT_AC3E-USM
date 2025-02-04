#include <Arduino_MKRIoTCarrier.h>
MKRIoTCarrier carrier;

const int MOISTURE_PIN = A0; 
const int PIR_PIN      = A6; 

int currentState = 0;
int oldState = -1;
String serialInput = "";

unsigned long lastMoistureReadTime = 0;      
const unsigned long MOISTURE_INTERVAL = 3000; 
int dryValue = 800;  
int wetValue = 690;  

unsigned long lastPirReadTime = 0; 
const unsigned long PIR_INTERVAL = 2000; 

void setup() {
  Serial.begin(9600);
  while (!Serial);

  CARRIER_CASE = false; 
  if (!carrier.begin()) {
    Serial.println("CARRIER ERROR");
    while (1);
  }

  pinMode(PIR_PIN, INPUT);
}

void loop() {
  carrier.Buttons.update();
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c != '\n') {
      serialInput += c;
    } else {
      handleSerialCommand(serialInput);
      serialInput = "";
    }
  }

  if (currentState == 0) {
    if (carrier.Buttons.onTouchDown(TOUCH1)) {
      currentState = 1;
    }
    if (carrier.Buttons.onTouchDown(TOUCH2)) {
      currentState = 2; 
    }
  } else {
    if (carrier.Buttons.onTouchDown(TOUCH0)) {
      currentState = 0;
    }
  }

  if (currentState == 1) {
    if (millis() - lastMoistureReadTime >= MOISTURE_INTERVAL) {
      lastMoistureReadTime = millis();
      leerHumedad();
    }
  }
  else if (currentState == 2) {
    if (millis() - lastPirReadTime >= PIR_INTERVAL) {
      lastPirReadTime = millis();
      leerPIR();
    }
  }

  if (oldState != currentState) {
    oldState = currentState;
    showState();
  }
}

void handleSerialCommand(String input) {
  if (currentState == 0) {
    // Menú principal
    if (input == "1") {
      currentState = 1; 
    } else if (input == "3") {
      currentState = 2; 
    }
  } 
  else if (currentState == 1) {
    if (input == "r") {
      currentState = 0;
    }
    else if (input == "c") {
      dryValue = analogRead(MOISTURE_PIN);
      Serial.print("Calibracion de suelo seco completada. dryValue = ");
      Serial.println(dryValue);
    }
    else if (input == "w") {
      wetValue = analogRead(MOISTURE_PIN);
      Serial.print("Calibracion de suelo humedo completada. wetValue = ");
      Serial.println(wetValue);
    }
  } 
  else if (currentState == 2) {
    if (input == "r") {
      currentState = 0;
    }
  }
}

void showState() {
  carrier.display.fillScreen(ST77XX_MAGENTA);
  carrier.display.setTextColor(ST77XX_BLACK);
  carrier.display.setCursor(0, 0);
  carrier.display.setTextSize(3);

  if (currentState == 0) {
    // Menu principal
    Serial.println("\nMENU PRINCIPAL:");
    Serial.println("1= Sensor de Humedad");
    Serial.println("2= Sensor PIR");
    Serial.println("Envía 'r' para regresar al menú principal.");

    carrier.display.setTextSize(4);
    carrier.display.setCursor(70, 20);
    carrier.display.println("MENU");
    
    carrier.display.setTextSize(3);
    carrier.display.setCursor(40, 70);
    carrier.display.println("PRINCIPAL");
    
    carrier.display.setTextSize(2);
    carrier.display.setCursor(40, 110);
    carrier.display.println("1 = Humedad");
    
    carrier.display.setCursor(40, 150);
    carrier.display.println("2 = PIR");
    
    carrier.display.setTextSize(2);
    carrier.display.setCursor(40, 190);
    carrier.display.println("'r' -> Menu");
    
  } 
  else if (currentState == 1) {
    carrier.display.setCursor(60, 40);
    carrier.display.println("HUMEDAD");
    Serial.println("HUMEDAD");
  } 
  else if (currentState == 2) {
    carrier.display.setCursor(95, 40);
    carrier.display.println("PIR");
    Serial.println("PIR");
  }
}

void leerHumedad() {
  int sensorValue = analogRead(MOISTURE_PIN);
  int humedadPercent = map(sensorValue, dryValue, wetValue, 0, 100);

  if (humedadPercent < 0)   humedadPercent = 0;
  if (humedadPercent > 100) humedadPercent = 100;

  String estadoSuelo = (humedadPercent > 50) ? "Humedo" : "Seco";

  Serial.print("   Valor sensor: ");
  Serial.print(sensorValue);
  Serial.print("   Humedad: ");
  Serial.print(humedadPercent);
  Serial.print("   Estado: ");
  Serial.println(estadoSuelo);

  carrier.display.fillScreen(ST77XX_GREEN);
  carrier.display.setCursor(60, 40);
  carrier.display.setTextSize(3);
  carrier.display.println("HUMEDAD");

carrier.display.setTextSize(2);
  carrier.display.setCursor(40, 90);
  carrier.display.print("Hum: ");
  carrier.display.print(humedadPercent);
  carrier.display.println("%");

  carrier.display.setCursor(40, 120);
  carrier.display.print("Estado: ");
  carrier.display.println(estadoSuelo);
}

void leerPIR() {
  const int numReadings = 5;
  const int threshold = 2; 
  int countHigh = 0;

  for (int i = 0; i < numReadings; i++) {
    if (digitalRead(PIR_PIN) == HIGH) {
      countHigh++;
    }
    delay(50);
  }

  if (countHigh >= threshold) {
    carrier.display.fillScreen(ST77XX_RED);
    carrier.display.setCursor(95, 30);
    carrier.display.setTextSize(3);
    carrier.display.println("PIR");

    carrier.display.fillScreen(ST77XX_BLUE);
    carrier.display.setTextSize(2);
    carrier.display.setCursor(65, 110);
    carrier.display.println("MOVIMIENTO");
    Serial.println("Hay movimiento");
  } else {
    carrier.display.fillScreen(ST77XX_YELLOW);
    carrier.display.setCursor(95, 30);
    carrier.display.setTextSize(3);
    carrier.display.println("PIR");

    carrier.display.setTextSize(2);
    carrier.display.setCursor(35, 100);
    carrier.display.println("SIN MOVIMIENTO");
    Serial.println("No hay movimiento");
  }
}
