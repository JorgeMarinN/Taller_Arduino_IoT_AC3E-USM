#include <Arduino_MKRIoTCarrier.h>

MKRIoTCarrier carrier;
bool isSubMenu = false;
int currentSubMenu = -1;
unsigned long buttonPressStart[5] = {0, 0, 0, 0, 0};
unsigned long lastUpdate = 0; // Para actualizar la pantalla periódicamente

void setup() {
  if (!carrier.begin()) {
    while (1); // Detener si el Carrier no se inicializa correctamente.
  }
  carrier.display.fillScreen(ST77XX_BLACK);
  displayMainMenu();

  if (!carrier.IMUmodule.begin()) {
    while (1); // Detener si el módulo IMU no se inicializa correctamente.
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
    if (millis() - lastUpdate >= 2000) { // Actualizar cada 1 segundo
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
  carrier.display.fillScreen(ST77XX_YELLOW);
  carrier.display.setTextColor(ST77XX_CYAN);
  carrier.display.setTextSize(4);
  carrier.display.setCursor(70, 50);
  carrier.display.println("MENU:");

  carrier.display.setTextSize(2);
  carrier.display.setCursor(40, 90);
  carrier.display.setTextColor(ST77XX_ORANGE);
  carrier.display.println("Temperatura: 00");
  carrier.display.setCursor(40, 110);
  carrier.display.setTextColor(ST77XX_BLACK);
  carrier.display.println("Humedad: 01 ");
  carrier.display.setCursor(40, 130);
  carrier.display.setTextColor(ST77XX_MAGENTA);
  carrier.display.println("Presion: 02 ");
  carrier.display.setCursor(40, 150);
  carrier.display.setTextColor(ST77XX_BLUE);
  carrier.display.println("Gases: 03 ");
  carrier.display.setTextColor(ST77XX_RED);
  carrier.display.setCursor(40, 170);
  carrier.display.println("Giroscopio: 04 ");
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
  carrier.display.fillScreen(ST77XX_ORANGE);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(50, 100);
  carrier.display.println("TEMPERATURA");
}

void displayHumiditySubMenu() {
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextColor(ST77XX_YELLOW);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(70, 100);
  carrier.display.println("HUMEDAD");
}

void displayPressureSubMenu() {
  carrier.display.fillScreen(ST77XX_MAGENTA);
  carrier.display.setTextColor(ST77XX_CYAN);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(70, 100);
  carrier.display.println("PRESION");
}

void displayGasSubMenu() {
  carrier.display.fillScreen(ST77XX_BLUE);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(70, 100);
  carrier.display.println("GASES");
}

void displayGyroscopeSubMenu() {
  carrier.display.fillScreen(ST77XX_RED);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(70, 100);
  carrier.display.println("GIROSCOPIO");
}

void updateTemperatureDisplay() {
  float temperatureC = carrier.Env.readTemperature();
  float temperatureF = (temperatureC * 9 / 5) + 32;
  float temperatureK = temperatureC + 273.15;

  carrier.display.fillScreen(ST77XX_ORANGE);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(50, 70);
  carrier.display.println("Temperatura:");

  carrier.display.setTextSize(1);
  carrier.display.setCursor(60, 110);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);
  carrier.display.print(temperatureC, 2);
  carrier.display.println(" C");

  carrier.display.setCursor(60, 130);
  carrier.display.print(temperatureF, 2);
  carrier.display.println(" F");

  carrier.display.setCursor(60, 150);
  carrier.display.print(temperatureK, 2);
  carrier.display.println(" K");
}

void updateHumidityDisplay() {
  float humidity = carrier.Env.readHumidity();

  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextColor(ST77XX_YELLOW);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(70, 70);
  carrier.display.println("Humedad:");

  carrier.display.setTextSize(2);
  carrier.display.setCursor(70, 100);
  carrier.display.print(humidity, 2);
  carrier.display.println(" %");
}

void updatePressureDisplay() {
  float pressure = carrier.Pressure.readPressure() / 100.0;

  carrier.display.fillScreen(ST77XX_MAGENTA);
  carrier.display.setTextColor(ST77XX_CYAN);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(70, 70);
  carrier.display.println("Presion:");

  carrier.display.setTextSize(2);
  carrier.display.setCursor(70, 100);
  carrier.display.print(pressure, 2);
  carrier.display.println(" hPa");
}

void updateGasDisplay() {
  float gasResistor = carrier.AirQuality.readGasResistor();
  float volatileOrganicCompounds = carrier.AirQuality.readVOC();
  float co2 = carrier.AirQuality.readCO2();

  carrier.display.fillScreen(ST77XX_BLUE);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(80, 70);
  carrier.display.println("Gases:");

  carrier.display.setTextSize(2);
  carrier.display.setCursor(20, 100);
  carrier.display.print("Res: ");
  carrier.display.print(gasResistor, 2);
  carrier.display.println(" ohms");

  carrier.display.setCursor(25, 120);
  carrier.display.print("VOC: ");
  carrier.display.print(volatileOrganicCompounds, 2);
  carrier.display.println(" ppm");

  carrier.display.setCursor(25, 140);
  carrier.display.print("CO2: ");
  carrier.display.print(co2, 2);
  carrier.display.println(" ppm");
}

void updateGyroscopeDisplay() {
  float x, y, z;

  if (carrier.IMUmodule.accelerationAvailable()) {
    carrier.IMUmodule.readAcceleration(x, y, z);

    carrier.display.fillScreen(ST77XX_RED);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.setTextSize(2);
    carrier.display.setCursor(70, 70);
    carrier.display.println("Giroscopio:");

    carrier.display.setTextSize(2);
    carrier.display.setCursor(70, 100);
    carrier.display.print("x: ");
    carrier.display.println(x, 2);

    carrier.display.setCursor(70, 120);
    carrier.display.print("y: ");
    carrier.display.println(y, 2);

    carrier.display.setCursor(70, 140);
    carrier.display.print("z: ");
    carrier.display.println(z, 2);
  }
}
