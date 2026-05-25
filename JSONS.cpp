#include "JSONS.h"
#include "Sensors.h"  // Necesario para acceder a las fechas de las alertas

extern sensores SEN;  // Referencia al objeto global
JSON_Factory JSON_fac;

String JSON_Factory::make_json(
  String hora,
  String fecha,
  uint16_t lux,
  float temp,
  float hum,
  bool warn_lluvia,
  bool warn_diff,
  bool warn_hum,
  bool warn_emergencia
) {
  JsonDocument doc;
  String json_str = "";

  // Asignar valores físicos principales
  doc["fecha"] = fecha;
  doc["hora"] = hora;
  doc["luminosidad_lux"] = lux;
  doc["temperatura_int"] = temp;
  doc["humedad_int"] = hum;

  // Agregar lectura cruda de la lluvia
  doc["lluvia_pct"] = SEN.porcentaje_lluvia;

  // Si existe alguna advertencia, crear el objeto anidado
  if (warn_lluvia || warn_diff || warn_hum || warn_emergencia) {
    JsonObject advertencia = doc["advertencias"].to<JsonObject>();

    // Anidar advertencia de lluvia
    if (warn_lluvia) {
      JsonObject adv_lluvia = advertencia["lluvia"].to<JsonObject>();
      adv_lluvia["fecha"] = SEN.fecha_lluvia;
      adv_lluvia["hora"] = SEN.hora_lluvia;
    }

    // Anidar advertencia de la temperatura
    if (warn_diff) {
      JsonObject adv_diff = advertencia["clima_favorable"].to<JsonObject>();
      adv_diff["fecha"] = SEN.fecha_diff; 
      adv_diff["hora"] = SEN.hora_diff;   
    }

    // Anidar advertencia de la humedad
    if (warn_hum) {
      JsonObject adv_hum = advertencia["humedad"].to<JsonObject>();
      adv_hum["fecha"] = SEN.fecha_hum;
      adv_hum["hora"] = SEN.hora_hum;
    }

    // Anidar advertencia de emergencia
    if (warn_emergencia) {
      JsonObject adv_emg = advertencia["emergencia"].to<JsonObject>();
      adv_emg["fecha"] = SEN.fecha_emergencia; 
      adv_emg["hora"] = SEN.hora_emergencia;  
    }
  }

  // Serializar el documento JSON
  serializeJson(doc, json_str);

  // Imprimir payload para corroborar funcionamiento
  Serial.print(F("Payload para Mosquitto/SD: "));
  Serial.println(json_str);

  return json_str;
}