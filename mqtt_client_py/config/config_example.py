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
MQTT_CLIENT_ID = "iot_backend_client" ## backend mqtt client name

# Authentication
MQTT_PASS = "iot_backend_client_password" # backend mqtt client password

# TLS Certificate (CA used by broker)
MQTT_CA_CRT = str((BASE_DIR / "certs" / "ca.crt").resolve()) # paste CA cert into ca.crt file

# Topic structure (scalable IoT hierarchy)
MQTT_BASE_TOPIC = "iot_light_sensor_01" ## ESP32 mqtt client name
MQTT_DATA_TOPIC = f"{MQTT_BASE_TOPIC}/data"



# ---------------- INFLUXDB CONFIGURATION ----------------

INFLUX_URL = "http://192.168.1.75:8086"
INFLUX_TOKEN = "influx_token"
INFLUX_ORG = "org_name"
INFLUX_BUCKET = "bucket_name"

