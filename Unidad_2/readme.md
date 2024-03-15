# Unidad 2 - Bibliotecas y Protocolos de comunicación

## Índice

- [2.1. Creación de bibliotecas o drivers para sensores.]()
- [2.2. Práctica de creación de bibliotecas o drivers.]()
- [2.3. Protocolos de comunicación digital (I2C y SPI).]()
- [2.4. Protocolos de comunicación inalámbrica (Bluetooth y WiFi).]()
- [2.5. Prototipo funcional del proyecto de aula.]()
- [2.5. Programación de firmware del proyecto de aula.]()


## 2.1. Creación de bibliotecas o drivers para sensores

En el mundo de los sistemas embebidos, es indispensable la inicialización y gestión de los dispositivos de hardware como sensores o actuadores. Para lograr esto, se recurre al software especializado conocido como <<controladores de dispositivo>> o *device drivers*. Estos programas son la columna vertebral que permite la interacción directa entre la capa de aplicación en el controlador principal (SoC, microcontrolador, etc.) y los dispositivos de hardware externos (sensores, actuadores, memorias, etc.).

Por lo anterior, en prácticamente todos los sistemas embebidos se requiere de la implementación de *device drivers* para garantizar el funcionamiento correcto y eficaz de una aplicación concreta.

Un *device driver* es un programa de software (firmware) que permite que una aplicación principal interactúe con un dispositivo de hardware específico. Un *device driver* proporciona una interfaz de software entre la aplicación principal y el hardware del dispositivo; es decir, proporciona las funciones necesarias para interactuar de manera correcta. 

Típicamente un *device driver* se encarga de las tareas de: inicialización, acceso a funciones, acceso a memoria, manejo de las interrupciones, gestión del flujo de datos y proporcionar una interfaz coherente y uniforme para el acceso al dispositivo.

A continuación, se presenta un diagrama de componentes y de capas:

<img src="imagenes/2_1_device_driver_component_diagram.png" width="500">

<img src="imagenes/2_1_device_drivers_layers.png" width="500">


**Tener en cuenta:* 

- Siempre se debe chequear los detalles del hardware para utilizar un driver de otro hardware similar.
- Nunca se debe asumir que un driver existente en el sistema embebido será compatible con un hardware particular.
- Solo se puede asumir que funcionará si es el mismo componente (100% idéntico).

### Clasificación de los device drivers

Los *device drivers* se pueden dividir en dos grupos:

1. **Específicos para la arquitectura**. Estos gestionan el hardware que está integrado en el procesador principal. Ejemplos: inicialización de memoria on-chip, MMUs y coprocesadores FPU.

2. **Genéricos**. Estos gestionan el hardware que está en el PCB, fuera del microcontrolador. En un driver genérico, usualmente se tienen porciones de código específico para la arquitectura, debido a que no se puede desasociar el microcontrolador del resto del sistema. Asimismo un driver genérico gestiona el hardware no específico del procesador, por lo que se puede construir para ser portable. Ejemplos:
	- Manejo de LCD alfanuméricos / gráficos / 7 segmentos.
	- Memorias: I2C / SPI externa, RAM externa, FLASH externa.
	- Convertidores ADC / DAC.
	- Sensores analógicos y digitales.

### Técnicas de diseño de drivers

Existen 3 técnicas de diseño de *device drivers* a saber: 

1. *Polled driver*.
2. *Interrupt driven drivers*
3. *DMA Driven drivers*

Cada una de ellas proporciona ventajas y desventajas en términos de tiempo de desarrollo, desempeño y consumo de energía del sistema. 

