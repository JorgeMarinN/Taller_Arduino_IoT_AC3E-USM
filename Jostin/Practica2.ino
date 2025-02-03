#include <Arduino_MKRIoTCarrier.h>

MKRIoTCarrier carrier;
bool isSubMenu = false;
int currentSubMenu = -1;
unsigned long buttonPressStart[5] = {0, 0, 0, 0, 0};
unsigned long lastUpdate = 0; 

void setup() {
  if (!carrier.begin()) {
    while (1); 
  }
  carrier.display.fillScreen(ST77XX_BLACK);
  displayMainMenu();

  if (!carrier.IMUmodule.begin()) {
    while (1); 
  }
}

void loop() {
  carrier.Buttons.update();

  for (int i = 0; i < 5; i++) {
    if (carrier.Buttons.getTouch((touchButtons)i)) {
      if (buttonPressStart[i] == 0) {
        buttonPressStart[i] = millis();
      } else if (millis() - buttonPressStart[i] >= 500) {
        buttonPressStart[i] = 0;

        if (isSubMenu && i != currentSubMenu) {
          displayMainMenu();
          isSubMenu = false;
          currentSubMenu = -1;
        } else if (!isSubMenu) {
          handleSubMenu(i);
        }
      }
    } else {
      buttonPressStart[i] = 0;
    }
  }

  if (isSubMenu) {
    if (millis() - lastUpdate >= 1000) { 
      switch (currentSubMenu) {
        case 0:
          updateTemperatureDisplay();
          break;
        case 1:
          updateHumidityDisplay();
          break;
        case 2:
          updatePressureDisplay();
          break;
        case 3:
          updateGasDisplay();
          break;
        case 4:
          updateGyroscopeDisplay();
          break;
        default:
          break;
      }
      lastUpdate = millis();
    }
  }
}

void displayMainMenu() {
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(100, 10);
  carrier.display.println("MENU:");

  carrier.display.setTextSize(1);
  carrier.display.setCursor(50, 50);
  carrier.display.println("00: Temperatura");
  carrier.display.setCursor(50, 70);
  carrier.display.println("01: Humedad");
  carrier.display.setCursor(50, 90);
  carrier.display.println("02: Presion");
  carrier.display.setCursor(50, 110);
  carrier.display.println("03: Gases");
  carrier.display.setCursor(50, 130);
  carrier.display.println("04: Giroscopio");
}

void handleSubMenu(int button) {
  switch (button) {
    case 0:
      displayTemperatureSubMenu();
      currentSubMenu = 0;
      break;
    case 1:
      displayHumiditySubMenu();
      currentSubMenu = 1;
      break;
    case 2:
      displayPressureSubMenu();
      currentSubMenu = 2;
      break;
    case 3:
      displayGasSubMenu();
      currentSubMenu = 3;
      break;
    case 4:
      displayGyroscopeSubMenu();
      currentSubMenu = 4;
      break;
    default:
      break;
  }
  isSubMenu = true;
}

void displayTemperatureSubMenu() {
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(50, 30);
  carrier.display.println("Temperatura:");
}

void displayHumiditySubMenu() {
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(80, 20);
  carrier.display.println("Humedad:");
}

void displayPressureSubMenu() {
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(75, 20);
  carrier.display.println("Presion:");
}

void displayGasSubMenu() {
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(90, 20);
  carrier.display.println("Gases:");
}

void displayGyroscopeSubMenu() {
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(55, 25);
  carrier.display.println("Giroscopio:");
}

void updateTemperatureDisplay() {
  float temperatureC = carrier.Env.readTemperature();
  float temperatureF = (temperatureC * 9 / 5) + 32;
  float temperatureK = temperatureC + 273.15;

  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(50, 30);
  carrier.display.println("Temperatura:");

  carrier.display.setTextSize(3);
  carrier.display.setCursor(60, 70);
  carrier.display.print(temperatureC, 2);
  carrier.display.println(" C");

  carrier.display.setCursor(60, 110);
  carrier.display.print(temperatureF, 2);
  carrier.display.println(" F");

  carrier.display.setCursor(60, 150);
  carrier.display.print(temperatureK, 2);
  carrier.display.println(" K");
}

void updateHumidityDisplay() {
  float humidity = carrier.Env.readHumidity();

  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(80, 20);
  carrier.display.println("Humedad:");

  carrier.display.setTextSize(3);
  carrier.display.setCursor(60, 110);
  carrier.display.print(humidity, 2);
  carrier.display.println(" %");
}

void updatePressureDisplay() {
  float pressure = carrier.Pressure.readPressure() / 100.0;

  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(75, 20);
  carrier.display.println("Presion:");

  carrier.display.setTextSize(3);
  carrier.display.setCursor(55, 110);
  carrier.display.print(pressure, 2);
  carrier.display.println(" hPa");
}

void updateGasDisplay() {
  float gasResistor = carrier.AirQuality.readGasResistor();
  float volatileOrganicCompounds = carrier.AirQuality.readVOC();
  float co2 = carrier.AirQuality.readCO2();

  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(90, 20);
  carrier.display.println("Gases:");

  carrier.display.setTextSize(2);
  carrier.display.setCursor(30, 90);
  carrier.display.print("R: ");
  carrier.display.print(gasResistor, 2);
  carrier.display.println(" ohms");

  carrier.display.setCursor(30, 120);
  carrier.display.print("VOC: ");
  carrier.display.print(volatileOrganicCompounds, 2);
  carrier.display.println(" ppm");

  carrier.display.setCursor(30, 150);
  carrier.display.print("CO2: ");
  carrier.display.print(co2, 2);
  carrier.display.println(" ppm");
}

void updateGyroscopeDisplay() {
  float x, y, z;

  if (carrier.IMUmodule.accelerationAvailable()) {
    carrier.IMUmodule.readAcceleration(x, y, z);

    carrier.display.fillScreen(ST77XX_BLACK);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.setTextSize(2);
    carrier.display.setCursor(55, 25);
    carrier.display.println("Giroscopio:");

    carrier.display.setTextSize(3);
    carrier.display.setCursor(50, 90);
    carrier.display.print("x: ");
    carrier.display.println(x, 2);

    carrier.display.setCursor(50, 120);
    carrier.display.print("y: ");
    carrier.display.println(y, 2);

    carrier.display.setCursor(50, 150);
    carrier.display.print("z: ");
    carrier.display.println(z, 2);
  }
}
