# ClassVision v2.1 — Face attendance + camera selection + ESP32 BLE output

This release builds on the supplied browser app. It adds:
- Camera device selector and refresh button. USB/UVC cameras are selectable only when the browser exposes them.
- Student semester field, carried into records, CSV exports and JSON backups. Older records remain readable.
- ESP32 Bluetooth LE attendance output. A saved attendance event sends the single-byte command `P` to the ESP32.
- Updated setup notes and a starter ESP32 firmware sketch.

## What works now
1. Face enrollment and matching remain in the browser using the existing face-api library.
2. Student records remain in IndexedDB on the current browser/device.
3. Camera list is populated after camera permission has been granted. Choose an external webcam if it appears.
4. Once an attendance record is successfully saved, the connected ESP32 receives a BLE signal. The ESP32 flashes green, sounds the buzzer and updates the OLED.
5. CSV and JSON backup/restore continue to work. Semester is included in new student data and CSV exports.

## Camera setup
- Deploy the site to GitHub Pages (HTTPS), open it in Chrome and grant camera permission.
- Click **Refresh** beside Camera device. Select the USB/UVC camera if listed, then click **Start camera**.
- Some browsers hide device labels until permission is granted. Refresh after the first permission prompt.
- Android OTG/UVC support varies by phone and browser. If your Android browser does not list the USB camera, this website cannot force the device to expose it; a native Android USB-host camera app/bridge is a separate development step.

## ESP32 BLE setup
The browser-to-ESP32 output in this release uses **Bluetooth Low Energy (BLE), not Wi-Fi**. BLE is used because a GitHub Pages HTTPS website cannot reliably call an ESP32's plain-HTTP local IP address: browser mixed-content and local-network permission restrictions can block it.

### Parts
- ESP32 DevKit board with BLE
- SSD1306 0.96-inch I2C OLED (optional)
- Green LED + 220-ohm resistor
- Red LED + 220-ohm resistor (optional idle/error indicator)
- Active buzzer (use a transistor driver if required by the buzzer current)
- Jumper wires

### Example wiring
- OLED VCC -> 3V3; GND -> GND; SDA -> GPIO 21; SCL -> GPIO 22
- Green LED anode -> GPIO 25 through 220-ohm resistor; cathode -> GND
- Red LED anode -> GPIO 26 through 220-ohm resistor; cathode -> GND
- Active buzzer signal -> GPIO 27; GND -> GND
- Use the ESP32 board's 3.3V logic. Do not connect 5V to GPIO pins.

### Upload firmware
1. Open `esp32/ClassVision_ESP32_BLE.ino` in Arduino IDE.
2. Install **Adafruit SSD1306** and **Adafruit GFX Library** from Library Manager.
3. Select the correct ESP32 board and COM/USB port, then upload.
4. Power the ESP32. It advertises as `ClassVision-ESP32`.
5. In ClassVision open Settings -> ESP32 attendance indicator -> Connect ESP32. Select the device.
6. Start a session and scan an enrolled face. After a record is saved, the green LED, buzzer and OLED should activate.

If you do not have an OLED, the firmware still works with the LEDs and buzzer. The OLED is optional.

## Deploy / update
1. Back up existing records first: Settings -> Download JSON backup.
2. Replace the repository's `index.html`, `app.js`, `style.css`, and `README.md` with these files. Upload the `esp32` folder too.
3. Commit to GitHub and wait for GitHub Pages to publish.
4. Open the HTTPS site and hard-refresh. Do not clear browser site data unless you have a backup.

## Important limitations — read before classroom use
- **No automatic cross-device sync yet.** Browser IndexedDB is still local to each device. Use JSON backup/restore to move records between devices. True sync needs a secure backend, server-side authentication, database, access rules and deployment.
- **No native Android USB camera bridge yet.** If Android Chrome does not expose the OTG camera, a native Android USB-host implementation is required.
- **BLE availability:** Web Bluetooth requires a supported browser (typically Chrome/Chromium), HTTPS or localhost, user interaction to pair, and Bluetooth enabled. iOS Safari does not support this Web Bluetooth flow.
- Face descriptors are biometric data and are not encrypted by this prototype. Obtain informed consent, get institutional approval where required, restrict access, define retention/deletion rules, and offer a non-biometric attendance option.
- Face matching can make false matches or miss real students. The five-scan consistency check is not proof of identity. A teacher must supervise and review records.
- The teacher login remains a local browser access gate, not secure server authentication.
- The face recognition library and models load from jsDelivr; first use needs internet access.
- Do not use this prototype as an official high-stakes attendance or disciplinary system without a proper security and privacy review.

## Troubleshooting
- No camera shown: grant permission, press Refresh, and check whether the OS/browser recognizes the UVC camera.
- Camera permission fails: use the GitHub Pages HTTPS address, not a plain LAN HTTP URL.
- ESP32 not found: check firmware upload, board power, Bluetooth permission, supported Chrome, and that the device advertises as `ClassVision-ESP32`.
- ESP32 connects but does not respond: check GPIO wiring, common ground, OLED I2C wiring, and that the sketch is running.
- Recognition model fails: check internet access and reload.
- If old UI remains after updating, hard-refresh Chrome. Export a backup before clearing site data.
