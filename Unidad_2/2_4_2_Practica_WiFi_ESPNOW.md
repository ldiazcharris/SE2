
# **Práctica 2.4.2. El protocolo WIFI: ESP-NOW**

## **Objetivo**

1. Guiar al estudiante en la configuración y uso del controlador WIFI.
2. Guiar al estudiante en la configuración y uso del protocolo ESP-NOW.


## **Materiales**

***Hardware***

- 1 Computador.
- 2 Placas de desarrollo basada en ESP32 (cualquiera que tenga a disposición). 
- 1 Cable de programación.
- 1 Protoboard.
- 10 cables dupont macho-macho.
- 3 LED's
- 1 Resistencia LDR (fotorresistencia).
- 1 Sensor DHT11 o DHT22.
- 1 Pantalla LCD 16x2 con I2C. 

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

1. Inicie el programa VSCode y cree dos nuevos proyectos con la herramienta platformIO siguiendo los pasos de la sección [Crear un nuevo proyecto con platformIO](/Unidad_1/0_nuevo_proyecto.md).
	
2. Cuando se haya creado los proyectos, despliegue la carpeta `src`, donde está el archivo "main.c" que es el archivo principal del proyecto. En este archivo se escribirá los programas que se indican en los pasos subsiguientes. El objetivo de esto es: 
	1. Transmitir desde una placa ESP32 (*Iniciator*) a otra (*Responder*) a través del protocolo ESP-NOW, una trama de datos.
	1. El *Responder* recibe a través de ESP-NOW, se imprime por UART la trama de datos y se envía como respuesta otra trama de datos al *Iniciator*. 
	1. El *Iniciator* recibe esta trama y la imprime por UART.
	
		**Nota:** Para poder transmitir a través de ESP-NOW es necesario que se conozca la dirección MAC del dispositivo al cual se desea transmitir. Por lo tanto, se requiere conocer con anticipación la MAC de cada dispositivo. Si no conoce este dato, puede ejecutar el ejemplo que se encuentra en el siguiente enlace: [Ejemplo ESP-NOW: ¿Cómo obtener la MAC?](ESP_NOW/ejemplo_obtener_MAC.md). 
	
	
	***Código para el Iniciator:*** Copie el código que se encuentra en el siguiente archivo: [esp_now_initiator_code](/Unidad_2/ESP_NOW/esp_now_initiator_code.c)
	
	***Código para el Responder:*** Copie el código que se encuentra en el siguiente archivo: [esp_now_responder_code](/Unidad_2/ESP_NOW/esp_now_responder_code.c)
	
	**Nota:** Las direcciones MAC que aparecen en los ejemplos no correspnden a las des su laca, por lo tanto debe conocer las de sus propias placas. 
	
Considere que para enviar los datos desde el *Iniciator* se hace el siguiente función `esp_now_send()`, para lo cual, necesita la dirección MAC del dispositivo al cual desea transmitir. 

De la misma manera, el *Responder*, puede hacer uso de la misma función (`esp_now_send()`) para dar una respuesta al *Iniciator*. Para que esto se haga de manera ordenada, se recomienda incluir la respuesta dentro de la función `esp_now_recv_cb()`, además de incluir la dirección MAC del *Iniciator*.


4. La configuración del hardware que usted debe realizar es la siguiente:
	
	- Necesita mantener conectado el cable de programación al computador y a cada una de las placas.


5. En la parte inferior de Visual Studio Code hay una serie de botones, se describen los más relevantes:

	<img src="/Unidad_1/imagenes/1.10_barra_inferior_platformIO.png" width=500>

	*Barra de herramientas de PlatformIO.*

	1. *"Build"*. Compilar cada proyecto.
	1. *"Upload"*. Cargar cada proyecto a una placa distinta (Una al *Iniciator* y otra al *Responder*).
	1. *"Serial monitor"*. Abrir un monitor serial para cada placa por separado.


6. Compile el proyecto usando el botón *"Build"*. La primera vez puede ser demorado ya que crea todos los archivos del proyecto. Si todo es correcto, se obtiene un mensaje similar al siguiente:

	<img src="/Unidad_1/imagenes/1.10_pront_build_programa.png" width=500>
	
7. Luego dar clic en *"Upload"* para subir el programa a la placa Si todo es correcto, se obtiene un mensaje similar al siguiente:

	<img src="/Unidad_1/imagenes/1.10_pront_carga_programa.png" width=500>

8. **Abra el puerto UART de cada placa ESP32 (*Iniciator* y *Responder*) en dos ventanas del monitor serial.**

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

- Posteriormente, de clic en el botón Open del panel "Serial". Esto abrirá el puerto serie y podrá transmitir y recibir con las placas ESP32 a través de comunicación UART.

Del lado del *Responder* deberá recibir la expresión "Hola Mundo".
	

## **Práctica**

Desarrolle un sistema domótico que permita: 

	1. Controlar de forma remota la apertura/cierre de una persiana. 
	1. Monitorear la temperatura y humedad de una habitación.
	1. Monitorear la cantidad de luz que incide en la ventana de dicha habitación. 

***Considere lo siguiente:***
	
1. Debe utilizar dos ESP32, una fungirá como controlador de la persiana y como unidad de monitoreo de las variables solicitadas anteriormente, y se llamará *Servidor*; y la otra fungirá como un control remoto para el usuario y se llamará *Cliente*. 

1. Como base, debe crear un módulo en un archivo separado con las funciones de ESP-NOW con el objetivo de separarlas de la aplicación que usted va a desarrollar. 

1. En el lado del *Servidor*, debe programar tres tareas:

	- Tarea 1: esta se encarga de monitorear la cantidad de luz que incide sobre la ventana a través de la lectura de una LDR, si hay luz entonces debe abrir la persiana si no hay luz, entonces debe cerrarla. Además, esta terea debe transmitir por ESP-NOW al cliente el estado de la persiana, así como la intensidad de luz. 
	
	- Tarea 2: esta se encarga monitorear la temperatura y humedad del recinto. Además, esta terea debe transmitir por ESP-NOW al cliente el estado de las dos variables.
	
	- Tarea 3: Si el *Servidor* recibe el comando de "Abrir_Persina" desde el *Cliente*, este la abrirá, si por el contrario, si recibe "Cerrar_Persina", entonces la cerrará. Esta operación se simulará con el encendido y apagado de un LED. El LED apagado indica el cierre de la persiana y el LED encendido indica la apertura de esta. 

1. En el lado del *Cliente*, debe programar dos tareas:

	- Tarea 1: al recibir las variables de humedad, temperatura, intensidad de luz y estado de la persiana, el *Cliente* debe mostrarlos en una pantalla LCD. 
	
	- Tarea 2: Monitorear la pulsación de un botón. Si el botón se presiona una vez, se envía el comando "Abrir_Persina" al servidor. Si se presiona dos veces seguidas se envía el comando "Cerrar_Persina". 
	




# Referencias

- [1] https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32/api-reference/peripherals/adc.html
- [2] https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/ledc.html 
- [3] https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/peripherals/i2c.html
- [4] https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/peripherals/mcpwm.html
- [5] https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/peripherals/spi_master.html
- [6] https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/peripherals/spi_slave.html
- [7] https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/ESP-NOW/index.html 
- [8] https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/network/esp_wifi.html
- [9] https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/network/esp_now.html
- [10] https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/network/esp-wifi-mesh.html
- [11] https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/protocols/mqtt.html



