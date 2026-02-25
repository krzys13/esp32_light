# Python MQTT Backend Client

This directory contains the Python backend client responsible for:

- Connecting securely to the MQTT broker (TLS)
- Subscribing to sensor data topics
- Parsing incoming JSON messages
- Writing data to InfluxDB

The client acts as a bridge between the MQTT broker and the database.

# ⚙ Configuration Required

Before running the client, you must configure it.

## 1️⃣ Create Configuration File

## 2️⃣ Add TLS Certificate