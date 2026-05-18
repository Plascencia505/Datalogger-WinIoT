#ifndef RTC_h
#define RTC_h

#include <Wire.h>  // Librería para el uso del protocolo I2C
#include "RTClib.h"

// Objeto global del tipo RTC_DS1307 declarado como externo
extern RTC_DS1307 rtc;

//  Clase DS1307_RTC
//  Contiene variables y métodos para inicializar, obtener y mostrar la hora
//  y fecha desde el módulo RTC DS1307.
class DS1307_RTC {

public:
  // Variables que almacenan la fecha y hora obtenida del RTC
  uint8_t hora = 0, minuto = 0, segundo = 0, dia = 0, mes = 0;
  uint16_t ano = 0;
  String fecha = " ";   // Cadena con la fecha formateada
  String tiempo = " ";  // Cadena con la hora formateada

public:
  void RTC_init(void);       // Inicializa el RTC
  void get_time(void);       // Obtiene la fecha y hora actuales
  String format_date(void);  // Devuelve la fecha en formato legible
  String format_time(void);  // Devuelve la hora en formato legible
  void show_time(void);      // Muestra fecha y hora en consola serial
};

#endif