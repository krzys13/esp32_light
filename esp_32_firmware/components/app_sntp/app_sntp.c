#include "sntp.h"
#include "esp_netif_sntp.h"
#include "esp_log.h"

#define TIME_SYN_MAX_ATTEMPS 10

static const char *TAG = "SNTP";


esp_err_t sntp_time_synch(void)
{
    esp_netif_sntp_deinit();

    esp_sntp_config_t config =
        ESP_NETIF_SNTP_DEFAULT_CONFIG("tempus1.gum.gov.pl");

    config.smooth_sync   = true;
    config.wait_for_sync = true;

    esp_err_t err = esp_netif_sntp_init(&config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init SNTP");
        return err;
    }

    esp_netif_sntp_start();

    int attempt = 1;

    while (attempt <= TIME_SYN_MAX_ATTEMPS)
    {
        err = esp_netif_sntp_sync_wait(pdMS_TO_TICKS(10000));

        if (err == ESP_OK) {
            ESP_LOGI(TAG, "SNTP time synchronized (attempt %d)", attempt);
            return ESP_OK;
        }

        ESP_LOGW(TAG,
                 "Failed to sync time within 10s (attempt %d/%d)",
                 attempt,
                 TIME_SYN_MAX_ATTEMPS);

        attempt++;
    }

    ESP_LOGE(TAG, "SNTP synchronization failed after %d attempts",TIME_SYN_MAX_ATTEMPS);

    return ESP_FAIL;

}
