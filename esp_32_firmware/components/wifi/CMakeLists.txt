idf_component_register(
    SRCS "wifi_connection.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_wifi esp_event nvs_flash app_sntp 
)
