idf_component_register(
    SRCS "mqtt_connection.c"
    INCLUDE_DIRS "include"
    REQUIRES mqtt esp_event esp_netif cmd led
    EMBED_TXTFILES "certs/ca.pem"
)