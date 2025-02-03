#include <Arduino_MKRIoTCarrier.h>
#include <WiFiNINA.h>
#include <ArduinoBLE.h>

MKRIoTCarrier carrier;
int buttonPressed = -1; 
String ssidList[6];   
int rssiList[6];      
int channelList[6];   
int networkCount = 0;  
int currentSelection = 0; 
BLEDevice devices[20]; // Aumentado el límite a 20 para más dispositivos
int deviceCount = 0;
BLEService genericService("180A");
BLEStringCharacteristic readWriteCharacteristic("2A29", BLERead | BLEWrite, 20);
bool bleConnected = false;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  if (!carrier.begin()) {
    Serial.println("ERROR CARRIER");
    while (1);
  }

  if (!BLE.begin()) {
    Serial.println("ERROR INICIANDO BLE");
    while (1);
  }

  Serial.println("................................................................................................................................................................................."); 
  showMenu();
}

void loop() {
  carrier.Buttons.update();
  if (carrier.Buttons.onTouchDown(TOUCH1)) {
    buttonPressed = 1; 
  } else if (carrier.Buttons.onTouchDown(TOUCH3)) {
    buttonPressed = 3; 
  } else if (carrier.Buttons.onTouchDown(TOUCH2)) {
    buttonPressed = -1;
    showMenu();
    return;
  }

  if (buttonPressed == 1) {
    buttonPressed = -1;
    handleWiFi();
  } else if (buttonPressed == 3) {
    buttonPressed = -1;
    handleBLE();
  }
}

void showMenu() {
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(90, 40);
  carrier.display.println("MENU");

  carrier.display.setCursor(55, 100);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.println("01. WI-FI");

  carrier.display.setCursor(50, 140);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.println("03. BLE");
}

void handleWiFi() {
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setCursor(40, 100);
  carrier.display.setTextSize(3);
  carrier.display.setTextColor(ST77XX_GREEN);
  carrier.display.println("Escaneando");
  
  networkCount = min(WiFi.scanNetworks(), 6);
  if (networkCount == 0) {
    Serial.println("...");
    carrier.display.println("NO SE ENCONTRO NADA");
    delay(2000);
    showMenu();
    return;
  }

  for (int i = 0; i < networkCount && i < 6; i++) {
    ssidList[i] = WiFi.SSID(i);
    rssiList[i] = WiFi.RSSI(i);
    channelList[i] = WiFi.channel(i);
  }

  Serial.println("ENCONTRADAS:");
  for (int i = 0; i < networkCount; i++) {
    Serial.print(i + 1);
    Serial.print(") SSID: ");
    Serial.print(ssidList[i]);
    Serial.print(" RSSI: ");
    Serial.print(rssiList[i]);
    Serial.print(" CH: ");
    Serial.println(channelList[i]);
  }

  selectNetwork();
}

void selectNetwork() {
  currentSelection = 0;
  bool networkSelected = false;

  while (!networkSelected) {
    carrier.display.fillScreen(ST77XX_BLACK);
    carrier.display.setTextSize(1);
    for (int i = 0; i < networkCount && i < 6; i++) {
      carrier.display.setCursor(40, 40 + i * 30);
      if (i == currentSelection) {
        carrier.display.setTextColor(ST77XX_GREEN);
      } else {
        carrier.display.setTextColor(ST77XX_WHITE);
      }
      carrier.display.println(ssidList[i]);
    }

    carrier.Buttons.update();

    if (carrier.Buttons.onTouchDown(TOUCH3)) {
      currentSelection = (currentSelection + 1) % networkCount;
    } else if (carrier.Buttons.onTouchDown(TOUCH1)) {
      currentSelection = (currentSelection - 1 + networkCount) % networkCount;
    } else if (carrier.Buttons.onTouchDown(TOUCH0)) {
      networkSelected = true;
    }

    if (Serial.available() > 0) {
      String input = Serial.readStringUntil('\n');
      if (input.toInt() > 0 && input.toInt() <= networkCount) {
        currentSelection = input.toInt() - 1;
        networkSelected = true;
      } else if (input.equalsIgnoreCase("m")) {
        showMenu();
        return;
      }
    }
  }

  connectToNetwork(currentSelection);
}

void connectToNetwork(int index) {
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextSize(3);
  carrier.display.setCursor(70, 60);
  carrier.display.setTextColor(ST77XX_GREEN);
  carrier.display.println("CLAVE: ");
  Serial.print("CONTRASEÑA: ");
  Serial.print(ssidList[index]);
  Serial.print(": ");

  while (!Serial.available());
  String password = Serial.readStringUntil('\n');
  carrier.display.setTextSize(3);
  carrier.display.setCursor(40, 120);
  carrier.display.println("Conectando");
  Serial.println("Conectando");
  WiFi.begin(ssidList[index].c_str(), password.c_str());

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 6) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("CONECTADO");
    Serial.print("Direccion IP: ");
    Serial.println(WiFi.localIP());

    carrier.display.fillScreen(ST77XX_BLACK);
    carrier.display.setTextSize(2);
    carrier.display.setCursor(70, 80);
    carrier.display.setTextColor(ST77XX_GREEN);
    carrier.display.println("CONECTADO");
    carrier.display.setCursor(20, 120);
    carrier.display.setTextColor(ST77XX_GREEN);
    carrier.display.print("IP: ");
    carrier.display.println(WiFi.localIP());

    while (true) {
      if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        if (input.equalsIgnoreCase("m")) {
          WiFi.disconnect();
          Serial.println("................................................................................................................................................................................."); 
          showMenu();
          return;
        }
      }
    }
  } else {
    Serial.println("FALLO");
    carrier.display.fillScreen(ST77XX_RED);
    carrier.display.setTextSize(3);
    carrier.display.setCursor(80, 100);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.println("FALLO");
    delay(2000);
    Serial.println("................................................................................................................................................................................."); 
    showMenu();
  }
}

void handleBLE() {
  BLE.setLocalName("CARRIER");
  BLE.setAdvertisedService(genericService);
  genericService.addCharacteristic(readWriteCharacteristic);
  BLE.addService(genericService);
  BLE.advertise();

  Serial.println("Listo para conectarse");
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setCursor(60, 100);
  carrier.display.setTextSize(2);
  carrier.display.setTextColor(ST77XX_GREEN);
  carrier.display.println("Disponible");

  while (true) {
    BLEDevice central = BLE.central();

    if (central) {
      bleConnected = true;
      Serial.print("CONECTADO A: ");
      Serial.println(central.address());
      carrier.display.fillScreen(ST77XX_BLACK);
      carrier.display.setCursor(60, 100);
      carrier.display.setTextSize(2);
      carrier.display.setTextColor(ST77XX_GREEN);
      carrier.display.println("CONECTADO A: ");
      carrier.display.setCursor(20, 140);
      carrier.display.println(central.address());

      while (central.connected()) {
        if (Serial.available()) {
          String input = Serial.readStringUntil('\n');
          if (input.equalsIgnoreCase("m")) {
            BLE.stopAdvertise();
            BLE.disconnect();
            bleConnected = false;
            showMenu();
            return;
          }
        }
      }

      Serial.println("DESCONECTADO");
      carrier.display.fillScreen(ST77XX_RED);
      carrier.display.setCursor(50, 100);
      carrier.display.setTextSize(2);
      carrier.display.setTextColor(ST77XX_WHITE);
      carrier.display.println("DESCONECTADO");
      delay(2000);
      showMenu();
      return;
    }
  }
}
