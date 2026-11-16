CLASSVISION BLE + OLED TEST FIX

This ZIP is a standalone BLE/OLED transport test. It does not change or improve
face recognition or automatic biometric matching.

FILES
1. ClassVision_ESP32.ino
   Upload this sketch to the ESP32. Keep only one .ino in the Arduino sketch folder.
2. ble-oled-test.html
   Open from a secure HTTPS page in a Web Bluetooth-capable Chrome browser.

WIRING
OLED VCC -> 3V3
OLED GND -> GND
OLED SDA -> GPIO21
OLED SCL -> GPIO22
Green LED -> GPIO25 through a suitable resistor
Red LED -> GPIO26 through a suitable resistor
Buzzer signal -> GPIO27

ARDUINO LIBRARIES
- Adafruit GFX Library
- Adafruit SSD1306
ESP32 board package supplies BLE libraries.

TEST
1. Upload ClassVision_ESP32.ino.
2. Wait for OLED to show Ready / Waiting for BLE.
3. Host ble-oled-test.html on HTTPS (for example, a temporary GitHub Pages test path).
4. Open it in Chrome, tap Connect ESP32, and select ClassVision-ESP32.
5. Enter a test name and roll, then tap Send test attendance to OLED.
6. OLED should show the name, roll number and PRESENT.

This verifies the BLE packet format separately from the attendance application.
Only send records after an appropriate, non-biometric verification/approval step.
