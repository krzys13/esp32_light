
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "time.h"
#include "esp_log.h"



#include "mqtt_connection.h"
#include "wifi_connection.h"
#include "veml7700.h"
#include "sntp.h"
#include "cmd.h"
#include "mqtt_client.h"



#define MEASURMENTS_TO_AVG 10

static const char *TAG = "app_main";

QueueSetHandle_t measure_queue;
esp_mqtt_client_handle_t mqtt_client_handle;

typedef struct node_data_t
{
    float lux;
    time_t time;
} node_data_t;  

static const i2c_master_bus_config_t i2c_mst_bus_conf = {
    .clk_source = I2C_CLK_SRC_DEFAULT,         
    .i2c_port = CONFIG_I2C_PORT,
    .scl_io_num = CONFIG_I2C_SCL_GPIO,         
    .sda_io_num = CONFIG_I2C_SDA_GPIO,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
     };

static i2c_master_bus_handle_t i2c_mst_bus_handle;

static i2c_veml7700_handle_t veml7700_handle;


void task_measure (void * pvParameters)
{
    float lux = 0;
    float lux_sum = 0;
    float lux_avg = 0; 
    i2c_new_master_bus(&i2c_mst_bus_conf, &i2c_mst_bus_handle);

     const i2c_veml7700_config_t veml7700_config = {
        .bus = i2c_mst_bus_handle,
        .gain = VEML7700_GAIN_1_8,
        .i2c_addr = 0x10,
        .it =VEML7700_IT_100MS,
        .scl_speed_hz = 100000,  
    };

    i2c_veml7700_init(&veml7700_config, &veml7700_handle);
    
    while (1)
    {
        if (!cmd_measurement_status()) 
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }
    
        lux_sum = 0;

        for (uint16_t i = 0; i < MEASURMENTS_TO_AVG; i++)
        {

            i2c_veml7700_read_lux(&veml7700_handle, &lux);
            lux_sum += lux;
        }

        lux_avg = lux_sum / MEASURMENTS_TO_AVG;
        node_data_t measure_data = (struct node_data_t) {
        .lux = lux_avg,};
        printf("%f\n",lux_avg);

        time(&measure_data.time);
        xQueueSend(measure_queue, &measure_data, pdMS_TO_TICKS(0));
    };
};


void task_send_MQTT( void *pvParameters)
{
    node_data_t measure_data;
    struct tm time_info ={0};
    char time_info_buffer[20];
    char data_buffer[80];
    while(1)
    {
        //int queue_length = uxQueueMessagesWaiting(measure_queue);
        
        xQueueReceive(measure_queue, &measure_data, portMAX_DELAY); // w tym czasie zawsze powienin przyjsc pomiar
        localtime_r(&measure_data.time, &time_info);
        strftime(time_info_buffer, sizeof(time_info_buffer), "%Y-%m-%d %H:%M:%S", &time_info);
        snprintf(data_buffer, sizeof(data_buffer),"{\"time\": \"%s\", \"lux\": %f}", time_info_buffer, measure_data.lux);

        esp_mqtt_client_publish(mqtt_client_handle, "sonda/data", data_buffer, strlen(data_buffer), 2, 0);
        vTaskDelay(pdMS_TO_TICKS(100));  // rozladowywanie kolejki gdyby urosla
    }

    
}
void app_main(void)
{
    
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    

    wifi_init_sta();
    mqtt_app_start(&mqtt_client_handle);
    measure_queue = xQueueCreate(500, sizeof(node_data_t)); // toworzy kolejke pomiedzy TaskMeasure a TaskSend
    xTaskCreate(task_measure, "task_measure", 10* 1024, NULL, 1, NULL);
    xTaskCreate(task_send_MQTT, "task_print", 10* 1024, NULL, 1, NULL);
    
}
