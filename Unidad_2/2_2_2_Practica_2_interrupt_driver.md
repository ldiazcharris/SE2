
# **Práctica 2.2. Interrupt Driven Driver**

## **Objetivo**

Guiar al estudiante en el diseño de un driver tipo "Interrupt Driven Driver", a través del periférico UART del microcontrolador. 

## **Materiales**

***Hardware***

- 1 Computador.
- 1 Placa de desarrollo basada en ESP32 (cualquiera que tenga a disposición).
- 1 Cable de programación.
- 1 Protoboard.
- 3 LED's.
- 4 cables dupont macho-macho.

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
	
2. Cuando se haya creado el proyecto, despliegue la carpeta `src`, donde está el archivo "main.c" que es el archivo principal del proyecto. En este archivo se escribirá el programa. 
	2.1. Agregue los siguientes archivos en la carpeta `src` de su proyecto: `utilities.h` y `utilities.c`, que se encuentran en la carpeta: ["bibliotecas/untilities"](/Unidad_2/bibliotecas/utilities).
		
3. Escriba el siguiente código dentro del archivo "main.c", cuyo objetivo es: 
	1. Crear dos tareas que se ejecutarán de manera "simultánea" gracias a FreeRTOS.
	1. La "Tarea UART" esperará a que ocurra una interrupción por el puerto UART_0 de la ESP32, para esperar un comando conocido:	
		1. 10: apaga el led 1, y envía el mensaje "Light Code: 10"
		1. 11: enciende el led 1, y envía el mensaje "Light Code: 11"
		1. 20: apaga el led 2, y envía el mensaje "Light Code: 20"
		1. 21: enciende el led 2, y envía el mensaje "Light Code: 21"
		1. Si el código no es correcto, envía el mensaje "Light Code ERROR"
	Además, envía el mensaje "Tarea UART" por UART_0 indicando la entrada a la ejecución del procesamiento de la trama recibida. 
	
	1. La "Tarea LED_3". Permanece haciendo un *blink* del led 3 y enviando el mensaje "Tarea LED_3" por UART_0. 

	~~~C

/************************INCLUDES*********************************/

#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "utilities.h"


/************************MACROS***********************************/

#define BUF_SIZE 1024
#define LED_1 2
#define LED_2 4
#define LED_3 15
#define UART0 UART_NUM_0
#define UART1 UART_NUM_1

/**************DECLARACIÓN DE VARIABLES GLOBALES*******************/

static QueueHandle_t uart0_queue;
static QueueHandle_t uart1_queue;

/****************DECLARACIÓN DE FUNCIONES*************************/

static void uart_interrupt_task(void *params);
static void led_task(void *params);

/**********************FUNCIÓN PRINCIPAL**************************/

void app_main()
{
    gpio_reset_pin(LED_1);
    gpio_reset_pin(LED_2);
    gpio_reset_pin(LED_3);
    gpio_set_direction(LED_1, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_2, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_3, GPIO_MODE_OUTPUT);

    uart_init(UART0, 115200, BUF_SIZE * 2, BUF_SIZE * 2, 50, &uart0_queue, ESP_INTR_FLAG_LEVEL1);
    //          (UART_NUM, TX, RX, RTS, CTS)
    uart_set_pin(UART0,     1,  3,  22,  19);

    uart_init(UART1, 115200, BUF_SIZE * 2, BUF_SIZE * 2, 50, &uart1_queue, ESP_INTR_FLAG_LEVEL1); //   ESP_INTR_FLAG_IRAM
    //          (UART_NUM, TX, RX, RTS, CTS)
    uart_set_pin(UART1,    33, 26,  14,  12);

    xTaskCreate(uart_interrupt_task,
                "Tarea UART",
                BUF_SIZE * 4,
                NULL,
                12,
                NULL);

    xTaskCreate(led_task,
                "Tarea LED_3",
                BUF_SIZE * 4,
                NULL,
                7,
                NULL);
}

/****************DEFINICIÓN DE FUNCIONES**************************/

