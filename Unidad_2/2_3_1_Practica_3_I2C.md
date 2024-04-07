
# **Práctica 2.3.1. El protocolo $I^2C$**

## **Objetivo**

Guiar al estudiante en la configuración y uso del periférico $I^2C$.
Mejorar la comprensión del funcionamiento del protocolo $I^2C$.

## **Materiales**

***Hardware***

- 1 Computador.
- 2 Placas de desarrollo basada en ESP32 (cualquiera que tenga a disposición). (Trabajo inter-grupos)
- 1 Cable de programación.
- 1 Protoboard.
- 1 Display LCD 16x2.
- 10 cables dupont macho-macho.

***Software***

- IDE *Visual Studio Code (VSCode)*, 
- Extensión de VSCode *PlatformIO*. A través de esta extensión tiene acceso a:
	- Framework ESP-IDF que esencialmente contiene la API (bibliotecas de software y código fuente) para la ESP32 y scripts para operar la *Toolchain* para compilar código.
	- Herramientas de compilación para crear una aplicación completa para ESP32.
		- CMake, and 
		- Ninja.
- [Hercules](https://www.hw-group.com/software/hercules-setup-utility).

Para realizar una instalación limpia de este software, por favor vaya a la [Guía de instalación](1.9_guia_instal_tools.md).

### **Ejercicio introductorio**

Por favor, siga la siguiente metodología.

1. Inicie el programa VSCode y cree un nuevo proyecto con la herramienta platformIO siguiendo los pasos de la sección [Crear un nuevo proyecto con platformIO](/Unidad_1/0_nuevo_proyecto.md).
	
2. Cuando se haya creado el proyecto, despliegue la carpeta `src`, donde está el archivo "main.c" que es el archivo principal del proyecto. En este archivo se escribirá el programa, cuyo objetivo es: 
	1. Transmitir desde una placa ESP32 (Master) a otra (Slave) a través del bus $I^2C$, una trama de datos.
	1. El ESP32 en modo Slave recibe e imprime por UART la trama de datos. 
	
**Este es el código para la ESP32 configurada como Master**

~~~C
// *************** Código para el MASTER*********************************** //

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"

// Parámetros de configuración para la comunicación I2C
#define SLAVE_ADDR          0x32   
#define MASTER_PORT         I2C_NUM_0 
#define MASTER_SDA          21   
#define MASTER_SCL          22         
#define MASTER_FREQ_HZ      100000     
     
// Función de inicialización del controlador I2C para modo Master
void i2c_master_init() {

    i2c_config_t i2c_master_config = {

    .mode = I2C_MODE_MASTER,
    .sda_io_num = MASTER_SDA,
    .scl_io_num = MASTER_SCL,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = MASTER_FREQ_HZ,
    };
	
    ESP_ERROR_CHECK(i2c_param_config(MASTER_PORT, &i2c_master_config));
    ESP_ERROR_CHECK(i2c_driver_install(MASTER_PORT, i2c_master_config.mode, 0, 0, 0));
}

void app_main() {

    i2c_master_init();
                        //    'H'  'O'   'L',   'A'
    const uint8_t data[4] = {0x48, 0x4F, 0x4C, 0x41}; 
    
    while(1) {

        // Se transmite la información al SLAVE
        // Parámetros que recibe la función i2c_master_write_to_device()
        //                        (i2c_num, device_address, *write_buffer, write_size, ticks_to_wait)
        ESP_ERROR_CHECK(i2c_master_write_to_device(MASTER_PORT, SLAVE_ADDR, data, (size_t)sizeof(data), 100/portTICK_PERIOD_MS));
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);

    }
}

~~~




**Este es el código para la ESP32 configurada como Slave**

~~~C
// *************** Código para el SLAVE*********************************** //

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include <stdlib.h>
#include <string.h>

// Parámetros de configuración para la comunicación I2C
#define SLAVE_RX_BUF        1024
#define SLAVE_ADDR          0x32
#define SLAVE_SCL           22
#define SLAVE_SDA           21


// Función de inicialización del controlador I2C para modo Slave
void i2c_slave_init() {

    i2c_config_t i2c_slave_config = {

    .mode = I2C_MODE_SLAVE,
    .sda_io_num = SLAVE_SDA,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_io_num = SLAVE_SCL,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .slave.addr_10bit_en = 0,
    .slave.slave_addr = SLAVE_ADDR,
    };

    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &i2c_slave_config));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, i2c_slave_config.mode, SLAVE_RX_BUF*2, 0, 0));
}

