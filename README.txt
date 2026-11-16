CLASSVISION ESP32 + WEBSITE BLE FILES

Files:
1. ClassVision_ESP32.ino
   Upload this sketch to the ESP32 DevKit V1 using Arduino IDE.

2. classvision-ble.js
   Add this file to the root of the vision-pro GitHub repository.
   In index.html, add this script tag after app.js:
   <script src="classvision-ble.js" defer></script>

Important:
- Remove the old BLE connect/disconnect handlers from app.js to avoid duplicate click handlers.
- Keep the existing HTML IDs:
  connectEsp32Btn, disconnectEsp32Btn, esp32Status, esp32Msg.
- After an approved non-biometric attendance verification, call:
  window.ClassVisionBLE.sendAttendance(studentName, rollNumber);
- Do not connect this to face-template matching or automatic face identification.
- The ESP32 and website use the same service/characteristic UUIDs and packet protocol.

Arduino IDE libraries:
- Adafruit GFX Library
- Adafruit SSD1306
The ESP32 BLE libraries are included with the ESP32 Arduino board package.

Wiring:
OLED VCC -> 3V3
OLED GND -> GND
OLED SDA -> GPIO 21
OLED SCL -> GPIO 22
Green LED -> GPIO 25 (through suitable resistor)
Red LED -> GPIO 26 (through suitable resistor)
Buzzer signal -> GPIO 27
