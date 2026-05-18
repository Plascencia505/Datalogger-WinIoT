#ifndef JSONS_H
#define JSONS_H

#include <ArduinoJson.h>  // Asegúrate de incluir la librería

class JSON_Factory {
public:
  // Método que construye un JSON con los parámetros recibidos y datos globales
  String make_json(
    String hora, 
    String fecha, 
    int pot, 
    float temp, 
    float hum, 
    bool warn_pot, 
    bool warn_temp,
    bool warn_hum
  );
};

#endif