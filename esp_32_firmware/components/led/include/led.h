#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define LED_GPIO GPIO_NUM_2

#include "esp_err.h"

esp_err_t led_init(void);
esp_err_t led_on(void);
esp_err_t led_off(void);

#ifdef __cplusplus
}
#endif