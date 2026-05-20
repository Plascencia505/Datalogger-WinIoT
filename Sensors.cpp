#include "Sensors.h"
#include "RTC.h"  // Inclusión para obtener la marca de tiempo de las alertas

extern DS1307_RTC MIRTC;

// Definición del objeto global
sensores SEN;

sensores::sensores()
  : dht(PIN_DHT_INT, DHTTYPE), oneWire(PIN_DS18_EXT), ds18b20(&oneWire) {}

void sensores::config(void) {
  pinMode(PIN_LLUVIA, INPUT);

  dht.begin();
  ds18b20.begin();

  Wire.begin();

  if (!luxometro.begin()) {
    Serial.println(F("Error inicializando sensor de luz BH1750"));
  } else {
    Serial.println(F("Sensor BH1750 inicializado correctamente"));
  }
}

void sensores::leer_ambientales(void) {
  // Lectura del DHT22
  float t_int = dht.readTemperature();
  float h_int = dht.readHumidity();
  // Validar las lecturas
  if (!isnan(t_int)) temp_int = t_int;
  if (!isnan(h_int)) hum_int = h_int;

  // Lectura del DS18B20
  ds18b20.requestTemperatures();
  float t_ext = ds18b20.getTempCByIndex(0);
  // Validar que el sensor siga conectado
  if (t_ext != DEVICE_DISCONNECTED_C) temp_ext = t_ext;

  // Lectura del BH1750
  lux = luxometro.readLightLevel();

  // Lectura y mapeo del YL-83
  int valor_crudo = analogRead(PIN_LLUVIA);
  valor_crudo = constrain(valor_crudo, 0, 4095);  // Evitar valores fuera de rango
  // Invertir y mapear a un porcentaje de 0% a 100%
  porcentaje_lluvia = map(valor_crudo, 4095, 0, 0, 100);

  evaluar_alertas();
}

void sensores::evaluar_alertas(void) {
  // Alerta de Temperatura
  if (temp_int > UMB_TEMP_ALTA_INT && !warning_temp) {
    warning_temp = true;
    MIRTC.get_time();
    fecha_temp = MIRTC.format_date();
    hora_temp = MIRTC.format_time();
  }

  // Alerta de Humedad
  if (hum_int > UMB_HUMEDAD_ALTA && !warning_hum) {
    warning_hum = true;
    MIRTC.get_time();
    fecha_hum = MIRTC.format_date();
    hora_hum = MIRTC.format_time();
  }

  // Alerta de Lluvia
  if (porcentaje_lluvia >= UMB_LLUVIA_CRITICA && !warning_lluvia) {
    warning_lluvia = true;
    MIRTC.get_time();
    fecha_lluvia = MIRTC.format_date();
    hora_lluvia = MIRTC.format_time();
  }

  // Alerta de Noche
  if (lux < UMB_LUX_NOCHE && !warning_noche) {
    warning_noche = true;
    MIRTC.get_time();
    fecha_noche = MIRTC.format_date();
    hora_noche = MIRTC.format_time();
  }
}