static void uart_interrupt_task(void *params)
{
    uart_event_t uart_event;
    char *uart_recv_data = (char *)malloc(BUF_SIZE);
    char *response_string = (char *)malloc(BUF_SIZE);
    uint16_t light_code; 
    while (1)
    {

        
        
        if (xQueueReceive(uart0_queue, (void *)&uart_event, (TickType_t)portMAX_DELAY))
        {
            uart_transmit(UART0, "Tarea UART\n", strlen("Tarea UART\n"));
            bzero(uart_recv_data, BUF_SIZE); // Puntero para guardar la información recibida
            bzero(response_string, BUF_SIZE); 

            switch (uart_event.type)
            {
            case UART_DATA:
                uart_receive(UART0, (void *)uart_recv_data, (uint32_t)uart_event.size);
                uart_transmit(UART0, (const void*)uart_recv_data, (uint32_t)uart_event.size);
                
                light_code = (uint16_t)atoi((const char *)uart_recv_data);
                
                switch(light_code)
                {
                    case 10:
                        sprintf(response_string, "Light Code: 10\n");
                        uart_transmit(UART0, response_string, strlen((const char*)response_string));
                        gpio_set_level(LED_1, 0);
                        break;

                    case 11:
                        sprintf(response_string, "Light Code: 11\n");
                        uart_transmit(UART0, response_string, strlen((const char*)response_string));
                        gpio_set_level(LED_1, 1);
                        break;
                    
                    case 20:
                        sprintf(response_string, "Light Code: 20\n");
                        uart_transmit(UART0, response_string, strlen((const char*)response_string));
                        gpio_set_level(LED_2, 0);
                        break;

                    case 21:
                        sprintf(response_string, "Light Code: 21\n");
                        uart_transmit(UART0, response_string, strlen((const char*)response_string));
                        gpio_set_level(LED_2, 1);
                        break;
                    default:
                        sprintf(response_string, "Light Code ERROR\n");
                        gpio_set_level(LED_2, 0);
                        gpio_set_level(LED_1, 1);
                        delay(500);
                        gpio_set_level(LED_2, 1);
                        gpio_set_level(LED_1, 0);
                        delay(500);
                        gpio_set_level(LED_2, 0);
                        gpio_set_level(LED_1, 0);

                        uart_transmit(UART0, response_string, strlen((const char*)response_string));
                }

                //uart_transmit(UART1, response_string, strlen((const char*)response_string));
                break; 

            default:
                break;
            }
        }
    }

    free(uart_recv_data);
    free(response_string);
}


static void led_task(void *params)
{
    while(1){
        
        gpio_set_level(LED_3, 0);
        delay(500);
        gpio_set_level(LED_3, 1);
        delay(500);
        uart_transmit(UART0, "Tarea LED_3\n", strlen("Tarea LED_3\n"));
        delay(500);
    }
    
}

~~~


4. La configuración del hardware que usted debe tener es la siguiente:
	
	- Necesita mantener conectado el cable de programación al computador y a la placa.
	- Realice el siguiente circuito usando los materiales descritos en la imagen:
	
		<img src="/Unidad_2/imagenes/2_2_2_ejercicio_introductorio_practica_2.png" width=500>

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

8. Luego puede usar cualquier programa para leer el puerto serial del computador. Sin embargo, se le recomienda [Hercules](https://www.hw-group.com/software/hercules-setup-utility). 
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
	
	
	- El resultado de esto se verá así: 
	<img src="/Unidad_2/imagenes/2_2_2_Hercules_practica_2.png" width=500>
	
	

## **Práctica**

Desarrolle un driver o biblioteca que facilite la portabilidad de código para crear un programa que sea capaz de controlar el encendido y apagado de 3 motores, a través de las salidas digitales de la placa ESP32. Los comandos de encendido y apagado se enviarán a través de una cadena de comandos separados por comas, usando la comunicación UART. El driver debe seguir los siguientes requerimientos:

1. **Inicialización del puerto UART** 
	
	Aprovéchese la funcionalidad que se compartió en los archivos `utilities.h` y `utilities.c`, para crear un nuevo driver que emplee las mismas funciones (reutilice el código en su propio driver).

2. **Distintas instancias del puerto UART**

	El driver deberá permitir la creación de distintas instancias del puerto UART (en este caso solo 2) en un mismo programa. Es decir, poder configurar más de un puerto UART para la comunicación entre varios dispositivos. Nuevamente, deberá utilizar las funciones ya proporcionadas por `utilities.h`.

3. **Interpretación de la trama UART**
	
	Los motores deben poderse encender y apagar de manera independiente. Por lo cual, deberá agregar una función que esté preparada para interpretar la siguiente trama: 
	
	~~~
	S,motor1_on_off,motor2_on_off,motor3_on_off,E
	~~~
	
	Donde:
	`S` byte que representa el inicio de la trama.
	`E` byte que representa el final de la trama.
	`motor1_on_off`, `motor2_on_off` y `motor2_on_off` byte que representa con valores numéricos comprendidos entre 0 y 1. Que representan los estados de apagado y activación de los tres motores. 
	
	En consecuencia, el microcontrolador deberá realizar la acción pertinente sobre los motores. 
	
	
4. **Consideraciones para la interpretación de la trama**
	
	La función a desarrollar para interpretar la trama UART recibida debe considerar las siguientes condiciones:
	
	1. Debe recibir la trama a través de una cola de freeRTOS.
	2. Si la trama no contiene los caracteres `S` y `E`, la trama deberá ser descartada. 
	3. Si los valores de las tramas `motor1_on_off`, `motor2_on_off` y `motor2_on_off` no son numéricos y no están en el rango permitido, la trama debe ser descartada. 


5. **Prueba del driver**

Para comprobar la funcionalidad del driver, debe enviar 5 tramas erróneas con diferentes combinaciones y 5 tramas válidas con diferentes combinaciones. El prototipo debe responder adecuadamente a todas las pruebas. Es decir, descartar las condiciones de trama errónea sin inmutarse y realizar los cambios solicitados en las tramas válidas. 


# Referencias

- [1] https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32/api-reference/peripherals/adc.html
- [2] https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/ledc.html 