1. **Polled driver**
	
	Es la técnica es la más básica de diseño de drivers, que utiliza un enfoque de programación simple y directo para interactuar con los dispositivos de hardware. Consiste en que el driver desarrollado realice una consulta o *poll* del periférico dispositivo externo para saber si se puede interactuar. Por ejemplo, si se trata de un polled-driver para un puerto UART, el software verificará regularmente si hay datos disponibles para ser transmitidos o si se han recibido nuevos datos.
	
	Si bien este enfoque puede simplificar el diseño del controlador y reducir la complejidad del código, también puede resultar en un uso ineficiente del procesador, debido a que el controlador debe realizar verificaciones frecuentes del estado del dispositivo, lo que consume ciclos de CPU incluso cuando no hay actividad.
	
	Ejemplo:
	
	~~~C
	adc_start();
	while(ADC_COMPLETE_FLAG == FALSE);
		adcResults = adc_ReadAll();
	return adcResults;
	~~~
	Como puede verse en el ejemplo, la instrucción `while(ADC_COMPLETE_FLAG == FALSE)` está consultando continuamente el estado del flag `ADC_COMPLETE_FLAG`. Se puede pensar que este flag está mapeado a un bit de hardware, para saber cuándo hay datos disponibles. Una gran mayoría de las implementaciones de drivers con este enfoque, son del tipo bloqueantes (*blocking driver*), debido a que una vez invocado el driver, no retorna hasta tener un resultado. 
	
	Existen implementaciones donde la aplicación es responsable de hacer el *polling* al driver. En este caso el driver tiene funciones no bloqueantes o que realizan la consulta una vez y retornan. De esta manera, se podría considerar el driver *non-blocking*. El desarrollador del driver decide en qué parte del programa se efectúa la consulta, lo que permite a la aplicación mayor flexibilidad para realizar las consultas a una frecuencia menor. 
	
	
