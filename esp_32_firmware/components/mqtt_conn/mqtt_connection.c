#include "mqtt_connection.h"
#include "mqtt_client.h"
#include "cmd.h"
#include "led.h"

#include <string.h>
#include <inttypes.h>
#include "esp_log.h"


static const char *TAG = "MQTT_";

extern const uint8_t ca_pem_start[] asm("_binary_ca_pem_start");
extern const uint8_t ca_pem_end[]   asm("_binary_ca_pem_end");


static bool is_mqtt_connected = false;

bool mqtt_connection_status(void)
{
    return is_mqtt_connected;
}



static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}


static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch ((esp_mqtt_event_id_t) event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        led_on();
        is_mqtt_connected = true;

        esp_mqtt_client_subscribe(client, "sonda/cmd", 2);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "MQTT_EVENT_DISCONNECTED");
        led_off();
        is_mqtt_connected = false;
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED to, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        ESP_LOGI(TAG, "TOPIC=%.*s", event->topic_len, event->topic);
        ESP_LOGI(TAG, "DATA=%.*s",  event->data_len,  event->data);
        
        // Przekierowanie wiadomosci do cmd
        if (event->topic_len == strlen("sonda/cmd") &&
        memcmp(event->topic, "sonda/cmd", event->topic_len) == 0){
            cmd_handle_message(event->data, event->data_len);}
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;

    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

esp_err_t mqtt_app_start (esp_mqtt_client_handle_t *mqtt_client_h)
{
    esp_err_t err;
    led_init();

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_BROKER_URL,
        .credentials.username = CONFIG_BROKER_USERNAME, 
        .credentials.authentication.password = CONFIG_BROKER_PASSWORD,

        .broker.verification.certificate = (const char *)ca_pem_start,
        .broker.verification.certificate = (const char *)ca_pem_start,
        
        .session.last_will = { 
            .msg ="SIGNAL HAS BEEN LOST !!!",
            .topic= "sonda/cmd"
            },
       };
       
    
    *mqtt_client_h = esp_mqtt_client_init(&mqtt_cfg);
    
    err = esp_mqtt_client_register_event(*mqtt_client_h, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    if( err != ESP_OK){
        return err;
    }

    esp_mqtt_client_start(*mqtt_client_h);
    if(err != ESP_OK)
    { 
        return err;
    }

    return(ESP_OK);
}
    








