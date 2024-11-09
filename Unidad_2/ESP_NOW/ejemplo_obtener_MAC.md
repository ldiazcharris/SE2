# Ejemplo ESP-NOW: ¿Cómo obtener la MAC?

## **Materiales**

***Hardware***

- 1 Computador.
- 2 Placas de desarrollo basada en ESP32 (cualquiera que tenga a disposición). 
- 1 Cable de programación.
- 1 Protoboard.
- 10 cables dupont macho-macho.
- 2 potenciómetros de 10 kΩ
- 3 LED's

***Software***

- IDE *Visual Studio Code (VSCode)*, 
- Extensión de VSCode *PlatformIO*. A través de esta extensión tiene acceso a:
	- Framework ESP-IDF que esencialmente contiene la API (bibliotecas de software y código fuente) para la ESP32 y scripts para operar la *Toolchain* para compilar código.
	- Herramientas de compilación para crear una aplicación completa para ESP32.
		- CMake, and 
		- Ninja.
- [Hercules](https://www.hw-group.com/software/hercules-setup-utility).

Para realizar una instalación limpia de este software, por favor vaya a la [Guía de instalación](1.9_guia_instal_tools.md).

### **Metodología**

1. Inicie el programa VSCode y cree un nuevo proyecto con la herramienta platformIO siguiendo los pasos de la sección [Crear un nuevo proyecto con platformIO](/Unidad_1/0_nuevo_proyecto.md).
	
1. Cuando se haya creado el proyecto, despliegue la carpeta `src`, donde está el archivo "main.c" que es el archivo principal del proyecto. En este archivo se escribirá el programa que se indican en los pasos subsiguientes. El objetivo de esto es: 
	- Inicializar por defecto la ESP32 e imprimir por UART la dirección MAC del dispositivo. 
1. Escriba el siguiente código en el proyecto y cárguelo en las dos ESP32.

***Código del Inicator***

~~~C
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

#define TAG               "ESP_NOW_EXAMPLE"
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

~~~

1. Una vez cargado, abra *Hercules* y conéctelo con el puerto serie de cada ESP32. 

1. Reinicie las placas sin desconectar el puerto serie. Luego, seguirá la secuencia de reinicio y al final le mostrará la dirección MAC del dispositivo, así:

	<img src="/Unidad_2/ESP_NOW/resultado_ejemplo_esp_now.png" width=300>

1. Para poder utilizar esta dirección en el probrama de C, debe guardarla en un array de la siguiente forma `mac_addr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}`. Reemplace los valores de la dirección MAC en el array. Según el presente ejemplo, quedaría de la siguiente manera: `mac_addr[] = {0x94, 0xb9, 0x7e, 0xfa, 0xc1, 0x20}`. 

