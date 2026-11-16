/* ClassVision BLE OLED + LED + buzzer receiver
   Board: ESP32 DevKit V1
   OLED: SSD1306 128x64 I2C at 0x3C
   OLED SDA=21, SCL=22; Green LED=25; Red LED=26; buzzer=27
   Libraries: Adafruit GFX, Adafruit SSD1306
   BLE service/RX UUIDs match the ClassVision website.
*/
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
#define OLED_ADDR 0x3C

static const char* SERVICE_UUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e";
static const char* RX_UUID      = "6e400002-b5a3-f393-e0a9-e50e24dcca9e";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool oledReady = false;
volatile bool deviceConnected = false;

portMUX_TYPE commandMux = portMUX_INITIALIZER_UNLOCKED;
char queuedMessage[256] = {0};
volatile bool commandPending = false;

void showMessage(const String& line1, const String& line2, const String& line3 = "") {
  if (!oledReady) return;
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 5);
  display.println(line1.substring(0, 21));
  display.drawLine(0, 17, 127, 17, SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(0, 23);
  display.println(line2.substring(0, 10));
  display.setTextSize(1);
  if (line3.length()) {
    display.setCursor(0, 51);
    display.println(line3.substring(0, 21));
  }
  display.display();
}

String getField(const String& data, int index) {
  int start = 0;
  for (int i = 0; i < index; i++) {
    int p = data.indexOf('|', start);
    if (p < 0) return "";
    start = p + 1;
  }
  int end = data.indexOf('|', start);
  return end < 0 ? data.substring(start) : data.substring(start, end);
}

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer*) override {
    deviceConnected = true;
    showMessage("ClassVision", "LINKED", "Phone connected");
  }
  void onDisconnect(BLEServer*) override {
    deviceConnected = false;
    BLEDevice::startAdvertising();
  }
};

class CommandCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* characteristic) override {
    std::string raw = characteristic->getValue();
    char message[256] = {0};
    size_t out = 0;
    for (char ch : raw) {
      if (ch >= 32 && ch <= 126 && out < sizeof(message) - 1) message[out++] = ch;
    }
    message[out] = '\0';
    portENTER_CRITICAL(&commandMux);
    strncpy(queuedMessage, message, sizeof(queuedMessage) - 1);
    queuedMessage[sizeof(queuedMessage) - 1] = '\0';
    commandPending = true;
    portEXIT_CRITICAL(&commandMux);
  }
};

void processMessage(const String& message) {
  String status = getField(message, 0);
  String name = getField(message, 1);
  String roll = getField(message, 2);
  String detail = getField(message, 3);

  if (status == "READY") {
    showMessage("ClassVision", "READY", "Bluetooth connected");
  } else if (status == "TEST") {
    showMessage("ClassVision", "BLE OK", "Test message received");
  } else if (status == "VERIFIED") {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    showMessage("ATTENDANCE SAVED", "PRESENT", name + " / " + roll);
    delay(250);
    digitalWrite(BUZZER_PIN, LOW);
    delay(1200);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    showMessage("ClassVision", "READY", "Awaiting next scan");
  } else if (status == "DUPLICATE") {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    showMessage("DUPLICATE BLOCKED", "ALREADY", name + " / " + roll);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(120);
    digitalWrite(BUZZER_PIN, LOW);
  } else if (status == "NOT_VERIFIED") {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    showMessage("NOT VERIFIED", "NOT FOUND", roll);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(350);
    digitalWrite(BUZZER_PIN, LOW);
  } else {
    showMessage("ClassVision", "MESSAGE", status);
  }
  Serial.println(message);
}

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
  if (!oledReady) Serial.println("OLED init failed: check wiring/address");
  showMessage("ClassVision", "STARTING", "Initialising BLE");

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

  showMessage("ClassVision", "READY", "BLE advertising");
  Serial.println("ClassVision ESP32 BLE ready: ClassVision-ESP32");
}

void loop() {
  char messageBuffer[256] = {0};
  bool haveMessage = false;
  portENTER_CRITICAL(&commandMux);
  if (commandPending) {
    strncpy(messageBuffer, queuedMessage, sizeof(messageBuffer) - 1);
    messageBuffer[sizeof(messageBuffer) - 1] = '\0';
    commandPending = false;
    haveMessage = true;
  }
  portEXIT_CRITICAL(&commandMux);
  if (haveMessage) processMessage(String(messageBuffer));
  delay(15);
}
