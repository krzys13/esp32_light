import json
import ssl
import sys
import time

import paho.mqtt.client as mqtt
from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS
from influxdb_client.client.exceptions import InfluxDBError

import config


# --- INFLUXDB INITIALIZATION ---
def init_influx():
    try:
        influx_client = InfluxDBClient(url=config.INFLUX_URL, token=config.INFLUX_TOKEN, org=config.INFLUX_ORG)
        write_api = influx_client.write_api(write_options=SYNCHRONOUS)
        print("Connected to InfluxDB:", config.INFLUX_URL)
        return influx_client, write_api
    except Exception as e:
        print("InfluxDB connection error:", e)
        sys.exit(1)


# -------------------------
# MQTT callbacks
# -------------------------

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        client.connected_flag = True
        print("Connected to MQTT broker, rc =", rc)
        client.subscribe(config.MQTT_DATA_TOPIC, qos=2)
        print("Subscribed to topic:", config.MQTT_DATA_TOPIC)
    else:
        print("MQTT connection failed, rc =", rc)

def on_disconnect(client, userdata, rc):
    client.connected_flag = False
    print("Disconnected from MQTT broker, rc =", rc)

def on_message(client, userdata, message):

    payload = str(message.payload.decode())
    print("Topic:", message.topic)
    print("Message received:", payload)
    print("    ")

    # --- SEND TO INFLUXDB ---
    try:
        data = json.loads(payload)
        lux = float(data["lux"])

        point = Point("sonda").time(time=data["time"]).tag("sensor_type", "VEML7700").field("lux", lux)
        
        userdata["write_api"].write(bucket=config.INFLUX_BUCKET, org=config.INFLUX_ORG, record=point)
        print(f"Data sent to InfluxDB: {lux},")
    except ValueError:
        print("The received message is not a number; nothing was sent to InfluxDB.")
    except InfluxDBError as e:
        print("Error while sending data to InfluxDB:", e)


# --- MQTT INITIALIZATION ---

def build_mqtt_client(write_api):

    client = mqtt.Client(client_id=config.MQTT_CLIENT_ID, protocol=mqtt.MQTTv311, userdata={"write_api": write_api})

    client.username_pw_set(username=config.MQTT_CLIENT_ID, password=config.MQTT_PASS)

    client.connected_flag = False

    client._reconnect_max_delay = 1
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.on_message = on_message

    client.tls_set(
        ca_certs=config.MQTT_CA_CRT,
        certfile=None,
        keyfile=None,
        cert_reqs=ssl.CERT_REQUIRED,
        tls_version=ssl.PROTOCOL_TLS_CLIENT,
        ciphers=None,
    )

    # do not disable certificate verification – we want full security
    client.tls_insecure_set(False)

    return client


def main():
    # --- MAIN LOOP ---
    influx_client, write_api = init_influx()
    client = build_mqtt_client(write_api)
    try:
        while not client.connected_flag:
            try:
                print("Connecting to MQTT broker:", config.MQTT_HOST, "Port:",config.MQTT_PORT)
                client.connect(host=config.MQTT_HOST, port=config.MQTT_PORT)
                client.loop_start()
                while not client.connected_flag:
                    time.sleep(0.1)
            except Exception as e:
                print("Failed to connect to MQTT broker:", e, "Retrying in", 10, "seconds")
                time.sleep(10)

        while True:
            time.sleep(1)

    except KeyboardInterrupt:
        print("Interrupted by user (Ctrl+C).")
        client.loop_stop()
        client.disconnect()
        influx_client.close()
        print("InfluxDB connection closed.")
        sys.exit()

if __name__ == "__main__":
    main()