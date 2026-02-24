#include "veml7700.h"

#include "driver/i2c_master.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ALS_CONF_REGISTER 0x00
#define ALS_DATA_REGISTER  0x04


static const char *TAG= "VEML7700";



static float it_to_ms(veml7700_it_t it)
{
    switch (it) {
        case VEML7700_IT_25MS:  return 25.0f;
        case VEML7700_IT_50MS:  return 50.0f;
        case VEML7700_IT_100MS: return 100.0f;
        case VEML7700_IT_200MS: return 200.0f;
        case VEML7700_IT_400MS: return 400.0f;
        case VEML7700_IT_800MS: return 800.0f;
        default:                return 100.0f;
    }
}

static float gain_to_gain_factor(veml7700_gain_t g)
{
    switch (g) {
        case VEML7700_GAIN_1X:  return 1.0f;
        case VEML7700_GAIN_2X:  return 2.0f;
        case VEML7700_GAIN_1_4: return 0.25f;
        case VEML7700_GAIN_1_8: return 0.125f;
        default:                return 1.0f;
    }
}

static uint16_t build_config_register(const i2c_veml7700_config_t *cfg)
{
    // pers=1, int_disable=0, shutdown=0
    const uint16_t pers = 0;      // 2 bity
    const uint16_t int_en = 0;    // 1 bit
    const uint16_t sd = 0;        // 1 bit

    uint16_t conf_value =
       ((uint16_t)cfg->gain << 11) |
        ((uint16_t)cfg->it   << 6 ) |
        ((uint16_t)pers      << 4 ) |
        ((uint16_t)int_en    << 2 ) |
        ((uint16_t)sd        << 0 );

    return conf_value;
}


esp_err_t i2c_veml7700_init(const i2c_veml7700_config_t *cfg, i2c_veml7700_handle_t *h) 
{
    h->config = *cfg; // przepiusanie configu do handlera

    const i2c_device_config_t i2c_VEML7700_conf ={
        .device_address = cfg->i2c_addr,
        .dev_addr_length = I2C_ADDR_BIT_7,
        .scl_speed_hz = cfg->scl_speed_hz,
    };
    
    ESP_ERROR_CHECK(i2c_master_bus_add_device(cfg->bus, &i2c_VEML7700_conf, &h->veml770_handle)); // dopisanie i2c_handlera do handlera veml7700
    

    // Rozbijamy na dwa bajty (VEML7700 wymaga LSB, potem MSB)
    uint8_t tx_buf[3];
    uint16_t conf_value = build_config_register(cfg);
    tx_buf[0] = ALS_CONF_REGISTER;           // adres rejestru (0x00)
    tx_buf[1] = (uint8_t)(conf_value & 0xFF); // LSB
    tx_buf[2] = (uint8_t)(conf_value >> 8);   // MSB

    
    esp_err_t ret = i2c_master_transmit(h->veml770_handle, tx_buf, 3, pdMS_TO_TICKS(1000));

    if( ret == ESP_OK){
        ESP_LOGI(TAG, "Powered on and configurated sensor VEML7700 succes");
}
    else{   
        ESP_LOGE(TAG, "Power on and configurate sensor VEML7700 fault");
    }
        return ret;
}


esp_err_t i2c_veml7700_read_lux(i2c_veml7700_handle_t *h, float *lux_out)
{
    uint8_t rx_buf[2];
    uint8_t data_reg = ALS_DATA_REGISTER;
    uint16_t als_raw_data;
    // Przelicznik lux wg datasheet
    float gain_factor = gain_to_gain_factor(h->config.gain);
    float coeff = 0.0672 * (100.0f / it_to_ms(h->config.it)) * (1.0f / gain_factor);
    float lux;

    vTaskDelay(pdMS_TO_TICKS(it_to_ms(h->config.it)));
    // Write + Read (2 bajty)
    esp_err_t err = i2c_master_transmit_receive(h->veml770_handle,
                                                &data_reg, 1,
                                                rx_buf, 2,
                                                pdMS_TO_TICKS(1000)
                                                );

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Measurment read timeout");
        return ESP_ERR_TIMEOUT;
    }
    
    // LSB pierwszy, potem MSB
    als_raw_data = ((uint16_t)rx_buf[1] << 8) | rx_buf[0];

    // Przeliczenie na lux
    lux = als_raw_data* coeff ;
    *lux_out = lux;
    return ESP_OK;
    
}

     








// float VEML7700_measure(void)
// {
//     const int count_amount = 10; 
//     double lux_sum =0;
//     uint8_t rx_buf[2];
//     uint16_t als_raw;
//     uint8_t reg = ALS_DATA_REGISTER; // adres rejestru z wynikiem

//     // Przelicznik lux wg datasheet
//     float gain_factor = VEML7700_GAIN_FACTOR(ALS_GAIN);
//     float coeff = 0.0672 * (100.0f / VEML7700_IT_TO_MS(ALS_IT)) * (1.0f / gain_factor);
//     float lux;

//     for (int i = 0; i < count_amount; i++) {
//         // Czekamy tyle, ile wynosi integration time
//         vTaskDelay(pdMS_TO_TICKS(10* 4 * VEML7700_IT_TO_MS(ALS_IT))); // zrobione X4 aby całkowity czas uśredniania wynosił 10 sekund bo pomiar co 100 ms a robimy ich 100

//         // Write + Read (2 bajty)
//         ESP_ERROR_CHECK(
//             i2c_master_transmit_receive(
//                 i2c_VEML7700_handle,
//                 &reg, 1,
//                 rx_buf, 2,
//                 pdMS_TO_TICKS(1000)
//             )
//         );

//         // LSB pierwszy, potem MSB
//         als_raw = ((uint16_t)rx_buf[1] << 8) | rx_buf[0];

//         // Przeliczenie na lux
//         lux = als_raw * coeff;
//         lux_sum += lux;
//     }

//     return (lux_sum / count_amount);
// }
    
    
    




    