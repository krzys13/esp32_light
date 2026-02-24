#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#include "mqtt_client.h"
#include <stdbool.h>     // bool
#include "esp_err.h"
// Flaga stanu MQTT (zdefiniowana w mqtt_connection.c)
bool mqtt_conneection_status();

// Uruchamia klienta MQTT (inicjalizacja + start + rejestracja eventów)
esp_err_t mqtt_app_start (esp_mqtt_client_handle_t *mqtt_client_h);

#ifdef __cplusplus
}
#endif
