#ifndef Sensors_h
#define Sensors_h

#include <Arduino.h>
#include <Wire.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <BH1750.h> 

// PINES
#define PIN_DHT_INT  16
#define DHTTYPE      DHT22
#define PIN_DS18_EXT 17
#define PIN_LLUVIA   33

// UMBRALES ESCALABLES
#define UMB_HUMEDAD_ALTA   70.0
#define UMB_TEMP_ALTA_INT  28.0
#define UMB_LUX_NOCHE      400
#define UMB_LLUVIA_CRITICA 70   

class sensores {
private:
  DHT dht;
  OneWire oneWire;
  DallasTemperature ds18b20;
  BH1750 luxometro;

public:
  sensores();

  bool warning_temp = false;
  String hora_temp = "";
  String fecha_temp = "";

  bool warning_hum = false;
  String hora_hum = "";
  String fecha_hum = "";

  bool warning_lluvia = false;
  String hora_lluvia = "";
  String fecha_lluvia = "";

  bool warning_noche = false;
  String hora_noche = "";
  String fecha_noche = "";

  float temp_int = 0.0;
  float hum_int = 0.0;
  float temp_ext = 0.0;
  uint16_t lux = 0;
  uint8_t porcentaje_lluvia = 0;

  void config(void);           
  void leer_ambientales(void); 
  void evaluar_alertas(void);  
};

// Declaración del objeto global para ser usado en otros módulos
extern sensores SEN;

#endif