2. **Interrupt driven drivers**

	En esta técnica se utilizan las interrupciones del hardware para gestionar la comunicación entre el software y el hardware de manera eficiente. En lugar de que el software del controlador consulte periódicamente el estado del dispositivo, éste se activa solo cuando se produce una interrupción en el hardware del dispositivo. 
	
	Cuando ocurre una interrupción, el procesador suspende temporalmente la ejecución de la tarea actual y salta a un "manejador de interrupciones (*handler*)" específico asociado con el dispositivo. Este *handler* es una función que está diseñada para manejar la interrupción generada por el dispositivo. Siguiendo el ejemplo del puerto UART, el *el handler* podría encargarse de recibir o enviar datos a través del puerto UART.
	
	El uso de *interrupt driven drivers* permite que el procesador dedique su tiempo a otras tareas mientras se activa el dispositivo. Esto reduce el consumo de CPU y mejora la eficiencia general del sistema embebido. Además, los controladores manejados por interrupciones pueden responder de manera más rápida y oportuna a eventos del hardware en comparación con los *polled drivers*, lo que puede ser crucial en sistemas que requieren tiempos de respuesta rápidos y predecibles.
	
	En general se tienen dos tipos de drivers basados en interrupciones:
	- Orientados a eventos. Estos lanzan una interrupción cuando ocurre un evento en el periférico que debe ser atendido. Ejemplo: driver de UART que lanza una interrupción cuando se recibe un byte y está disponible en el buffer.
		
	- Según cronograma (scheduled). Estos están asociados a una base de tiempo. Ejemplo: driver de ADC que lanza una conversión periódicamente mediante un timer.

	Desarrollar drivers del *interrupt-driven driver* implica complejidades extra, como como por ejemplo:
		- Manejar la activación de las interrupciones apropiadas.
		- Mantener las interrupciones lo más cortas posible.
		- Declarar las variables que se comparten como `volatile`* [[1], [2], [3](#referencias).
		- Gestionar datos/tareas de alta prioridad y luego pasar la carga de proceso a la aplicación. 

	
3. **DMA Driven drivers**
	
	En esta técnica, para desarrollar el driver, se utiliza el Acceso Directo a Memoria (DMA, del inglés *Direct Memory Access*) para transferir datos entre dispositivos periféricos y la memoria del sistema sin la intervención directa del procesador principal.

	El DMA es un mecanismo que permite a ciertos dispositivos periféricos acceder directamente a la memoria del sistema para realizar transferencias de datos de manera más rápida y eficiente, sin requerir la intervención del procesador principal para cada operación de transferencia. Esto es especialmente útil en situaciones en las que se necesitan grandes volúmenes de datos transferidos de manera rápida y continua, como la transmisión de datos desde un dispositivo de captura de video o la lectura de datos desde un dispositivo de almacenamiento de alta velocidad.
	
	Hay que tener en cuenta que los controladores DMA son una implementación de hardware específica para un microcontrolador. Los pueden mover datos de las siguientes maneras:
		- Periférico a memoria.
		- Memoria a memoria.
		- Memoria a periférico. 

	Un driver gestionado por DMA, el controlador de dispositivo configura el DMA para que realice la transferencia de datos entre el dispositivo periférico y la memoria del sistema. Una vez configurado, el DMA puede iniciar y completar las transferencias de datos de manera autónoma, sin requerir la intervención del procesador principal. Esto libera al procesador de la carga de trabajo asociada con la transferencia de datos, lo que puede mejorar significativamente el rendimiento del sistema y permitir que el procesador se dedique a otras tareas críticas.
	
	Para que un microcontrolador pueda gestionar un driver por DMA hay que considerar: 
		- La gestión de buffers en estas aplicaciones requieren una acción constante del CPU.
		- Si por algún motivo el CPU no logra mantener el ritmo, se pierden o demoran datos.
		- Es común utilizar controladores DMA cuando se tiene preocupación sobre el *throughput** de datos. En estos casos, la utilización de controladores DMA dentro del driver es altamente deseable.
		
	Sin embargo, existe el problema de que los microcontroladores tienen un número limitado de canales DMA (a veces implementados de manera reducida) o que de plano no tienen estos controladores implementados. Por esto, no todos los drivers pueden ser escritos para utilizar DMA. Por lo tanto, el desarrollador debe seleccionar que periférico tendrá restricciones de ancho de banda para asignarle DMA.

****Notas conceptuales:*** 
	
*volatile*: es un calificador de tipos del lenguaje C, cuyo propósito es forzar a la implementación a suprimir la optimización de memoria. Por ejemplo, para una máquina con entrada/salida asignado a memoria, el apuntador a un registro de dispositivo se podría declarar como un apuntador a `volatile` para prevenir que el compilador remueva las referencias aparentemente redundantes a través del apuntador [[1]](#referencias). Revisar el ejemplo que se encuentra en [[2]](#referencias). 
	
*throughput*: es la cantidad de datos movidos exitosamente de un lugar a otro en un período de tiempo determinado. El rendimiento de la red normalmente se mide en bits por segundo (bps), como megabits por segundo (Mbps) o gigabits por segundo (Gbps).

## 2.2. Práctica de creación de bibliotecas o drivers

[2.2.2 Practica 1 Polled driver](Unidad_2/2_2_1_Practica_1_polled_driver.md).
[2.2.2 Practica 2 Interrupt driven driver](Unidad_2/2_2_2_Practica_2_interrupt_driver.md).


## 2.3. Protocolos de comunicación digital (I2C y SPI)


## 2.4. Protocolos de comunicación inalámbrica (Bluetooth y WiFi)


## 2.5. Prototipo funcional del proyecto de aula


## 2.5. Programación de firmware del proyecto de aula



## Referencias

- [1] BRIAN W. KERNIGHAN y DENNIS M. RITCHIE. *El lenguaje de programación C*. 2da Edición. Pearson Education. 1991. Prentice-Hall Hispanoamericana. Pág 234.  
- [2] https://learn.microsoft.com/es-es/cpp/cpp/volatile-cpp?view=msvc-170
- [3] https://trucosinformaticos.wordpress.com/2018/04/01/para-que-sirve-volatile-en-c/ 