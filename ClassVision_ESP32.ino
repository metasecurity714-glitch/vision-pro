#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define GREEN_LED 25
#define RED_LED 26
#define BUZZER_PIN 27
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

#define SERVICE_UUID "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID "6e400002-b5a3-f393-e0a9-e50e24dcca9e"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
BLEServer* server = nullptr;

String rxBuffer;
String studentName;
String rollNumber;
volatile bool connected = false;
volatile bool connectionChanged = false;
volatile bool attendanceReady = false;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void showLines(const String& a, const String& b = "", const String& c = "") {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 4); display.println(a);
  display.setCursor(0, 24); display.println(b);
  display.setCursor(0, 44); display.println(c);
  display.display();
}

void showAttendance() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0); display.println("CLASSVISION");
  display.drawLine(0, 12, 127, 12, SSD1306_WHITE);
  display.setCursor(0, 18); display.print("Name: "); display.println(studentName);
  display.setCursor(0, 32); display.print("Roll: "); display.println(rollNumber);
  display.setTextSize(2);
  display.setCursor(0, 47); display.println("PRESENT");
  display.display();
}

class ServerCB : public BLEServerCallbacks {
  void onConnect(BLEServer*) override {
    connected = true; connectionChanged = true;
  }
  void onDisconnect(BLEServer*) override {
    connected = false; connectionChanged = true;
  }
};

class RxCB : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* characteristic) override {
    // Arduino-ESP32 BLE API version returns String here.
    String value = characteristic->getValue();
    if (!value.length()) return;

    portENTER_CRITICAL(&mux);
    if (value == "START") {
      rxBuffer = "";
    } else if (value == "END") {
      int first = rxBuffer.indexOf('|');
      int second = rxBuffer.indexOf('|', first + 1);
      if (first >= 0 && second > first &&
          rxBuffer.substring(0, first) == "ATT") {
        studentName = rxBuffer.substring(first + 1, second);
        rollNumber = rxBuffer.substring(second + 1);
        attendanceReady = true;
      }
      rxBuffer = "";
    } else if (value.startsWith("D|")) {
      rxBuffer += value.substring(2);
    }
    portEXIT_CRITICAL(&mux);
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("OLED init failed");
    while (true) delay(1000);
  }
  showLines("CLASSVISION", "Starting...", "");

  BLEDevice::init("ClassVision-ESP32");
  server = BLEDevice::createServer();
  server->setCallbacks(new ServerCB());
  BLEService* service = server->createService(SERVICE_UUID);
  BLECharacteristic* rx = service->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
  );
  rx->setCallbacks(new RxCB());
  service->start();
  BLEDevice::getAdvertising()->addServiceUUID(SERVICE_UUID);
  BLEDevice::startAdvertising();

  showLines("CLASSVISION", "Ready", "Waiting for BLE");
  Serial.println("ClassVision BLE ready");
}

void loop() {
  if (connectionChanged) {
    connectionChanged = false;
    if (connected) showLines("CLASSVISION", "BLE CONNECTED", "Waiting for data");
    else {
      showLines("CLASSVISION", "Disconnected", "Waiting for BLE");
      BLEDevice::startAdvertising();
    }
  }

  if (attendanceReady) {
    portENTER_CRITICAL(&mux);
    attendanceReady = false;
    portEXIT_CRITICAL(&mux);

    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(120);
    digitalWrite(BUZZER_PIN, LOW);

    showAttendance();
    Serial.println("Attendance packet received");
    Serial.println(studentName);
    Serial.println(rollNumber);

    delay(1800);
    digitalWrite(GREEN_LED, LOW);
  }
  delay(15);
}
