# ==========================================================
# IoT Light Monitoring System - Configuration Template
# Copy this file to config.py and fill in real credentials.
# ==========================================================

from pathlib import Path
CONFIG_DIR = Path(__file__).resolve().parent

BASE_DIR = CONFIG_DIR.parent


# ---------------- MQTT CONFIGURATION ----------------

MQTT_HOST = "192.168.1.75"
MQTT_PORT = 8883

# Backend client identity
MQTT_CLIENT_ID = "mqtt_server_client" ## backend mqtt client name

# Authentication
MQTT_PASS = "pollub" # backend mqtt client password

# TLS Certificate (CA used by broker)
MQTT_CA_CRT = str((BASE_DIR / "certs" / "ca.crt").resolve()) # paste CA cert into ca.crt file

# Topic structure (scalable IoT hierarchy)
MQTT_BASE_TOPIC = "sonda" ## ESP32 mqtt client name
MQTT_DATA_TOPIC = f"{MQTT_BASE_TOPIC}/data"



# ---------------- INFLUXDB CONFIGURATION ----------------

INFLUX_URL = "http://192.168.1.75:8086"
INFLUX_TOKEN = "sDUcbjMsy3CWWzYCzcWGFx8WL2FjdZGIGycHLx3oGLqB1Jmc5RjU9J1pygYkYAt-mbatgqEnwQhjAbEdG0iaug=="
INFLUX_ORG = "pollub"
INFLUX_BUCKET = "sonda_data"