void app_main() {

    i2c_slave_init();
    
    size_t size = 4;
    uint8_t data[4];
    int len = 0;

    bzero(data, size);
    
    while(1) {
        
        // Se recibe eternamente información del Master
        len = i2c_slave_read_buffer(I2C_NUM_0, data, size, portMAX_DELAY);
        if(len > 0)
            printf("Received data: %c, %c, %c, %c\n", data[0], data[1], data[2], data[3]);
        
    }
    
}

~~~


4. La configuración del hardware que usted debe realizar es la siguiente:
	
	- Necesita mantener conectado el cable de programación al computador y a la placa.
	- Realice el siguiente circuito usando los materiales descritos en la imagen:
	
		<img src="/Unidad_2/imagenes/2_3_1_ejercicio_introductorio_practica.png" width=500>

5. En la parte inferior de Visual Studio Code hay una serie de botones, se describen los más relevantes:

	<img src="/Unidad_1/imagenes/1.10_barra_inferior_platformIO.png" width=500>

	*Barra de herramientas de PlatformIO.*

	1. *"Build"*. Compilar el proyecto.
	1. *"Upload"*. Cargar el proyecto a la placa.
	1. *"Serial monitor"*. Abrir un monitor serial.


6. Compile el proyecto usando el botón *"Build"*. La primera vez puede ser demorado ya que crea todos los archivos del proyecto. Si todo es correcto, se obtiene un mensaje similar al siguiente:

	<img src="/Unidad_1/imagenes/1.10_pront_build_programa.png" width=500>
	
7. Luego dar clic en *"Upload"* para subir el programa a la placa Si todo es correcto, se obtiene un mensaje similar al siguiente:

	<img src="/Unidad_1/imagenes/1.10_pront_carga_programa.png" width=500>

8. **Abra el puerto UART_0 de la placa ESP32 que está en modo *Slave* en un monitor serial.**

Puede usar cualquier programa para leer el puerto serial del computador. Sin embargo, se le recomienda [Hercules](https://www.hw-group.com/software/hercules-setup-utility). 
- Hercules es un software portable (no requiere instalación). Para los usuarios de Windows, luego de descargar el archivo, para ejecutarlo puede hacer doble clic.
- Al ejecutarlo, se le mostrará la siguiente vista:

<img src="/Unidad_1/imagenes/1.10.2_Hercules_1.png" width=500>

- Vaya a la pestaña "Serial" que se encuentra en la parte superior izquierda de la ventana de Hercules. De clic en ella y se mostrará la siguiente vista:

<img src="/Unidad_1/imagenes/1.10.2_Hercules_2.png" width=500>

- Luego, en el panel derecho denominado "Serial", configure la comunicación UART. En esta guía no se profundizará en la comunicación UART, que se dejará para guías posteriores. Simplemente, siga las siguientes recomendaciones:
	- Elija el Puerto COM al que está conectada la tarjeta ESP32. Para los usuarios de Windows, esto lo puede averiguar en la "Administración de Dispositivos".
	- Luego configure cada uno de los campos como se muestra a continuación:

	<img src="/Unidad_1/imagenes/1.10.2_Hercules_3.png" width=100>

- Posteriormente, de clic en el botón Open del panel "Serial". Esto abrirá el puerto serie y podrá transmitir y recibir con la placa ESP32 a través de comunicación UART. 	
	

## **Práctica**

Implemente en un prototipo una comunicación I2C entre 3 dispositivos a saber: un master (ESP32) y dos slaves (ESP32 y pantalla LCD). 

Para la pantalla LCD puede usar el [driver LCD I2C](https://github.com/ldiazcharris/SE1/tree/main/Unidad_2/bibliotecas/lcd_i2c) proporcionado en el curso de Sistemas Embebidos I. [Ver Práctica 2.6 Integración con visualizadores, para recordar](https://github.com/ldiazcharris/SE1/blob/main/Unidad_2/2.6_practica_2_6_visualizadores.md).

El prototipo debe realizar lo siguiente:

1. El master debe recibir comandos por UART, usando el driver creado en la práctica "2.2.2. Practica 2 Interrupt driver", pero en lugar de activar los motores debe enviar comandos a través del bus $I^2$ a la ESP32 slave, donde estarán los motores (LEDS). 
1. El master debe imprimir en la pantalla LCD el estado de los 3 motores. 



# Referencias

- [1] https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32/api-reference/peripherals/adc.html
- [2] https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/ledc.html 