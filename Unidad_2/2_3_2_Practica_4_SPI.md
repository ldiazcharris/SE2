
# **Práctica 2.3.2. El protocolo SPI**

## **Objetivo**

Guiar al estudiante en la configuración y uso del periférico SPI.
Mejorar la comprensión del funcionamiento del protocolo SPI.

## **Materiales**

***Hardware***

- 1 Computador.
- 2 Placas de desarrollo basada en ESP32 (cualquiera que tenga a disposición). 
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
	1. Transmitir desde una placa ESP32 (Master) a otra (Slave) a través del bus SPI, una trama de datos.
	1. El ESP32 en modo Slave recibe a través del bus SPI e imprime por UART la trama de datos. 
	
**Este es el código para la ESP32 configurada como Master**

~~~C
// *************** Código para el MASTER*********************************** //

#include <stdio.h>
#include "driver/spi_master.h"
#include "esp_log.h"
#include "utilities.h"

// Definir los pines de SPI
#define MISO_PIN 25
#define MOSI_PIN 23
#define CLK_PIN  19
#define CS_PIN   22

void app_main(void)
{
    spi_device_handle_t spi;

    spi_bus_config_t bus_config = {
        .miso_io_num = MISO_PIN,
        .mosi_io_num = MOSI_PIN,
        .sclk_io_num = CLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    spi_device_interface_config_t device_config={
        .mode = 0,                            //Polaridad y fase del reloj
        .clock_speed_hz = 1000000,           //Velocidad del reloj
        .duty_cycle_pos = 128,              // 50% duty cycle
        .spics_io_num = CS_PIN,           //Pin de selección de chip
        .queue_size = 3,                      //Número de transacciones en la cola de SPI
        .cs_ena_posttrans = 3,              // Keep the CS low 3 cycles after transaction
        .pre_cb = NULL,
        .post_cb = NULL,
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0
    };

    //Configurar bus SPI
    spi_bus_initialize(HSPI_HOST, &bus_config, 1);

    //Agregar dispositivos SPI al bus
    spi_bus_add_device(HSPI_HOST, &device_config, &spi);

    //Enviar y recibir datos
    uint8_t tx_data[] = {'H', 'O', 'L', 'A'};

    while (1)
    {

        spi_transaction_t trans = {
            .length = 8 * sizeof(tx_data),
            .tx_buffer = tx_data,
            .rx_buffer = NULL
        };

        spi_device_transmit(spi, &trans);
        printf("Was sent %s\n", tx_data);
        delay(2000);
    }
}

~~~




**Este es el código para la ESP32 configurada como Slave**

~~~C
// *************** Código para el SLAVE*********************************** //

#include <stdio.h>
#include "driver/spi_slave.h"

#include "utilities.h"

// Definir los pines de SPI
#define MISO_PIN 25
#define MOSI_PIN 23
#define CLK_PIN  19
#define CS_PIN   22


// Buffer para recibir datos
#define BUF_SIZE (128)

void app_main(void)
{
    esp_err_t ret;

    spi_bus_config_t bus_config={
        .miso_io_num = MISO_PIN,
        .mosi_io_num = MOSI_PIN,
        .sclk_io_num = CLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    spi_slave_interface_config_t slave_config = {
        .mode = 0,                              // Polaridad y fase del reloj
        .spics_io_num = CS_PIN,             // Pin de selección de chip
        .queue_size = 10,                        // Tamaño de la cola de transacciones
        .flags = 0,                             // Bandera de configuración adicional
    };

    ret = spi_slave_initialize(HSPI_HOST, &bus_config, &slave_config, SPI_DMA_CH_AUTO); //SPI_DMA_CH1
    if (ret != ESP_OK) {
        printf("Error initializing SPI slave. Code: %d", ret);
        return;
    }

    uint8_t rx_buf[BUF_SIZE];
    esp_err_t size_;
    int size;

    bzero(rx_buf, sizeof(rx_buf));
    
    while (1) {

        printf("While...");

        spi_slave_transaction_t transaction = {
            .length = BUF_SIZE*8,
            .rx_buffer = rx_buf,
            .tx_buffer = NULL,
        };
        
        size_ = spi_slave_transmit(HSPI_HOST, &transaction, portMAX_DELAY);
        
        if (ESP_OK != size_) {
            printf("Error receiving data. Code: %d", size_);            
        }
        else{
            
            printf("Received: %s\n", rx_buf);
            bzero(rx_buf, sizeof(rx_buf));
        }
        
        printf("\n");
        delay(2000);
    }
}


~~~


4. La configuración del hardware que usted debe realizar es la siguiente:
	
	- Necesita mantener conectado el cable de programación al computador y a la placa.
	- Realice el siguiente circuito usando los materiales descritos en la imagen:
	
		<img src="/Unidad_2/imagenes/2_3_2_ejercicio_introductorio_practica_4_SPI.png" width=500>

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

	<img src="/Unidad_1/imagenes/1.10.2_Hercules_3.png" width=250>

- Posteriormente, de clic en el botón Open del panel "Serial". Esto abrirá el puerto serie y podrá transmitir y recibir con la placa ESP32 a través de comunicación UART. 	
	

## **Práctica**

Implemente en un prototipo una comunicación SPI entre 2 ESP32 a saber: un master (ESP32) y un slave (ESP32). 


El prototipo debe realizar lo siguiente:

1. El master debe recibir comandos por UART, usando el driver creado en la práctica "2.2.2. Practica 2 Interrupt driver", pero en lugar de activar los motores debe enviar comandos a través del bus SPI a la ESP32 slave, donde estarán los motores (LEDS). 


# Referencias

- [1] https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32/api-reference/peripherals/adc.html
- [2] https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/ledc.html 