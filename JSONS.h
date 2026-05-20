#ifndef JSONS_H
#define JSONS_H

#include <ArduinoJson.h>

class JSON_Factory {
public:
  // Método actualizado con los parámetros de la ventana inteligente
  String make_json(
    String hora,
    String fecha,
    uint16_t lux,
    float temp,
    float hum,
    bool warn_lluvia,
    bool warn_temp,
    bool warn_hum);
};

extern JSON_Factory JSON_fac;

#endif