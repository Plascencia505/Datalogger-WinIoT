#ifndef MQTT_h
#define MQTT_h

#include <WiFi.h>              // Librería para el uso del cliente WiFi
#include "PubSubClient.h"      // Librería para el uso del protocolo MQTT

// Objetos globales declarados como externos
extern WiFiClient ESP32_WIFI;
extern PubSubClient client;

#define HOTSPOT_WIFI "MotoEdge30"         // Nombre del punto de acceso WiFi
#define HOTSPOT_PWD "internetPlease"      // Contraseña de la red WiFi
#define MQTT_SERVER "test.mosquitto.org"  // Dirección del Broker MQTT
#define MQTT_PORT 1883                    // Puerto del Broker MQTT
#define TXTOPIC "20206896/ucol/TX_6B"     // Topic de publicación
#define RXTOPIC "20206896/ucol/RX_6B"     // Topic de suscripción
//#define RXTOPIC1    "/E"                // Ejemplo de otro topic de suscripción

class MQTT {
public:
  void subscribe_MQTT(void);                         // Suscribirse a un topic MQTT
  void publish_MQTT(String);                         // Publicar un mensaje en un topic MQTT
  void reconnect_MQTT(void);                         // Reconectarse al servidor MQTT
  void setup_WiFi(void);                             // Configurar y conectar a la red WiFi
  void set_MQTT_server(void);                        // Establecer dirección y puerto del Broker MQTT
  void set_MQTT_callback(void);                      // Configurar la función callback para recibir mensajes
  static void callback(char*, byte*, unsigned int);  // Función callback para manejar mensajes recibidos
};

extern MQTT mqtt;

#endif