#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define GREEN_LED 25
#define RED_LED 26
#define BUZZER_PIN 27
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

static const char* SERVICE_UUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e";
static const char* RX_UUID      = "6e400002-b5a3-f393-e0a9-e50e24dcca9e";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool oledReady = false;
bool deviceConnected = false;

void showMessage(const char* line1, const char* line2) {
  if (!oledReady) return;
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 12);
  display.println(line1);
  display.setTextSize(2);
  display.setCursor(0, 32);
  display.println(line2);
  display.display();
}

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* server) override {
    deviceConnected = true;
    showMessage("ClassVision", "CONNECTED");
  }
  void onDisconnect(BLEServer* server) override {
    deviceConnected = false;
    showMessage("ClassVision", "READY");
    BLEDevice::startAdvertising();
  }
};

class CommandCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* characteristic) override {
    String command = characteristic->getValue().c_str();
    command.trim();
    if (command == "P") {
      digitalWrite(RED_LED, LOW);
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
      showMessage("ATTENDANCE SAVED", "PRESENT");
      delay(350);
      digitalWrite(BUZZER_PIN, LOW);
      delay(900);
      digitalWrite(GREEN_LED, LOW);
      showMessage("ClassVision", "READY");
    }
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH);
  digitalWrite(BUZZER_PIN, LOW);

  Wire.begin(21, 22);
  oledReady = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  showMessage("ClassVision", "STARTING");

  BLEDevice::init("ClassVision-ESP32");
  BLEServer* server = BLEDevice::createServer();
  server->setCallbacks(new ServerCallbacks());
  BLEService* service = server->createService(SERVICE_UUID);
  BLECharacteristic* rx = service->createCharacteristic(
    RX_UUID, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
  );
  rx->setCallbacks(new CommandCallbacks());
  service->start();

  BLEAdvertising* advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  showMessage("ClassVision", "READY");
  Serial.println("ClassVision ESP32 BLE ready.");
}

void loop() {
  delay(20);
}
