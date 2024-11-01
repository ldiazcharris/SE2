
# **Práctica 2.4.1. El protocolo Bluetooht**

## **Objetivo**

Guiar al estudiante en la configuración y uso del controlador Bluetooth.

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

### **Ejercicio introductorio**

Por favor, siga la siguiente metodología.

1. Inicie el programa VSCode y cree dos nuevos proyectos con la herramienta platformIO siguiendo los pasos de la sección [Crear un nuevo proyecto con platformIO](/Unidad_1/0_nuevo_proyecto.md).
	
2. Cuando se haya creado los proyectos, despliegue la carpeta `src`, donde está el archivo "main.c" que es el archivo principal del proyecto. En este archivo se escribirá los programas que se indican en los pasos subsiguientes. El objetivo de esto es: 
	1. Transmitir desde una placa ESP32 (*Cliente*) a otra (*Servidor*) a través del protocolo Bluetooth, una trama de datos.
	1. El *Servidor* recibe a través de Bluetooth, se imprime por UART la trama de datos y se envía como respuesta otra trama de datos al *Cliente*. 
	1. El *Cliente* recibe esta trama y la imprime por UART.
	
		**Nota:** Tenga en cuenta que para que el proyecto funcione, debe configurar el entorno Platformio con ESP-IDF para habilitar el controlador Bluetooth en los archivos de compilación del proyecto. Se debe abrir el archivo `sdkconfig.*` que se crea automáticamente con el proyecto y se encuentra la raiz de los archivos de la carpeta del proyecto. 

		<img src="imagenes/2_4_1_sdkconfig_file.png" width="300">

		En este archivo debe buscar la sección `# Bluetooth` y escribir debajo de ella lo siguiente:

~~~
CONFIG_BT_ENABLED=y
CONFIG_BT_BLUEDROID_ENABLED=y
CONFIG_BT_CONTROLLER_ENABLED=y
~~~

		Guarde y cierre el archivo. 
	
	
	***Código para el Cliente:*** Copie el código que se encuentra en el siguiente archivo: [GATT_Client_code_example](GATT_BLE/GATT_Client_code_example.c)
	
	***Código para el Servidor:*** Copie el código que se encuentra en el siguiente archivo: [GATT_Server_code_example](GATT_BLE/GATT_Server_code_example.c)



4. La configuración del hardware que usted debe realizar es la siguiente:
	
	- Necesita mantener conectado el cable de programación al computador y a cada una de las placas.


5. En la parte inferior de Visual Studio Code hay una serie de botones, se describen los más relevantes:

	<img src="/Unidad_1/imagenes/1.10_barra_inferior_platformIO.png" width=500>

	*Barra de herramientas de PlatformIO.*

	1. *"Build"*. Compilar cada proyecto.
	1. *"Upload"*. Cargar cada proyecto a una placa distinta (Una al *Cliente* y otra al *Servidor*).
	1. *"Serial monitor"*. Abrir un monitor serial para cada placa por separado.


6. Compile el proyecto usando el botón *"Build"*. La primera vez puede ser demorado ya que crea todos los archivos del proyecto. Si todo es correcto, se obtiene un mensaje similar al siguiente:

	<img src="/Unidad_1/imagenes/1.10_pront_build_programa.png" width=500>
	
7. Luego dar clic en *"Upload"* para subir el programa a la placa Si todo es correcto, se obtiene un mensaje similar al siguiente:

	<img src="/Unidad_1/imagenes/1.10_pront_carga_programa.png" width=500>

8. **Abra el puerto UART de cada placa ESP32 (*Cliente* y *Servidor*) en dos ventanas del monitor serial.**

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

Del lado del *Servidor* deberá recibir la expresión "Hola Mundo" y del lado del Cliente deberá recibir "Hola ESP".
	

## **Práctica**

***Desde el código del Servidor***

De acuerdo con lo anterior, considere que para recibir los datos en el *Servidor* se hace el siguiente bloque de código:

~~~

case ESP_GATTS_WRITE_EVT: {
    ESP_LOGI(GATTS_TAG, "GATT_WRITE_EVT, conn_id %d, trans_id %" PRIu32 ", handle %d",
             param->write.conn_id, param->write.trans_id, param->write.handle);

    if (!param->write.is_prep){
        ESP_LOGI(GATTS_TAG, "Datos recibidos del Cliente, longitud %d:", param->write.len);
        esp_log_buffer_hex(GATTS_TAG, param->write.value, param->write.len);

        // Aquí se pueden procesar los datos recibidos
        // Por ejemplo, guardarlos en una variable o realizar una acción específica
    }...


~~~

la variable `param->write.value`, es la que contiene la información recibida desde el *Cliente* en un formato no especificado o previamente no conocido, pero que viene contenido en un array con elementos enteros sin signo de 8 bits. En la variable `param->write.len`, está contenida la cantidad de elementos de ese array. Se recomienda por tanto transmitir la información en este formato. 

Para dar la respuesta al *Cliente*, puede almacenar la respuesta en el array `uint8_t notify_data[15]`. Este se envía con la llamada a la función: `esp_ble_gatts_send_indicate()`. 


***Desde el código del Cliente***

Del lado del *Cliente*, la solicitudes se transmiten a través del array `uint8_t write_char_data[35]` y se envían hacia el *Servidor* con la llamada a la función: `esp_ble_gatts_send_indicate()`.

***Ejercicios***

De acuerdo con lo anterior, realice lo siguiente:

1. Cree un módulo en un archivo separado con las funciones del *Servidor* con el objetivo de separar las funciones de Bluetooth de la aplicación que usted va a desarrollar. 

2. Haga lo mismo del punto 1 con las funciones del *Cliente*. 

3. *Del lado del código del Cliente.* Utilizando las tareas de FreeRTOS, cree una tarea que detecte la pulsación de un botón, cuando detecte esta actividad, deberá solicitar al *Servidor* que le envíe los datos de temperatura ambiente. 

4. *Del lado del *Servidor*.* Al recibir la solicitud, el *Servidor* debe procesarla e identificar qué dato solicita el *Cliente*. Una vez identificado que el *Cliente* solicita el dato de temperatura ambiente, se procederá a enviarlo a través de las notificaciones. 


# Referencias

- [1] https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32/api-reference/peripherals/adc.html
- [2] https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/ledc.html 
- [3] https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/peripherals/i2c.html
- [4] https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/peripherals/mcpwm.html
- [5] https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/peripherals/spi_master.html
- [6] https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/peripherals/spi_slave.html
- [7] https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/bluetooth/index.html 
- [8] https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/network/esp_wifi.html
- [9] https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/network/esp_now.html
- [10] https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/network/esp-wifi-mesh.html
- [11] https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/protocols/mqtt.html



