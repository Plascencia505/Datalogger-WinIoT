#include "RTC.h"

// Definición del objeto global
RTC_DS1307 rtc;
DS1307_RTC MIRTC;

//  Inicializa el módulo RTC DS1307.
//  Si no se reconoce el dispositivo, muestra un error y reintenta.
//  Si el reloj no está corriendo, ajusta la fecha y hora con la de la computadora.
void DS1307_RTC::RTC_init(void) {
  uint8_t intentos = 0;

  while (!rtc.begin() && intentos < 5) {
    Serial.println(F("Hay un error de reconocimiento del RTC, revisa conexiones y alimentación"));
    Serial.flush();  // Espera a que se transmita la información serial
    delay(1000);     // Reducido a 1s para no prolongar el arranque innecesariamente
    intentos++;
  }

  if (intentos >= 5) {
    Serial.println(F("Error crítico: Límite de intentos alcanzado. Continuando sin RTC."));
    return;  // Salir para evitar evaluar isrunning() en un módulo desconectado
  }

  if (!rtc.isrunning()) {  // Si el reloj no está ejecutándose
    Serial.println(F("El reloj se está configurando con la fecha y hora de la computadora"));
    // __DATE__ y __TIME__ son macros que retornan la fecha y hora de la computadora
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Ajuste automático
  }
}

//  Obtiene la fecha y hora actuales desde el RTC y las guarda en variables.
void DS1307_RTC::get_time(void) {
  DateTime now = rtc.now();
  segundo = now.second();
  minuto = now.minute();
  hora = now.hour();
  dia = now.day();
  mes = now.month();
  ano = now.year();
}

//  Devuelve la fecha en formato legible: DD/MM/AAAA.
//  Se agregan ceros a la izquierda si el día o mes son menores a 10.
String DS1307_RTC::format_date(void) {
  String fecha = "";
  if (dia < 10) fecha += '0';
  fecha += dia;
  fecha += '/';
  if (mes < 10) fecha += '0';
  fecha += mes;
  fecha += '/';
  fecha += ano;
  return fecha;
}

//  Devuelve la hora en formato legible: HH:MM:SS.
//  Se agregan ceros a la izquierda si hora, minuto o segundo son menores a 10.
String DS1307_RTC::format_time(void) {
  String tiempo = "";
  if (hora < 10) tiempo += '0';
  tiempo += hora;
  tiempo += ':';

  if (minuto < 10) tiempo += '0';
  tiempo += minuto;
  tiempo += ':';

  if (segundo < 10) tiempo += '0';
  tiempo += segundo;
  return tiempo;
}

//  Muestra en la consola serial la hora y fecha obtenidas del RTC.
void DS1307_RTC::show_time(void) {
  Serial.print(F("Hora: "));
  Serial.println(format_time());  // Mostrar hora formateada
  Serial.print(F("Fecha: "));
  Serial.println(format_date());  // Mostrar fecha formateada
}