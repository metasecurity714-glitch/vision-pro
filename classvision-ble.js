/*
 * ClassVision BLE bridge
 * Include this file after app.js or before </body>.
 * Uses the existing index.html IDs:
 * connectEsp32Btn, disconnectEsp32Btn, esp32Status, esp32Msg.
 *
 * Send only after your approved, non-biometric attendance verification step.
 */
(() => {
  'use strict';

  const SERVICE_UUID = '6e400001-b5a3-f393-e0a9-e50e24dcca9e';
  const RX_UUID = '6e400002-b5a3-f393-e0a9-e50e24dcca9e';

  let device = null;
  let characteristic = null;
  let sendQueue = Promise.resolve();

  const byId = id => document.getElementById(id);
  const setMessage = text => {
    const el = byId('esp32Msg');
    if (el) el.textContent = text;
  };
  const setStatus = text => {
    const el = byId('esp32Status');
    if (el) el.textContent = text;
  };

  async function writePacket(bytes) {
    if (!characteristic) throw new Error('ESP32 is not connected.');
    if (characteristic.writeValueWithResponse) {
      await characteristic.writeValueWithResponse(bytes);
    } else if (characteristic.writeValueWithoutResponse) {
      await characteristic.writeValueWithoutResponse(bytes);
    } else {
      await characteristic.writeValue(bytes);
    }
  }

  // Queue writes to prevent BLE packets from arriving out of order.
  function queuePacket(bytes) {
    sendQueue = sendQueue.then(() => writePacket(bytes));
    return sendQueue;
  }

  async function sendMessage(message) {
    if (!characteristic) {
      setMessage('Connect ESP32 first.');
      return false;
    }

    try {
      const encoder = new TextEncoder();
      const data = encoder.encode(message);

      await queuePacket(encoder.encode('START'));

      // ATT payload is normally 20 bytes at the default MTU.
      // "D|" uses 2 bytes, leaving 18 bytes for each data chunk.
      for (let offset = 0; offset < data.length; offset += 18) {
        const chunk = data.slice(offset, offset + 18);
        const packet = new Uint8Array(chunk.length + 2);
        packet[0] = 68; // D
        packet[1] = 124; // |
        packet.set(chunk, 2);
        await queuePacket(packet);
      }

      await queuePacket(encoder.encode('END'));
      return true;
    } catch (error) {
      setMessage('BLE send error: ' + (error.message || error));
      return false;
    }
  }

  async function connect() {
    if (!navigator.bluetooth) {
      setMessage('Web Bluetooth is unavailable. Use a supported Chrome browser over HTTPS.');
      return;
    }

    const connectButton = byId('connectEsp32Btn');
    const disconnectButton = byId('disconnectEsp32Btn');
    if (connectButton) connectButton.disabled = true;

    try {
      device = await navigator.bluetooth.requestDevice({
        filters: [{ services: [SERVICE_UUID] }]
      });

      device.addEventListener('gattserverdisconnected', () => {
        characteristic = null;
        setStatus('Disconnected');
        setMessage('ESP32 disconnected.');
        if (connectButton) connectButton.disabled = false;
        if (disconnectButton) disconnectButton.disabled = true;
      });

      const server = await device.gatt.connect();
      const service = await server.getPrimaryService(SERVICE_UUID);
      characteristic = await service.getCharacteristic(RX_UUID);

      setStatus('Connected');
      setMessage('Connected to ClassVision-ESP32.');
      if (disconnectButton) disconnectButton.disabled = false;

      await sendMessage('TEST');
    } catch (error) {
      characteristic = null;
      setStatus('Not connected');
      setMessage(error.message || 'Could not connect to ESP32.');
      if (connectButton) connectButton.disabled = false;
      if (disconnectButton) disconnectButton.disabled = true;
    }
  }

  function disconnect() {
    if (device && device.gatt && device.gatt.connected) {
      device.gatt.disconnect();
    } else {
      characteristic = null;
      setStatus('Not connected');
      if (byId('connectEsp32Btn')) byId('connectEsp32Btn').disabled = false;
      if (byId('disconnectEsp32Btn')) byId('disconnectEsp32Btn').disabled = true;
    }
  }

  async function sendAttendance(studentName, rollNumber) {
    const cleanName = String(studentName || '').replace(/[|\r\n]/g, ' ').trim().slice(0, 30);
    const cleanRoll = String(rollNumber || '').replace(/[|\r\n]/g, ' ').trim().slice(0, 15);

    if (!cleanName || !cleanRoll) {
      setMessage('Student name and roll number are required.');
      return false;
    }

    const sent = await sendMessage('ATT|' + cleanName + '|' + cleanRoll);
    if (sent) setMessage('Attendance details sent to OLED.');
    return sent;
  }

  function init() {
    const connectButton = byId('connectEsp32Btn');
    const disconnectButton = byId('disconnectEsp32Btn');
    if (connectButton) connectButton.addEventListener('click', connect);
    if (disconnectButton) disconnectButton.addEventListener('click', disconnect);
    if (disconnectButton) disconnectButton.disabled = true;
  }

  window.ClassVisionBLE = {
    connect,
    disconnect,
    sendAttendance,
    sendTest: () => sendMessage('TEST')
  };

  if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', init, { once: true });
  } else {
    init();
  }
})();
