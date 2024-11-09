//                RESPONDER

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/Task.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"

#define TAG               "ESP_NOW_2_RESPONDER"
#define ESP_NOW_CHH_1     1

static void wifi_init_secuence();
static void esp_now_init_secuence();
void esp_now_recv_cb(const esp_now_recv_info_t * esp_now_info, const uint8_t *data, int data_len);
void esp_now_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status);
void esp_now_peer_register(uint8_t * peer_mac_addr);


void app_main()
{
    wifi_init_secuence();
    esp_now_init_secuence();
}

static void wifi_init_secuence()
{
    wifi_init_config_t wifi_init_struct = WIFI_INIT_CONFIG_DEFAULT();

    esp_netif_init();
    esp_event_loop_create_default();
    nvs_flash_init();
    esp_wifi_init(&wifi_init_struct);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_storage(WIFI_STORAGE_FLASH);
    esp_wifi_start();
    ESP_LOGI(TAG, "ESP_NOW Init OK");

}

static void esp_now_init_secuence()
{
    esp_now_init();
    esp_now_register_recv_cb(esp_now_recv_cb);
    esp_now_register_send_cb(esp_now_send_cb);    

}

void esp_now_recv_cb(const esp_now_recv_info_t * esp_now_info, const uint8_t *data, int data_len)
{
    ESP_LOGI(TAG, "Data received: " MACSTR "%s", MAC2STR(esp_now_info->src_addr), data);
}

void esp_now_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    if(ESP_NOW_SEND_SUCCESS == status)
    {
        ESP_LOGI(TAG, "ESP_NOW_SEND_SUCCESS");
    }
    else
    {
        ESP_LOGI(TAG, "ESP_NOW_SEND_FAIL");
    }
}
