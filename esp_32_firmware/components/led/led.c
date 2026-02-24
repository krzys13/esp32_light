#include "led.h"

#include <stdbool.h>

#include "driver/gpio.h"
#include "esp_log.h"



static const char *TAG = "LED";
static bool led_state = false;

static const gpio_config_t gpio_conf = {
        .pin_bit_mask = (1ULL << LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };


esp_err_t led_init(void)
{
    
    esp_err_t ret = gpio_config(&gpio_conf);
    if (ret == ESP_OK) {
         ESP_LOGI(TAG, "GPIO config success");
    }
    else{
        ESP_LOGE(TAG, "GPIO config failed (%s)", esp_err_to_name(ret));
    }
    
    return ret;
}

esp_err_t led_on(void)
{
    esp_err_t ret = gpio_set_level(LED_GPIO, 1);
    if (ret == ESP_OK) {
        led_state = true;
        ESP_LOGI(TAG, "Success to SET LED ON");
    } else {
        ESP_LOGE(TAG, "Failed to set LED ON (%s)", esp_err_to_name(ret));
    }

    return ret;
}

esp_err_t led_off(void)
{
    esp_err_t ret = gpio_set_level(LED_GPIO, 0);
    if (ret == ESP_OK) {
        led_state = false;
        ESP_LOGI(TAG, "Success to SET LED OFF");
    } else {
        ESP_LOGE(TAG, "Failed to set LED OFF (%s)", esp_err_to_name(ret));
    }

    return ret;
}