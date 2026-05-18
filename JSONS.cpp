#include "JSONS.h"
#include "Sensor.h"  // Necesario para acceder a la clase de sensores

extern sensores SEN; // Referencia al objeto global instanciado en el archivo principal

String JSON_Factory::make_json(
  String hora, 
  String fecha, 
  int pot, 
  float temp, 
  float hum, 
  bool warn_pot, 
  bool warn_temp,
  bool warn_hum
) {
  JsonDocument doc;
  String json_str = "";

  // Asignar valores principales
  doc["fecha"] = fecha;
  doc["hora"] = hora;
  doc["potenciometro"] = pot;

  doc["temperatura"] = temp;
  doc["humedad"] = hum;

  // CORRECCIÓN CRÍTICA: Se integró warn_hum a la condición para no perder las alertas
  if (warn_pot || warn_temp || warn_hum) {
    JsonObject advertencia = doc["advertencia"].to<JsonObject>();
    
    // Anidar advertencia del potenciómetro
    if (warn_pot) {
      JsonObject advertencia_pot = advertencia["potenciometro"].to<JsonObject>();
      advertencia_pot["fecha"] = SEN.fecha_pot; 
      advertencia_pot["hora"] = SEN.hora_pot;   
    }

    // Anidar advertencia de la temperatura
    if (warn_temp) {
      JsonObject advertencia_temp = advertencia["temperatura"].to<JsonObject>();
      advertencia_temp["fecha"] = SEN.fecha_temp; 
      advertencia_temp["hora"] = SEN.hora_temp;   
    }

    // Anidar advertencia de la humedad
    if (warn_hum) {
      JsonObject advertencia_hum = advertencia["humedad"].to<JsonObject>();
      advertencia_hum["fecha"] = SEN.fecha_hum; 
      advertencia_hum["hora"] = SEN.hora_hum;  
    }
  }

  // Serializar el documento JSON y guardarlo en la variable
  serializeJson(doc, json_str);

  // Imprimir por el monitor serie para depuración
  Serial.print("JSON Generado: ");
  Serial.println(json_str);

  return json_str;
}