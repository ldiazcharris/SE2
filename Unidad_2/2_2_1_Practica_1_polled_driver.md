# **Práctica 2.1. Polled driver**

## **Objetivo**

Guiar al estudiante en el diseño de un driver tipo "polled driver", a través del periférico ADC del microcontrolador. 

## **Materiales**

***Hardware***

- 1 Computador.
- 1 Placa de desarrollo basada en ESP32 (cualquiera que tenga a disposición).
- 1 Cable de programación.
- 1 Protoboard.
- 1 Sensor de temperatura LM35.
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
	
2. Cuando se haya creado el proyecto, despliegue la carpeta "src", donde está el archivo "main.c" que es el archivo principal del proyecto. En este archivo se escribirá el programa. 

3. Escriba el siguiente código dentro del archivo "main.c", cuyo objetivo es: 
	1. Leer un valor analógico por el canal 0 del ADC1.
	1. Mostrar el valor leído a través del puerto serie.

	~~~c
	#include <stdio.h>              // Biblioteca estándar de C para entrada/salida
	#include "driver/gpio.h"		// Biblioteca de funciones para interactuar con los pines GPIO
	#include "driver/adc.h"			// Biblioteca de funciones para interactuar con el ADC
	#include "freertos/FreeRTOS.h"  // Biblioteca para programación multitarea en ESP32
	#include "freertos/task.h"      // Biblioteca para crear tareas en ESP32
	#include "driver/uart.h"        // Biblioteca para usar las funciones del UART del ESP32

	/* Macro para la definición de un tiempo de retardo */
	#define DELAY 	500

	/* Función principal */
	void app_main(void)
	{
		/* Se configura la resulución de lectura del ADC1. Esto aplica para todos sus canales. */
		ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));

		/* Se configura la atenuación requerida para el canal 0 del ADC1 
		* @param 1: Canal ADC1. El número de canal del ADC1. En este caso canal 0.
		* @param 2: Atenuación. El tipo de atenuación con la que trabajará el ADC.
		*           La atenuación depende del rango del voltaje usado como entrada 
		*           al canal ADC. Revisar la tabla de atenuación proporcionada en 
		*           el desarrollo del tema de clase (https://github.com/ldiazcharris/SE1/blob/main/Unidad_2/readme.md#22-integraci%C3%B3n-de-sensores-anal%C3%B3gicos). 
		*/
		ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_6));
		

		/* 
		* Se crea la variable 'val' para almacenar el valor leído en el puerto ADC1.
		* Realiza la primera lectura del ADC1 y devuelve el valor leído, usando la 
		* función adc1_get_raw(), que recibe como parámetro el el canal del puerto 
		* ADC1 que será leído.
		*/
		int val = adc1_get_raw(ADC1_CHANNEL_0);

		/* Bucle infinito para que el programa se ejecute*/
		while(1)
		{
			/*
			* Se lee continuamente del canal 0 del ADC1.
			*/
			val = adc1_get_raw(ADC1_CHANNEL_0);
			
			/**
			 * Se imprime por puerto serial la lectura tomada. 
			 * Tenga en cuenta que cada vez que se reinicia el ciclo, el valor leído 
			 * anteriormente se reemplaza por el valor de la nueva lectura. 
			*/
			printf("La lectura ADC_0 es: %d\n", val);

			vTaskDelay( DELAY / portTICK_PERIOD_MS);
		}
	}
	~~~

4. La configuración del hardware que usted debe tener es la siguiente:
	
	- Necesita mantener conectado el cable de programación al computador y a la placa.
	- Realice el siguiente circuito usando los materiales descritos en la imagen:
	
		<img src="/Unidad_2/imagenes/2.2_ejercicio_introductorio.png" width=500>

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
	

## **Práctica**

### P1. 

Desarrolle un driver o biblioteca que facilite la portabilidad de código para conectar un sensor LM35 al puerto ADC de la ESP32. 

Este driver debe ser capaz de abstraer los siguientes requerimientos:

1. **Inicialización del pin ADC donde se va a conectar el sensor** 
	
	Para esto deberá existir, por lo menos, una función llamada lm35_init() (o un nombre similar, según su criterio). Esta función debe realizar todo el proceso de inicialización. Considere los parámetros de entrada necesarios para realizar tal operación y además, considere que la función retorne valores standard o conocidos para indicar si la operación fue exitosa o no. Se deja a su discreción cuáles son dichos valores de retorno y parámetros de entrada. 

2. **Distintas instancias de sensor**

	El driver deberá permitir la creación de distintas instancias de sensores LM35 en un mismo programa. Es decir, poder configurar más de un puerto ADC para la lectura de varios sensores del mismo tipo. 

3. **Consulta del valor leído por el sensor**
	
	Para esto deberá existir, por lo menos, una función llamada lm35_read() (o un nombre similar, según su criterio). Esta función debe permitir la lectura de un sensor específico a la vez según una instancia específica. 

4. **Cálculo e interpretación en temperatura del valor leído en el ADC**
	
	El driver debe considerar las operaciones matemáticas para interpretar en forma de temperatura el valor leído por el ADC del microcontrolador. 

	considere los siguientes datos técnicos del sensor LM35:

		- Según el [*datasheet* del sensor LM35](https://www.ti.com/lit/ds/symlink/lm35.pdf), su salida es lineal a razón de 10 mV/°C, que el rango de lectura es de -55 °C hasta 150 °C. Además de su pinout.

		- Circuito de referencia para el presente ejercicio.

			<img src="/Unidad_2/imagenes/2.2_conexion_LM35.png" width=500>

			Este corresponde a la conexión básica descrita en el Tenga en cuenta que según el [*datasheet* del sensor LM35](https://www.ti.com/lit/ds/symlink/lm35.pdf) (ver página 1), cuyo rango de medición de temperatura será de 2 °C a 150 °C, como se muestra a continuación:

			<img src="/Unidad_2/imagenes/2.2_LM35_conexion_basica.png" width=500>


**Prueba del driver**

Para comprobar la funcionalidad del driver, cree un programa que implemente el driver desarrollado, para imprimir por consola el valor de temperatura leído por dos sensores LM35. 


# Referencias

- [1] https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32/api-reference/peripherals/adc.html
