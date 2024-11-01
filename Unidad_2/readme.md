# Unidad 2 - Bibliotecas y Protocolos de comunicación

## Índice

- [2.1. Creación de bibliotecas o drivers para sensores.](#21-creación-de-bibliotecas-o-drivers-para-sensores)
- [2.2. Práctica de creación de bibliotecas o drivers.](#22-práctica-de-creación-de-bibliotecas-o-drivers)
- [2.3. Protocolos de comunicación digital ($I^2C$ y SPI).](#23-protocolos-de-comunicación-digital-i2c-y-spi)
	- [2.3.1. Protocolo $I^2C$ .](#231-protocolo-i2c)
	- [2.3.2. Protocolo SPI.](#232-protocolo-spi)
- [2.4. Protocolos de comunicación inalámbrica (Bluetooth y WiFi).](#24-protocolos-de-comunicación-inalámbrica-bluetooth-y-wifi)
- [2.5. Programación de firmware del proyecto de aula.](#25-prototipo-funcional-del-proyecto-de-aula)
- [2.6. Programación de firmware del proyecto de aula.](#26-programación-de-firmware-del-proyecto-de-aula)


## 2.1. Creación de bibliotecas o drivers para sensores

En el mundo de los sistemas embebidos, es indispensable la inicialización y gestión de los dispositivos de hardware como sensores o actuadores. Para lograr esto, se recurre al software especializado conocido como <<controladores de dispositivo>> o *device drivers*. Estos programas son la columna vertebral que permite la interacción directa entre la capa de aplicación en el controlador principal (SoC, microcontrolador, etc.) y los dispositivos de hardware externos (sensores, actuadores, memorias, etc.).

Por lo anterior, en prácticamente todos los sistemas embebidos se requiere de la implementación de *device drivers* para garantizar el funcionamiento correcto y eficaz de una aplicación concreta.

Un *device driver* es un programa de software (firmware) que permite que una aplicación principal interactúe con un dispositivo de hardware específico. Un *device driver* proporciona una interfaz de software entre la aplicación principal y el hardware del dispositivo; es decir, proporciona las funciones necesarias para interactuar de manera correcta. 

Típicamente un *device driver* se encarga de las tareas de: inicialización, acceso a funciones, acceso a memoria, manejo de las interrupciones, gestión del flujo de datos y proporcionar una interfaz coherente y uniforme para el acceso al dispositivo.

A continuación, se presenta un diagrama de componentes y de capas:

<img src="imagenes/2_1_device_driver_component_diagram.png" width="600">

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
	- Memorias: $I^2C$ / SPI externa, RAM externa, FLASH externa.
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

[2.2.2 Practica 1 Polled driver](./2_2_1_Practica_1_polled_driver.md).

[2.2.2 Practica 2 Interrupt driven driver](./2_2_2_Practica_2_interrupt_driver.md).


## 2.3. Protocolos de comunicación digital ($I^2C$ y SPI)

### 2.3.1. Protocolo $I^2C$ 

El acrónimo $I^2C$ significa *Inter Integrated Circuit*. Este protocolo está diseñado específicamente para llevar a cabo comunicación digital entre circuitos integrados. Este protocolo de comunicación simplifica las conexiones necesarias para interconectar y comunicar varios dispositivos a la vez.

El protocolo $I^2C$ fue desarrollado por *Philips Semiconductors* (hoy *NXP Semiconductors*) entre 1980 y 1982. Hoy en día muchos fabricantes de circuitos integrados (como microcontroladores) ofrecen dispositivos que implementan un bus $I^2C$ para control o comunicaciones. 

Este protocolo de comunicación tiene la siguientes características:

1. Es Síncrono. 
2. Permite la comunicación Half-duplex. 
3. Permite una topología de bus (multipunto). 
4. Permite una topología multi-maestro.
5. Un mismo dispositivo puede enviar y recibir datos. 
6. Establece un mecanismo de adaptación de velocidad. 
7. La velocidad de transmisión típica es:
	- Standard: hasta 100 Kbits/s.
	- Fast: hasta 400 Kbits/s.
	- High-speed: hasta 3,4 Mbits/s
8. Cada dispositivo del bus tiene una dirección única.
	- 7 bits, $I^2C$ estándar.
	- 11 bits, $I^2C$ mejorado.
9. Distancia y número de dispositivos:
	- Limitado por la capacitancia del bus (inferior a 400pF). Normalmente 2 o 3 metros.
	- Drivers para largas distancias (centenares de metros).
10. El protocolo requiere que cada byte de información sea confirmado (ACK) por el destinatario.
11. Se pueden conectar al bus 128 dispositivos. Sin embargo, para la versión $I^2C$ de direccionamiento con 10 bits se pueden direccionar hasta 1024 dispositivos. 
	
En este protocolo de comunicación la capa física está compuesta de dos pines principales a saber: SDA (*Serial Data line*) y SCL (*Serial Clock line*).

Además, para que exista compatibilidad eléctrica entre los dispositivos, se debe conectar el pin de GND. Por otro lado, debido a la característica particular de los pines SDA y SCL, tienen la configuración eléctrica de *open drain* u *open collector*, se deben conectar resistores del tipo *pull-up* en las líneas SDA y SCL. Este resitor puede estar en el rango de entre 2 kΩ hasta 10 kΩ. 

Cada dispositivo que se requiera conectar a un bus $I^2C$, debe hacerlo en las líneas de transmisión SDA y SCL. Todos en la misma conexión. 

<img src="imagenes/2_3_1_I2C_Colector_abierto.png" width="500">

El dispositivo maestro en un bus $I^2C$ tiene las siguientes capacidades: 
- Controla la comunicación
- Genera la señal de reloj del bus (SCL).
- Inicia y termina la comunicación.
- Direcciona a los esclavos.
- Establece el sentido de la comunicación (recepción / transmisión).


**Mini Glosario $I^2C$**

- Emisor: Dispositivo que envía datos al bus.
- Receptor: Dispositivo que recibe datos del bus.
- Maestro: Dispositivo que inicia una transferencia, genera las señales
de reloj y termina la transferencia.
- Esclavo: Dispositivo direccionado por un maestro.
- SDA: *Serial Data Line* (señal de datos).
- SCL: *Serial Clock line* (señal de reloj).
- ACK: *Acknowledgement*, (Recibido o Confirmación).
- NACK: *Negative Acknowledgement*, (No Recibido o No Confirmación).

### Protocolo de comunicación

**Condiciones básicas para la comunicación**

La transmisión en el protocolo $I^2C$ los datos viajan por SDA. Por cada bit de información es necesario un pulso de SCL y los datos sólo pueden cambiar cuando SCL está a nivel bajo.

<img src="imagenes/2_3_1_I2C_Trama.png" width="500">

*Tomada de: [[4]](#referencias)*

En el protocolo $I^2C$, la unidad básica de comunicación es el byte, por lo tanto, las transferencias de información son de 8 bits. Cada vez que se envía un byte, se requiere una respuesta de confirmación (ACK). Este ACK se da de la siguiente mantera: 

El receptor (maestro o esclavo) del mensaje mantiene SDA a nivel bajo durante un tiempo de bit. Si no lo hace, se entenderá como un NACK, (no confirmado).

<img src="imagenes/2_3_1_I2C_Trama_0.png" width="500">

*Tomada de: [[4]](#referencias)*

- **INICIO:** La transmisión la inicia el maestro, con la condición de *Start*, la cual consiste en que existe un Flanco de **bajada** en SDA con SCL a nivel alto. Cuando nadie accede al bus hay un nivel alto en SCL y SDA.

<img src="imagenes/2_3_1_I2C_Trama_1.png" width="500">

*Tomada de: [[4]](#referencias)*

- **FIN:** La transmisión la finaliza el maestro, con la condición de *Stop*, la cual consiste en que existe un Flanco de **subida** en SDA con SCL a nivel alto.

<img src="imagenes/2_3_1_I2C_Trama_2.png" width="500">

*Tomada de: [[4]](#referencias)*

**Direccionamiento en el bus**

Luego de que el maestro de la condición de inicio (*Start*), también debe enviar la siguiente información por el bus:
- Dirección del esclavo (7 bits)
- Acción a realizar: Comando de lectura o escritura en el bit R/W. Para lectura el bit será 1 y para escritura el bit será 0. 

<img src="imagenes/2_3_1_I2C_Trama_3.png" width="500">

*Tomada de: [[4]](#referencias)*

### $I^2C$ en el ESP32

La ESP32 posee dos controladores (puertos) $I^2C$, con los que se puede entablar una comunicación en el bus $I^2C$. Un solo controlador $I^2C$ puede ser un maestro o un esclavo.

La ESP32 soporta direccionamiento de 7 bits y de 10 bits. ESP32 admite tanto el modo estándar $I^2C$ (SM, *Standard Mode*) como el modo rápido (FM, *Fast Mode*), cuyas frecuencias de reloj van desde 100KHz hasta 400KHz, respectivamente [[6]](#referencias).

**Nota de ESPRESSIF:**

La frecuencia de SCL está influenciada tanto por la resistencia pull-up como por la capacitancia del cable. Por lo tanto, se recomienda encarecidamente elegir resistencias de *pull-up* apropiadas para que la frecuencia sea precisa. El valor recomendado para las resistencias *pull-up* generalmente varía de 1K ohmios a 10K ohmios [[6]](#referencias).

Tenga en cuenta que cuanto mayor sea la frecuencia, menor será la resistencia *pull-up* (pero no menor que 1 KOhms). De hecho, las resistencias grandes disminuirán la corriente, lo que aumentará el tiempo de conmutación del reloj y reducirá la frecuencia. Por lo general, recomendamos un rango de 2 KOhms a 5 KOhms, pero los usuarios también pueden necesitar hacer algunos ajustes dependiendo de sus requerimientos [[6]](#referencias).}

### Estructura de archivos de ESP-IDF para $I^2C$

<img src="imagenes/2_3_1_I2C_code_structure.png" width="500">

Fuente: Tomada de [[6]](#referencias).

`i2c.h`: El archivo de encabezado de las API I2C tipo *legacy* (controlador heredado, en desuso).

`i2c_master.h`: El archivo de encabezado que proporciona API específicas del modo de comunicación estándar para el modo *Master* (controlador con modo maestro).

`i2c_slave.h`: El archivo de encabezado que proporciona API específicas del modo de comunicación estándar par ael modo *Slave* (con modo esclavo).

### [Práctica I2C](./2_3_1_Practica_3_I2C.md)

### 2.3.2. Protocolo SPI

### Introducción a SPI

La interfaz periférica serial (SPI, del inglés *Serial Peripheral Interface*) es una interfaz de comunicación utilizada para enviar datos entre múltiples dispositivos. Estos dispositivos están organizados en una configuración de "maestro y esclavo", en la que el maestro tiene control sobre los esclavos y los esclavos reciben instrucciones del maestro. 

La implementación más común de SPI consiste en una configuración en la que un solo dispositivo es el maestro y el resto de los dispositivos son esclavos.

SPI es un protocolo de comunicación síncrono que transmite y recibe información simultáneamente con altas tasas de transferencia de datos y está diseñado para la comunicación a nivel de placa sobre distancias cortas.

La interfaz de comunicación SPI es ventajosa cuando se necesita comunicar entre múltiples dispositivos. Ofrece una velocidad de transferencia de datos más alta que muchos otros tipos de interfaces de comunicación y permite que los datos se envíen y reciban al mismo tiempo. Sin embargo, SPI también requiere más líneas de señal o cables que otros tipos de comunicación. Además, no existe un protocolo de mensaje estándar para comunicarse a través de SPI, lo que significa que cada dispositivo podría tener su propia convención para el formato de mensaje de datos.

### Conexiones SPI

Se requieren cuatro señales para implementar la comunicación SPI, a saber:

| Señal |                			    Descripción   			           |
|  ---- |                			    -----------    			           |
| MOSI  | Datos: Salida en Master (Master Out)/entrada en Slave (Slave In) |
| MISO  | Datos: Entrada en Master (Master In)/Salida en Slave (Slave Out) |
| SCLK  | Serial Clock                                                     |
| CS    | Chip Select                                                      |

Todas las líneas son controladas por el maestro excepto la línea MISO, que controlada por el esclavo. *Chip Select* (CS), a veces referido como *Slave Select* (SS), también suele ser denotado como -CS o -SS. porque un chip/esclavo en particular está activo cuando esa línea es puesta a nivel bajo por el maestro (la línea sobre la parte superior indica una señal invertida).

<img src="imagenes/2_3_2_SPI_interface.png" width="500">

<img src="imagenes/2_3_2_SPI_varios_slaves.png" width="500">

- **Señal de reloj o Serial Clock**

La señal de reloj es generada por el dispositivo maestro a una frecuencia específica y se utiliza para sincronizar los datos que se transmiten y reciben entre los dispositivos. Esta señal puede ser configurada por el maestro utilizando dos propiedades conocidas como polaridad del reloj (CPOL) y fase del reloj (CPHA). La polaridad del reloj determina la polaridad de la señal de reloj y puede configurarse para estar en reposo ya sea en bajo (0) o en alto (1). Una señal de reloj que está en reposo bajo tiene un pulso alto y un flanco ascendente, mientras que una señal de reloj que está en reposo alto tiene un pulso bajo y un flanco descendente.

La fase del reloj determina el momento en el que los datos deben ser modificados y leídos. Si la fase del reloj se establece en cero, los datos se modifican en el flanco descendente de la señal de reloj y se leen en el flanco ascendente. Por el contrario, si esta propiedad se establece en uno, los datos se cambian en el flanco ascendente de la señal de reloj y se leen en el flanco descendente. A medida que los ciclos del reloj, los datos se envían bit a bit, simultáneamente, a través de las líneas MOSI y MISO.

<img src="imagenes/2_3_2_SPI_data.jpg" width="500">

- **Señal MOSI y MISO**

En la comunicación SPI se utilizan dos líneas de datos conocidas como MOSI y MISO. La señal MOSI envía datos desde el maestro y es recibida por todos los esclavos. De manera similar, la línea de datos MISO transmite datos desde uno de los dispositivos esclavos hacia el dispositivo maestro.

- **Señal Chip select**

La señal de selección de chip es utilizada por el maestro para seleccionar con qué esclavo comunicarse. Esta línea para el esclavo específico debe ser establecida a nivel bajo cuando el maestro quiera comunicarse con el esclavo. Si se utilizan varios dispositivos esclavos en el mismo bus, entonces cada esclavo tendrá su propia línea de selección de chip dedicada, mientras comparten las líneas de reloj y datos. Cuando el maestro haya terminado de comunicarse con el esclavo, la línea de selección de chip se vuelve a establecer en nivel alto.

- **Señal Write-Protect**

Esta es una señal "opcional". La señal Write-Protect (WP) permitirá operaciones normales de lectura/escritura cuando se mantenga alto. Cuando el pin WP baja se inhiben todas las operaciones de escritura. Si WP baja mientras CS aún está bajo, se interrumpirá una operación de escritura. Si el ciclo de escritura interno ya se ha iniciado, la disminución de WP no tendrá ningún efecto en ninguna operación de escritura en el registro STATUS. La función del pin WP se bloquea cuando el bit WPEN en el registro STATUS se establece en un '0' lógico. Esto permitirá al usuario instalar el AT25512 en un sistema con el pin WP conectado a tierra y aún poder escribir en el registro de ESTADO. Todas las funciones del pin WP están habilitadas cuando el bit WPEN se establece en un "1" lógico.

### Configuración del SPI en la ESP32


| Término | Definición |
|:---:|:---:|
| Host | El   periférico del controlador SPI dentro del ESP32 inicia las transmisiones SPI   a través del bus y actúa como un maestro SPI. |
| Device | Dispositivo   esclavo SPI. Un bus SPI puede estar conectado a uno o más dispositivos. Cada   dispositivo comparte las señales MOSI, MISO y SCLK, pero solo está activo en   el bus cuando el host afirma la línea CS individual del dispositivo. |
| Bus | Un bus de   señal, común a todos los dispositivos conectados a un host. En general, un   autobús incluye las siguientes líneas: MISO, MOSI, SCLK, una o más líneas CS   y, opcionalmente, QUADWP y QUADHD. Por lo tanto, los dispositivos están   conectados a las mismas líneas, con la excepción de que cada dispositivo   tiene su propia línea CS. Varios dispositivos también pueden compartir una   línea CS si se conectan en cadena. |
| MOSI | Master   Out, Slave In, también conocido como D. Transmisión de datos desde un host a   un dispositivo. También señal data0 en modo Octal/OPI. |
| MISO | Master   In, Slave Out, también conocido como Q. Transmisión de datos desde un   dispositivo al host. También señal data1 en modo Octal/OPI. |
| SCLK | Reloj de   serie. La señal oscilante generada por un host mantiene sincronizada la   transmisión de bits de datos. |
| CS | Selección   de chip. Permite a un host seleccionar dispositivos individuales conectados   al bus para enviar o recibir datos. |
| QUADWP | Señal de   protección contra escritura. Se utiliza para transacciones de 4 bits   (qio/qout). También para la señal data2 en modo Octal/OPI. |
| QUADHD | Señal de   espera. Se utiliza para transacciones de 4 bits (qio/qout). También para la   señal data3 en modo Octal/OPI. |
| DATA4 | Señal   Data4 en modo Octal/OPI. |
| DATA5 | Señal   Data5 en modo Octal/OPI. |
| DATA6 | Señal   Data6 en modo Octal/OPI. |
| DATA7 | Señal   Data7 en modo Octal/OPI. |
| Assertion | La acción   de activar una línea. |
| De-assertion | La acción   de devolver la línea al estado inactivo (de nuevo a inactivo). |
| Transaction | Una   instancia de un host que afirma una línea CS, transfiere datos hacia y desde   un dispositivo y anula la aserción de la línea CS. Las transacciones son   atómicas, lo que significa que nunca pueden ser interrumpidas por otra   transacción. |
| Launch   Edge | Borde del   reloj en el que el registro de origen lanza la señal a la línea. |
| Latch   Edge | Borde del   reloj en el que el registro de destino se engancha   en la señal. |



**Modos de operación**

| Mode name | Command Line Width | Address Line Width | Data Line Width | Transaction Flag | Bus IO Setting Flag |
|:---:|:---:|:---:|:---:|:---:|:---:|
| Normal SPI | 1 | 1 | 1 | 0 | 0 |
| Dual Output | 1 | 1 | 2 | SPI_TRANS_MODE_DIO | SPICOMMON_BUSFLAG_DUAL |
| Dual I/O | 1 | 2 | 2 | SPI_TRANS_MODE_DIO   SPI_TRANS_MULTILINE_ADDR | SPICOMMON_BUSFLAG_DUAL |
| Quad Output | 1 | 1 | 4 | SPI_TRANS_MODE_QIO | SPICOMMON_BUSFLAG_QUAD |
| Quad I/O | 1 | 4 | 4 | SPI_TRANS_MODE_QIO   SPI_TRANS_MULTILINE_ADDR | SPICOMMON_BUSFLAG_QUAD |




## 2.4. Protocolos de comunicación inalámbrica (Bluetooth y Wifi)

### 2.4.1. Bluetooth

Bluetooth® es una tecnología y un estándar de comunicación inalámbrica de corto alcance o de área personal (WPAN, Wireless Personal Area Network). Permite que dos dispositivos se conecten directamente sin requerir una infraestructura de red compatible, como un enrutador inalámbrico o un punto de acceso. Hoy en día, la tecnología Bluetooth es más utilizada por personas de todo el mundo para conectar dispositivos como auriculares inalámbricos, teclados, ratones y altavoces a PC y dispositivos móviles [[7], [8], [9]](#referencias).

Bluetooth permite la transmisión de voz y datos entre diferentes dispositivos mediante un enlace por radiofrecuencia en la banda ISM (Industrial, Science and Medical Band) de los 2.4 GHz. Esta tecnología se elevó a estándar por el IEEE, con la denominación 802.15.1 en el año 2002. 

Las principales ventajas de Bluetooth son:

- Robustez.
- Bajo consumo.
- Bajo costo. 

El sistema Bluetooth puede dividirse en dos categorías diferentes: 

- **Bluetooth clásico**
	Conocida como *Bluetooth Basic Rate/Enhanced Data Rate* (BR/EDR), es una radio de baja potencia que transmite datos a través de 79 canales en la banda ISM sin licencia de 2,4 GHz. Compatible con la comunicación punto a punto entre dispositivos [[7]](#referencias). 
	
	Bluetooth Classic se utiliza principalmente para permitir la transmisión inalámbrica de audio y se ha convertido en el protocolo de radio estándar detrás de los altavoces inalámbricos, auriculares y sistemas de entretenimiento en el automóvil. La radio Bluetooth Classic también permite aplicaciones de transferencia de datos como mobile printing [[7]](#referencias).


- **Bluetooth Low Energy (BLE)**
	Está diseñada para un funcionamiento de muy bajo consumo. Transmite datos a través de 40 canales en la banda ISM sin licencia de 2,4 GHz. 
	
	Admite múltiples topologías de comunicación: *point-to-point*, *broadcast* y *mesh*. Aunque inicialmente era conocida por sus capacidades de comunicación entre dispositivos, BLE también se utiliza ahora como tecnología de posicionamiento de dispositivos en interiores. Además, incluye funciones para determinar la presencia, distancia y dirección de otro dispositivo [[7]](#referencias).
	
**Tabla comparativa: Bluetooth Calssic Vs Bluetooth LE**
	
|  | Bluetooth Low Energy (LE) | Bluetooth Classic |
|---|---|---|
| Frequency Band | 2.4GHz   ISM Band (2.402 – 2.480 GHz Utilized) | 2.4GHz   ISM Band (2.402 – 2.480 GHz Utilized) |
| Channels | 40 channels with 2 MHz spacing      (3 advertising channels/37 data channels) | 79   channels with 1 MHz spacing |
|  |  |  |
| Channel Usage | Frequency-Hopping   Spread Spectrum (FHSS) | Frequency-Hopping   Spread Spectrum (FHSS) |
| Modulation | GFSK | GFSK,   π/4 DQPSK, 8DPSK |
| Data Rate | LE 2M PHY: 2 Mb/s      LE 1M PHY: 1 Mb/s      LE Coded PHY (S=2): 500 Kb/s      LE Coded PHY (S=8): 125 Kb/s | BR PHY (GFSK): 1 Mb/s      EDR PHY (π/4 DQPSK): 2 Mb/s      EDR PHY (8DPSK): 3 Mb/s |
| Tx Power* | ≤   100 mW (+20 dBm) | ≤   100 mW (+20 dBm) |
| Rx Sensitivity | LE 2M PHY: ≤-70 dBm      LE 1M PHY: ≤-70 dBm      LE Coded PHY (S=2): ≤-75 dBm      LE Coded PHY (S=8): ≤-82 dBm | ≤-70   dBm |
| Data Transports | Asynchronous Connection-oriented      Isochronous Connection-oriented      Asynchronous Connectionless      Synchronous Connectionless      Isochronous Connectionless | Asynchronous Connection-oriented      Synchronous Connection-oriented |
| Communication Topologies | Point-to-Point (including   piconet)      Broadcast      Mesh | Point-to-Point   (including piconet) |
| Positioning Features | Presence: Advertising      Direction: Direction Finding (AoA/AoD)      Distance: RSSI, HADM (Coming) | None |

**Overview**

<img src="imagenes/2_4_1_Bluetooth_Technology_Overview_Graphic.png" width="800">


**Bluetooth en la ESP32**

ESP32 implementa el stack Bluetooth de Android. Admite Bluetooth de modo dual, lo que significa que tanto Bluetooth clásico como Bluetooth LE son compatibles con ESP32 [[9]](#referencias).

El *stack* del protocolo Bluetooth en ESP32 puede dividirse en dos partes: un "Controller Stack" y un "Host Stack" [[9]](#referencias). 

El *stack* del controlador contiene el Physical Layer (PHY), Baseband, Link Controller, Link Manager, Device Manager, Host Controller Interface (HCI) y otros módulos, y se utiliza para la gestión de la interfaz de hardware y la gestión de enlaces [[9]](#referencias). 

El *stack* del host contiene L2CAP, SMP, SDP, ATT, GATT (Generic Attribute), GAP (Generic Access Profile) y varios perfiles, y funciona como interfaz para la capa de aplicación, facilitando así a ésta el acceso al sistema Bluetooth. El Host Bluetooth puede implementarse en el mismo dispositivo que el Controlador, o en dispositivos diferentes. ESP32 admite ambos enfoques. A continuación, se describen las estructuras típicas de aplicación:

<img src="imagenes/2_4_1_Architecture_Bluetooth_host_and_controller.png" width="700">

Tomado de: [[9]](#referencias)

	**Nota: BlueDroid** es la implementación del stack Bluetooth de Android utilizada por terceros cuando desean utilizar el soporte Bluetooth nativo de Android.

1. **Escenario 1 (Configuración ESP-IDF por defecto):** Se selecciona BLUEDROID como Host Bluetooth y se utiliza VHCI (interfaz HCI virtual implementada por software) para la comunicación entre el Host Bluetooth y el Controlador. En este escenario, tanto el BLUEDROID como el Controlador se implementan en el mismo dispositivo (es decir, el ESP32), eliminando la necesidad de un PC adicional u otros dispositivos host que ejecuten el Host Bluetooth [[9]](#referencias).

2. **Escenario 2:** El sistema ESP32 se utiliza sólo como un controlador Bluetooth (módulo de hardware separado), para un dispositivo adicional Bluetooth (como un PC Linux con BlueZ o un dispositivo Android con un dispositivo Android con BLUEDROID, etc). En este caso, el controlador y el host implementados en dispositivos diferentes, lo que es bastante similar al caso de los teléfonos móviles, PADs o PCs [[9]](#referencias).

3. **Escenario 3:** Este escenario es similar al escenario 2. La diferencia radica en que, en la prueba del controlador BQB (Bluetooth Qualification Body Radio Frequency Test, u otras certificaciones) el ESP32 se puede probar conectándolo a las herramientas de prueba, con la UART habilitada como interfaz IO [[9]](#referencias).

	**Nota:** *La certificación Bluetooth® de un producto demuestra que dicho producto cumple todas las especificaciones de la tecnología Bluetooth® y es capaz de establecer conexiones adecuadas con otros dispositivos habilitados para Bluetooth® [[7]](#referencias).*


El controlador del protocolo es invisible para las aplicaciones de usuario y se ocupa de las capas inferiores del *stack* BLE. La configuración del controlador incluye establecer:

- El tamaño del stack del controlador BT.
- La prioridad.
- La velocidad de transmisión HCI.

<img src="imagenes/2_4_1_Bluedroid_ESP32_components.png" width="600">

### Bluetooth LE GAP APIs

El GAP (perfil de acceso genérico) define el proceso de descubrimiento, la gestión de dispositivos y el establecimiento de una conexión entre dispositivos Bluetooth LE. y el establecimiento de la conexión entre dispositivos Bluetooth LE [[9]](#referencias).

El Bluetooth LE GAP se implementa en forma de llamadas a la API y devoluciones de eventos. El resultado del procesamiento de las llamadas API en la pila del protocolo se devuelve mediante Eventos. Cuando un dispositivo inicia una solicitud, el estado de dicho dispositivo también se devuelve mediante un evento. 
Hay cuatro roles GAP definidos para un dispositivo Bluetooth LE:

- **Broadcaster**: Un broadcaster es un dispositivo que envía paquetes de publicidad (advertising packets), para que pueda ser descubierto por los observadores. Este dispositivo sólo puede anunciarse, pero no puede conectarse.

- **Observer**: Un observador es un dispositivo que escanea broadcasters y envía esta información a una aplicación. Este dispositivo sólo puede enviar solicitudes de exploración, pero no puede conectarse.

- **Peripheral**: Un periférico es un dispositivo que se anuncia mediante *connectable advertising packets* y se convierte en esclavo una vez que se conecta.

- **Central**: Una central es un dispositivo que inicia las conexiones con los periféricos y se convierte en maestro una vez que se establece un enlace físico.

**Estados del GAP**

<img src="imagenes/2_4_1_estados_del_rol_GAP.png" width="600">


**Procedimiento de broadcast**



Se definen cinco modos para las emisiones Bluetooth LE:

1. **Connectable Scannable Undirected mode:** En este modo, un dispositivo puede ser descubierto por y conectado a cualquier dispositivo. La escaneabilidad indica que el dispositivo local debe responder con una respuesta de escaneo cuando otro dispositivo envía una solicitud de escaneo.

2. **High Duty Cycle Directed mode y Connectable Low Duty Cycle Directed mode:** En estos modos, las difusiones dirigidas IP sólo pueden ser descubiertas por los dispositivos designados y conectarse a ellos. El paquete de "broadcast dirigido de alto ciclo de trabajo" incluye 6 bytes de la dirección del dispositivo de difusión y 6 bytes de la dirección del dispositivo receptor. En este modo, se ignoran los parámetros de difusión adv_int_min y adv_int_max.

En el modo Conectable de bajo ciclo de trabajo dirigido, los parámetros de difusión adv_int_min y adv_int_max deben ser superiores a 100 ms (0xA0).

3. **Scannable Undirected mode:** En este modo un dispositivo puede ser descubierto por cualquier otro dispositivo, pero no puede conectarse a él. Un paquete Scannable Undirected incluye 6 bytes de una dirección de difusión y 0~31 bytes de los datos del paquete de difusión, que es la misma que en el paquete Scannable Undirected Conectable. 

4. **Nonconnectable Undirected mode:** En este modo, un dispositivo puede ser descubierto por cualquier dispositivo, pero no puede ser escaneado ni conectado a ningún dispositivo. Un dispositivo no-escaneable es aquel que no responderá con una respuesta de escaneo, cuando un dispositivo par envía una solicitud de escaneo. No-conectable significa que no puede conectarse a ningún dispositivo.
El paquete incluye 6 bytes de dirección de difusión y 0~31 bytes de datos del paquete de difusión. En este modo, un dispositivo puede ser descubierto pero no puede ser escaneado ni conectado por otros dispositivos.


### Modelo Cliente-Servidor

El dispositivo que contiene los datos (es decir, los atributos) se define como servidor, y el dispositivo que obtiene los datos del servidor se define como cliente. A continuación se enumeran las operaciones comunes entre un servidor y un cliente:

- Un cliente envía datos a un servidor escribiendo datos en el servidor. Tanto la solicitud de escritura como el comando de escritura pueden utilizarse para escribir un valor de atributo. Sin embargo, sólo se solicita una respuesta de escritura cuando se utiliza una solicitud de escritura.

- Un servidor envía datos a un cliente enviándole una Indicación o Notificación. La única diferencia entre una indicación y una notificación es que la confirmación sólo se solicita cuando se utiliza una indicación. Esto es similar a la diferencia entre una solicitud de escritura y un comando de escritura.

<img src="imagenes/2_4_1_Client_server_relation.png" width="500">

### Aplicación Genérica de Bluetooth con ESP32 

Para crear una aplicación de Bluetooth con ESP32 se deben seguir los siguientes pasos: 

***Nota:*** los siguientes pasos están adaptados para su uso en Platformio con ESP-IDF.

1. Crear un proyecto en Platformio con ESP-IDF. 

2. Configurar el entorno de desarrollo Platformio con ESP-IDF para habilitar el controlador Bluetooth en los archivos de compilación del proyecto. Se debe abrir el archivo `sdkconfig.*` que se crea automáticamente con el proyecto y se encuentra la raiz de los archivos de la carpeta del proyecto. 

<img src="imagenes/2_4_1_sdkconfig_file.png" width="400">

En este archivo debe buscar la sección `# Bluetooth` y escribir debajo de ella lo siguiente:

~~~
CONFIG_BT_ENABLED=y
CONFIG_BT_BLUEDROID_ENABLED=y
CONFIG_BT_CONTROLLER_ENABLED=y
~~~

Guarde y cierre el archivo. 

3. Iniciar la codificación de la aplicación de acuerdo con la documentación de Espressif. Existen dos tipos de aplicación dependiendo del tipo de dispositivo a configurar: Cliente y Servidor. 

4. **Para el dispositivo cliente**

4.1. En el lado del cliente, se debe escribir el código que se encargue de conectarse al servidor BLE (otro ESP32), descubrir sus servicios y características, para realizar operaciones de lectura o escritura.

4.2. Implementa las funciones de manejo de eventos BLE para manejar la conexión, desconexión, escritura de características.

5. **Para el dispositivo servidor**

5.1. En el lado del servidor, implementa el código para ofertar las características del dispositivo.

5.2. En el lado del servidor, implementa el código para procesar las solicitudes del cliente.

Los manejadores de eventos GAP y GATT son las funciones utilizadas para capturar los eventos generados por stack BLE y ejecutar funciones para configurar parámetros de la aplicación. Además, los manejadores de eventos también se utilizan para manejar eventos de lectura y escritura procedentes de la central. 

En ambos casos, en el ESP32 se deben implementar dos configuraciones importantes: 

El **manejador de eventos GAP** se encarga de escanear y conectarse a los servidores.

El **manejador de eventos GATT** gestiona los eventos que ocurren después de que el cliente se haya conectado a un servidor, como la búsqueda de servicios y la escritura y lectura de datos. 

Los manejadores de eventos GAP y GATT se registran utilizando:

~~~C
esp_ble_gap_register_callback();
esp_ble_gattc_register_callback();
~~~

### [**Práctica 2.4.1. Bluetooth***]()



### 2.4.2. Wifi

Wi-Fi, abreviatura de "Wireless Fidelity", es una tecnología de comunicación inalámbrica que permite la conexión de dispositivos a una red local (LAN) sin necesidad de cables físicos.

Wi-Fi se basa en una serie de especificaciones técnicas desarrolladas por el **Instituto de Ingenieros Eléctricos y Electrónicos (IEEE)**, que define el estándar más común el 802.11. Este estándar ha tenido múltiples revisiones a lo largo del tiempo para mejorar la velocidad, el alcance y la seguridad de la conexión inalámbrica.

La estructura básica de una red Wi-Fi consiste en un punto de acceso (access point) que actúa como un puente entre los dispositivos inalámbricos y la red cableada, como un router, y los dispositivos finales que se conectan a través de este punto de acceso, como teléfonos inteligentes, computadoras portátiles, tabletas, etc.

<img src="imagenes/2_4_2_conexion_wifi.png" width="500">

*Fuente: [[10]](#referencias)*

En cuanto a las capas OSI, el Wi-Fi implementa las capas física (Capa 1) y de enlace de datos (Capa 2). La capa física se encarga de la transmisión y recepción de las señales de radio, mientras que la capa de enlace de datos se encarga de la segmentación de los datos en tramas, el control de acceso al medio y la detección de errores.

<img src="imagenes/2_4_2_OSI_Model.gif" width="500"> 

*Fuente: [[11]](#referencias)*

Con Wi-Fi se pueden crear diferentes tipos de redes, como redes domésticas, empresariales, públicas (como las ofrecidas en cafeterías, aeropuertos, etc.), y redes de área metropolitana inalámbricas (WMAN) o redes de área amplia inalámbricas (WWAN), que tienen un alcance mayor.

**El alcance máximo de una red Wi-Fi** depende de varios factores, como la potencia de transmisión del punto de acceso, la presencia de obstáculos físicos, la interferencia de otras redes inalámbricas y dispositivos eléctricos, entre otros. Por lo general, en condiciones ideales, el alcance máximo puede ser de varios cientos de metros.

En cuanto a las características técnicas del protocolo de comunicación Wi-Fi, algunas de las más importantes incluyen:

- **Velocidad de transmisión:** Varía según el estándar Wi-Fi utilizado:
| Protocolo | Frecuencia | Velocidad de datos máxima   (en teoría) |
|---|---|---|
| 802.11ax | 2,4 o 5 GHz | 2,4 Gbps1 |
| 802.11ac wave2 | 5 GHz | 1,73 Gbps2 |
| 802.11ac wave1 | 5 GHz | 866,7 Mbps2 |
| 802.11n | 2,4 o 5 GHz | 450 Mbps3 |

*Fuente: tomado de [[12]](#referencias)*
	
- **Ancho de banda:** La cantidad de espectro de radio disponible para la transmisión de datos.
- **Frecuencia de operación:** Puede ser en la banda de 2.4 GHz y/o en la banda de 5 GHz, dependiendo del estándar y de la configuración de la red.
- **Seguridad:** Incluye protocolos de cifrado como WEP, WPA, y WPA2 para proteger la privacidad y seguridad de la red inalámbrica.
- **Calidad de servicio (QoS):** Permite priorizar ciertos tipos de tráfico de red para garantizar un rendimiento óptimo para aplicaciones sensibles al retraso, como la voz sobre IP (VoIP) o el streaming de video.

### Wifi en la ESP32

### Controlador wifi en la ESP32

- Soporte para redes Wi-Fi: El controlador Wi-Fi de la ESP32 es compatible con redes Wi-Fi 802.11 b/g/n, lo que significa que puede conectarse a una variedad de redes Wi-Fi domésticas, empresariales y públicas.

- Funcionalidad de estación y punto de acceso: La ESP32 puede actuar tanto como estación Wi-Fi, conectándose a una red existente, como punto de acceso Wi-Fi, creando su propia red a la que otros dispositivos pueden conectarse.

- Seguridad: Ofrece soporte para diferentes protocolos de seguridad Wi-Fi, incluyendo WEP, WPA/WPA2, y WPA3, lo que garantiza la seguridad de las comunicaciones inalámbricas.

- Modos de bajo consumo de energía: El controlador Wi-Fi de la ESP32 está diseñado para ser eficiente en cuanto a consumo de energía, lo que permite su uso en proyectos alimentados por batería o energía solar.

- API de programación: Se proporciona una API de programación completa para controlar y configurar el controlador Wi-Fi de la ESP32 desde el código del usuario, lo que facilita la integración de la funcionalidad Wi-Fi en proyectos.

- Soporte para funciones avanzadas: Además de la conectividad básica a redes Wi-Fi, el controlador Wi-Fi de la ESP32 también soporta funciones avanzadas como la configuración de direcciones IP estáticas o dinámicas, el escaneo de redes disponibles, y la gestión de conexiones concurrentes.

### ESP-NOW

ESP-NOW es un tipo de protocolo de comunicación Wi-Fi sin conexión definido por Espressif. En ESP-NOW, los datos de la aplicación se encapsulan en un marco de acción específico del proveedor y, a continuación, se transmiten de un dispositivo Wi-Fi a otro sin conexión *[[13]](#referencias)*.

CTR con protocolo CBC-MAC (CCMP) se utiliza para proteger el marco de acción para la seguridad. ESP-NOW es ampliamente utilizado en luz inteligente, control remoto, sensores, etc *[[13]](#referencias)*.

ESP-NOW utiliza un marco de acción específico del proveedor para transmitir datos de ESP-NOW. La velocidad de bits predeterminada de ESP-NOW es de 1 Mbps. El formato del marco de acción específico del proveedor es el siguiente *[[13]](#referencias)*:	

~~~
------------------------------------------------------------------------------------------------------------
| MAC Header | Category Code | Organization Identifier | Random Values | Vendor Specific Content |   FCS   |
------------------------------------------------------------------------------------------------------------
  24 bytes         1 byte              3 bytes               4 bytes             7-257 bytes        4 bytes
~~~

Código de categoría: El campo Código de categoría se establece en el valor (127) que indica la categoría específica del proveedor *[[13]](#referencias)*.

Identificador de la organización: El identificador de la organización contiene un identificador único (0x18fe34), que son los primeros tres bytes de la dirección MAC aplicada por Espressif *[[12]](#referencias)*.

Valor aleatorio: El valor aleatorio archivado se utiliza para evitar ataques de retransmisión *[[13]](#referencias)*.

Contenido específico del proveedor: El contenido específico del proveedor contiene campos específicos del proveedor de la siguiente manera *[[13]](#referencias)*:


~~~
-------------------------------------------------------------------------------
| Element ID | Length | Organization Identifier | Type | Version |    Body    |
-------------------------------------------------------------------------------
    1 byte     1 byte            3 bytes         1 byte   1 byte   0-250 bytes
~~~

ID de elemento: El campo ID de elemento se establece en el valor (221), lo que indica el elemento específico del proveedor.

Longitud: La longitud es la longitud total del identificador de la organización, el tipo, la versión y el cuerpo.

Identificador de la organización: El identificador de la organización contiene un identificador único (0x18fe34), que son los primeros tres bytes de la dirección MAC aplicada por Espressif.

Tipo: El campo Tipo se establece en el valor (4) que indica ESP-NOW.

Versión: El campo Versión se establece en la versión de ESP-NOW.

Cuerpo: El cuerpo contiene los datos de ESP-NOW.

Como ESP-NOW no tiene conexión, el encabezado MAC es un poco diferente al de las tramas estándar. Los bits FromDS y ToDS del campo FrameControl son 0. El primer campo de dirección se establece en la dirección de destino. El segundo campo de dirección se establece en la dirección de origen. El tercer campo de dirección se establece en dirección de difusión (0xff:0xff:0xff:0xff:0xff:0xff).

### Seguridad

ESP-NOW utiliza el método CCMP, que se describe en la norma IEEE 802.11-2012, para proteger el marco de acción específico del proveedor. El dispositivo Wi-Fi mantiene una clave maestra principal (PMK) y varias claves maestras locales (LMK). Las longitudes de PMK y LMk son de 16 bytes *[[13]](#referencias)*.

PMK se utiliza para cifrar LMK con el algoritmo AES-128. Si no se establece PMK, se utilizará un PMK predeterminado *[[13]](#referencias)*. Para configurar PMK llame a la función `esp_now_set_pmk()`.

LMK del dispositivo emparejado se utiliza para cifrar el marco de acción específico del proveedor con el método CCMP. El número máximo de LMK diferentes es seis. Si no se establece el LMK del dispositivo emparejado, el marco de acción específico del proveedor no se cifrará *[[13]](#referencias)*.

No se admite el cifrado de marcos de acción específicos del proveedor de multidifusión *[[13]](#referencias)*.


## 2.5. Prototipo funcional del proyecto de aula


## 2.6. Programación de firmware del proyecto de aula



## Referencias

- [1] BRIAN W. KERNIGHAN y DENNIS M. RITCHIE. *El lenguaje de programación C*. 2da Edición. Pearson Education. 1991. Prentice-Hall Hispanoamericana. Pág 234.  
- [2] https://learn.microsoft.com/es-es/cpp/cpp/volatile-cpp?view=msvc-170
- [3] https://trucosinformaticos.wordpress.com/2018/04/01/para-que-sirve-volatile-en-c/ 
- [4] http://arantxa.ii.uam.es/~gdrivera/labetcii/docs/I2C_alcala.pdf 
- [5] https://www.ti.com/lit/an/slva704/slva704.pdf?ts=1712031173602&ref_url=https%253A%252F%252Fwww.google.com%252F 
- [6] https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/i2c.html
- [7] https://www.bluetooth.com/learn-about-bluetooth/tech-overview/
- [8] https://www.intel.com/content/www/us/en/products/docs/wireless/what-is-bluetooth.html
- [9] Espressif Systems. ESP32 Bluetooth Architecture V1.2. Copyright © 2024. 
- [10] https://www.conceptdraw.com/examples/wireless-router-network-diagram
- [11] https://medium.com/@sreekanth.thummala/osi-model-7-layers-dataflow-example-b711dbca5eff
- [12] https://www.intel.la/content/www/xl/es/support/articles/000005725/wireless/legacy-intel-wireless-products.html
- [13] https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/network/esp_now.html
