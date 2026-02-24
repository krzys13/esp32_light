#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "esp_err.h"
#include "driver/i2c_master.h"

/* Integration time */
typedef enum {
    VEML7700_IT_25MS  = 0x0C,
    VEML7700_IT_50MS  = 0x08,
    VEML7700_IT_100MS = 0x00,
    VEML7700_IT_200MS = 0x01,
    VEML7700_IT_400MS = 0x02,
    VEML7700_IT_800MS = 0x03,
} veml7700_it_t;

/* Gain */
typedef enum {
    VEML7700_GAIN_1X = 0x00, 
    VEML7700_GAIN_2X = 0x01,
    VEML7700_GAIN_1_4 = 0x02,
    VEML7700_GAIN_1_8 = 0x03,

} veml7700_gain_t;

/* Struktura configuracyjna */
typedef struct {
    i2c_master_bus_handle_t bus;
    uint16_t i2c_addr;          // default 0x10
    uint32_t scl_speed_hz;      // 100000
    veml7700_gain_t gain;
    veml7700_it_t it;

} i2c_veml7700_config_t;


/* Handle sensora */
typedef struct{
    i2c_master_dev_handle_t veml770_handle;
    i2c_veml7700_config_t config;
}i2c_veml7700_handle_t;


esp_err_t i2c_veml7700_init(const i2c_veml7700_config_t *cfg, i2c_veml7700_handle_t *h); // zwraca uchwyt do sensora do skonfigurowanego sensora

esp_err_t i2c_veml7700_read_lux(i2c_veml7700_handle_t *h, float *lux_out);

#ifdef __cplusplus
}
#endif
