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

static const char* SERVICE_UUID =
  "6e400001-b5a3-f393-e0a9-e50e24dcca9e";
static const char* RX_UUID =
  "6e400002-b5a3-f393-e0a9-e50e24dcca9e";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool oledReady = false;
bool deviceConnected = false;

portMUX_TYPE messageMux = portMUX_INITIALIZER_UNLOCKED;
char incomingMessage[160] = "";
bool messagePending = false;

void showReady() {
  if (!oledReady) return;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.println("CLASSVISION");
  display.setTextSize(2);
  display.setCursor(0, 32);
  display.println("READY");
  display.display();
}

void showAttendance(const char* name, const char* roll,
                    const char* status) {
  if (!oledReady) return;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("CLASSVISION");

  display.setCursor(0, 16);
  display.print("NAME: ");
  display.println(name);

  display.setCursor(0, 30);
  display.print("ROLL: ");
  display.println(roll);

  display.setTextSize(2);
  display.setCursor(0, 46);
  display.println(status);
  display.display();
}

void processMessage(char* message) {
  char* type = strtok(message, "|");
  char* name = strtok(NULL, "|");
  char* roll = strtok(NULL, "|");
  char* status = strtok(NULL, "|");

  if (!type || !name || !roll || !status) {
    Serial.println("Invalid message format");
    return;
  }

  if (strcmp(type, "VERIFIED") != 0 ||
      strcmp(status, "PRESENT") != 0) {
    Serial.println("Unsupported attendance message");
    return;
  }

  showAttendance(name, roll, status);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(250);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.print("Student: ");
  Serial.println(name);
  Serial.print("Roll: ");
  Serial.println(roll);

  delay(2500);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH);
  showReady();
}

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* server) override {
    deviceConnected = true;
    Serial.println("Website connected");
    showReady();
  }

  void onDisconnect(BLEServer* server) override {
    deviceConnected = false;
    Serial.println("Website disconnected");
    showReady();
    BLEDevice::startAdvertising();
  }
};

class CommandCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* characteristic) override {
    std::string value = characteristic->getValue();

    if (value.empty() || value.length() >= sizeof(incomingMessage)) {
      Serial.println("BLE message empty or too long");
      return;
    }

    portENTER_CRITICAL(&messageMux);
    memcpy(incomingMessage, value.data(), value.length());
    incomingMessage[value.length()] = '\0';
    messagePending = true;
    portEXIT_CRITICAL(&messageMux);
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

  if (oledReady) showReady();
  else Serial.println("OLED not detected. Check I2C wiring/address.");

  BLEDevice::init("ClassVision-ESP32");

  BLEServer* server = BLEDevice::createServer();
  server->setCallbacks(new ServerCallbacks());

  BLEService* service = server->createService(SERVICE_UUID);

  BLECharacteristic* rx = service->createCharacteristic(
    RX_UUID,
    BLECharacteristic::PROPERTY_WRITE |
    BLECharacteristic::PROPERTY_WRITE_NR
  );

  rx->setCallbacks(new CommandCallbacks());
  service->start();

  BLEAdvertising* advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("ClassVision ESP32 BLE ready");
  showReady();
}

void loop() {
  char localMessage[160];
  bool hasMessage = false;

  portENTER_CRITICAL(&messageMux);
  if (messagePending) {
    strncpy(localMessage, incomingMessage, sizeof(localMessage) - 1);
    localMessage[sizeof(localMessage) - 1] = '\0';
    messagePending = false;
    hasMessage = true;
  }
  portEXIT_CRITICAL(&messageMux);

  if (hasMessage) processMessage(localMessage);

  delay(10);
}
