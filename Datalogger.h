#ifndef Datalogger_h
#define Datalogger_h

#include "MicroSD.h"
MicroSD MSD;  // Objeto para manejo de archivos en MicroSD

#include "RTC.h"
DS1307_RTC MIRTC;  // Objeto para manejo del reloj en tiempo real

#include <ArduinoJson.h>  // Librería para crear y manipular objetos JSON

#include "MQTT.h"
MQTT mqtt;  // Objeto para comunicación con Broker MQTT

#include "Sensors.h"
sensores SEN;  // Objeto para lectura de sensores

#include "Actuators.h"
actuadores ACT;  // Objeto para control de actuadores

#include "JSONS.h"
JSON_Factory JSON_fac;  // Objeto para creación de mensajes JSON

#include "Tasks.h"
millis_tasks TSK;  // Objeto para gestión de tareas periódicas

#endif
