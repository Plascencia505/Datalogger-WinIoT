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
  bool warn_temp,
  bool warn_hum) {
  JsonDocument doc;
  String json_str = "";

  // Asignar valores físicos principales
  doc["fecha"] = fecha;
  doc["hora"] = hora;
  doc["luminosidad_lux"] = lux;
  doc["temperatura_int"] = temp;
  doc["humedad_int"] = hum;

  // Agregamos la lectura cruda de la lluvia por si quieres graficarla
  doc["lluvia_pct"] = SEN.porcentaje_lluvia;

  // Si existe alguna advertencia, crear el objeto anidado
  if (warn_lluvia || warn_temp || warn_hum) {
    JsonObject advertencia = doc["advertencias"].to<JsonObject>();

    // Anidar advertencia de lluvia
    if (warn_lluvia) {
      JsonObject adv_lluvia = advertencia["lluvia"].to<JsonObject>();
      adv_lluvia["fecha"] = SEN.fecha_lluvia;
      adv_lluvia["hora"] = SEN.hora_lluvia;
    }

    // Anidar advertencia de la temperatura
    if (warn_temp) {
      JsonObject adv_temp = advertencia["temperatura"].to<JsonObject>();
      adv_temp["fecha"] = SEN.fecha_temp;
      adv_temp["hora"] = SEN.hora_temp;
    }

    // Anidar advertencia de la humedad
    if (warn_hum) {
      JsonObject adv_hum = advertencia["humedad"].to<JsonObject>();
      adv_hum["fecha"] = SEN.fecha_hum;
      adv_hum["hora"] = SEN.hora_hum;
    }
  }

  // Serializar el documento JSON y guardarlo en la variable String
  serializeJson(doc, json_str);

  // Imprimir por el monitor serie para depuración (opcional, puedes comentarlo en producción)
  Serial.print(F("Payload para Mosquitto/SD: "));
  Serial.println(json_str);

  return json_str;
}