#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_netif.h"
#include "esp_http_client.h"
// #include "audio_element.h"
// #include "audio_pipeline.h"
// #include "audio_event_iface.h"
// #include "audio_event_iface.h"
// #include "audio_pipeline.h"
// #include "i2s_stream.h"
// #include "mp3_decoder.h"

// static audio_pipeline_handle_t pipeline;
// static audio_element_handle_t i2s_stream_writer, mp3_decoder;


const char *ssid = "JaisNet2";
const char *pass = "heyheygaga";

bool wifi_connected = false;
int retry_num = 0;

esp_err_t client_event_post_handler(esp_http_client_event_handle_t evt) {
    switch (evt->event_id) {
    case HTTP_EVENT_ON_DATA:
        printf("--- POST RESPONSE -----------\n");
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        printf("-------------------------\n");
        // audio_element_input(mp3_decoder, (char *)evt->data, evt->data_len);

        break;

    default:
        break;
    }
    return ESP_OK;
}

static void post_rest(const char *url, const char *data) {
    esp_http_client_config_t config_post = {
        // .url = "https://gappu-nextjs.vercel.app/api/google/translate",
        .url=url,
        .method = HTTP_METHOD_POST,
        .cert_pem = NULL,
        .event_handler = client_event_post_handler};

    esp_http_client_handle_t client = esp_http_client_init(&config_post);

    // const char *post_data = "{\n    \"text\": \"here you start to learn about ramayan\",\n    \"source\": \"en\",\n    \"target\": \"hi\"\n\n}";

    esp_http_client_set_post_field(client, data, strlen(data));
    esp_http_client_set_header(client, "Content-Type", "application/json");

    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf('------------- POST RESPONSE ----------------\n');
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        printf('--------------------------------\n');
            // xSemaphoreTake(xSemaphore, portMAX_DELAY);
            // i2s_write_bytes(I2S_NUM, (const char *)evt->data, evt->data_len, portMAX_DELAY);
            // xSemaphoreGive(xSemaphore);

        break;

    default:
        break;
    }
    return ESP_OK;
}

static void rest_get()
{
    esp_http_client_config_t config_get = {
        .url = "http://worldclockapi.com/api/json/utc/now",
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        .event_handler = client_event_get_handler};
        
    esp_http_client_handle_t client = esp_http_client_init(&config_get);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_id == WIFI_EVENT_STA_START) {
        printf("WIFI CONNECTING....\n");
    } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
        wifi_connected = true;
        printf("WiFi CONNECTED\n");
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        printf("WiFi lost connection\n");
        wifi_connected = false;
        if (retry_num < 5) {
            esp_wifi_connect();
            retry_num++;
            printf("Retrying to Connect...\n");
        }
    } else if (event_id == IP_EVENT_STA_GOT_IP) {
        printf("Wifi got IP...\n\n");
        if (wifi_connected) {
            printf("Make the post request\n\n");
            const char *service = "https://gappu-nextjs.vercel.app/api/openai/text-to-speech";
            const char *data = "{\n    \"text\": \"Where are you going?\",\n    \"language\": \"en\"\n}";
            // const char *service = "https://gappu-nextjs.vercel.app/api/openai/translate";
            // const char *data = "{\n    \"text\": \"A quick brown fox jumped over the lazy dog\",\n    \"source\": \"en\",\n    \"target\": \"hi\"\n}";
            post_rest(service, data);
            // rest_get();
        }
    }
}

void wifi_connection() {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);

    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = "",
            .password = "",
        }
    };

    strcpy((char *)wifi_configuration.sta.ssid, ssid);
    strcpy((char *)wifi_configuration.sta.password, pass);

    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    esp_wifi_start();
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_connect();

    printf("wifi_init_softap finished. SSID:%s  password:%s\n", ssid, pass);
}

void app_main(void) {
    // audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();

    // pipeline = audio_pipeline_init(&pipeline_cfg);
    // mem_assert(pipeline);

    // audio_element_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    // i2s_stream_writer = i2s_stream_init(&i2s_cfg);

    // audio_element_cfg_t mp3_cfg = DEFAULT_MP3_DECODER_CONFIG();
    // mp3_decoder = mp3_decoder_init(&mp3_cfg);

    // audio_pipeline_register(pipeline, mp3_decoder, "mp3");
    // audio_pipeline_register(pipeline, i2s_stream_writer, "i2s");

    // audio_pipeline_link(pipeline, (const char *[]){"mp3", "i2s"}, 2);

    // audio_pipeline_run(pipeline);

    nvs_flash_init();
    wifi_connection();
}
