#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "esp_err.h"

// Czy pomiary są włączone
bool cmd_measurement_status(void);

// Obsługa komendy z MQTT (topic: sonda/cmd)
esp_err_t cmd_handle_message(const char *payload, int payload_len);

#ifdef __cplusplus
}
#endif
