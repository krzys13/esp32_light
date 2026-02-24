#include "cmd.h"
#include <string.h>
#include <ctype.h>
#include "esp_log.h"
#include "esp_system.h"

#define CMD_BUFFER_SIZE 64

static const char *TAG = "CMD";

// Globalny stan pomiarów
static bool is_measure_enabled = true;


// Getter stanu pomiarów
bool cmd_measurement_status(void)
{
    return is_measure_enabled;
}
// Usuwa białe znaki z końca stringa (\r, \n, spacje)
static void trim_end(char *buf)
{
    size_t len = strlen(buf);
    
    while (len > 0 && isspace((unsigned char)buf[len - 1])) {
    buf[--len] = '\0';
}
}

// Obsługa komend z MQTT
esp_err_t cmd_handle_message(const char *payload, int payload_len)
{
    if (payload == NULL || payload_len <= 0) {
        ESP_LOGE(TAG, "Invalid argument: payload is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    char cmd[CMD_BUFFER_SIZE];

    //Przepisanie do bufora
      size_t n = (size_t)payload_len;
    if (n > sizeof(cmd) - 1) {
        ESP_LOGW(TAG, "Payload too long (%d), truncating to %d bytes", payload_len, (int)(sizeof(cmd) - 1));
        n = sizeof(cmd) - 1;
    }

    memcpy(cmd, payload, n);
     cmd[n] = '\0';

    // Usuwamy \r\n z końca
    trim_end(cmd);
    
    
 if (strcmp(cmd, "restart") == 0) {
        ESP_LOGW(TAG, "ESP RESTART");
        esp_restart();
        return ESP_OK; 

    } else if (strcmp(cmd, "measure on") == 0) {
        is_measure_enabled = true;
        ESP_LOGI(TAG, "Measurements ENABLED");
        return ESP_OK;

    } else if (strcmp(cmd, "measure off") == 0) {
        is_measure_enabled = false;
        ESP_LOGI(TAG, "Measurements DISABLED");
        return ESP_OK;

    } else {
        ESP_LOGW(TAG, "Unknown command: '%s'", cmd);
        return ESP_ERR_NOT_SUPPORTED;
    }
}