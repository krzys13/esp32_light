# ESP32 IoT Light Monitoring System

A complete IoT system based on ESP32, MQTT (TLS), Python backend client, and InfluxDB.

The ESP32 reads light intensity from a VEML7700 sensor and publishes data securely via MQTT.
A Python client subscribes to the MQTT topic and stores the data in InfluxDB.

---

## 🏗 System Architecture

ESP32 (VEML7700)  
→ MQTT Broker (TLS, port 8883)  
→ Python MQTT Client  
→ InfluxDB  

---

## 📁 Project Structure

ESP32-Projekt/
│
├── esp_32_firmware/ # ESP-IDF firmware for ESP32
│
├── MQTT_CLIENT_PY/ # Python MQTT → InfluxDB client
│ ├── mqtt_client.py
│ ├── config/
│ │ ├── config_example.py
│ │ └── README.md
│ ├── certs/
│ │ └── README.md
│ └── README.md
│
└── README.md
# 🔌 ESP32 Firmware

## Features

- Reads lux value from VEML7700 (I2C)
- Publishes JSON payload over MQTT
- Secure TLS connection (port 8883)
- QoS 2 support

## Example MQTT Payload

```json
{
  "lux": 123.45,
  "time": 1710000000
}