//                INITIATOR

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

#define TAG               "ESP_NOW_1_INITIATOR"
#define ESP_NOW_CHH_1     1

static void wifi_init_secuence();
static void esp_now_init_secuence();
void esp_now_recv_cb(const esp_now_recv_info_t * esp_now_info, const uint8_t *data, int data_len);
void esp_now_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status);
void esp_now_peer_register(uint8_t * peer_mac_addr);

// MAC DEVKIT V4
static uint8_t peer_mac_addr[] = {0x94, 0xb9, 0x7e, 0xfa, 0xc1, 0x20};

// MAC DEVKIT V1
static uint8_t peer_mac_addr_1[] = {0xb0, 0xa7, 0x32, 0x2a, 0xc1, 0xfc};

void app_main()
{
    wifi_init_secuence();
    esp_now_init_secuence();
    esp_now_peer_register(peer_mac_addr);

    const char * hola_mundo = "Hola Mundo";

    while (1)
    {
        esp_now_send(peer_mac_addr, (uint8_t*)hola_mundo, strlen(hola_mundo));
        vTaskDelay(pdMS_TO_TICKS(1500));

    }

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

void esp_now_peer_register(uint8_t * peer_mac_addr)
{
    esp_now_peer_info_t esp_peer = {
        .channel = ESP_NOW_CHH_1,
        .ifidx = ESP_IF_WIFI_STA,
    };
    memcpy(esp_peer.peer_addr, peer_mac_addr, ESP_NOW_ETH_ALEN);
    esp_now_add_peer(&esp_peer);
}