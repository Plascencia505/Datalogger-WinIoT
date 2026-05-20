#include "MQTT.h"

// Definición de los objetos globales
WiFiClient ESP32_WIFI;
PubSubClient client(ESP32_WIFI);
MQTT mqtt;

void MQTT::set_MQTT_server(void) {
  client.setServer(MQTT_SERVER, MQTT_PORT);  // Configuración del Broker MQTT
}

void MQTT::set_MQTT_callback(void) {
  client.setCallback(this->callback);  // Asignar función callback
}

void MQTT::subscribe_MQTT(void) {
  client.subscribe(RXTOPIC);  // Suscripción al topic principal
  //client.subscribe ( RXTOPIC1 );               // Ejemplo de suscripción adicional
}

void MQTT::publish_MQTT(String msg) {
  Serial.println(F("Publicando información"));
  client.publish(TXTOPIC, msg.c_str());  // Publicar mensaje en el topic
}

void MQTT::reconnect_MQTT(void) {
  if (!client.connected()) {
    // Variable estática para llevar la cuenta del tiempo sin detener el ESP32
    static uint32_t ultimo_intento = 0;

    // Solo intenta reconectar si han pasado 5000 ms desde el último intento
    if (millis() - ultimo_intento > 5000) {
      ultimo_intento = millis();
      Serial.print(F("Conectando nuevamente al servidor... "));

      if (client.connect("Client_test")) {  // Intento de conexión
        Serial.println(F("Cliente conectado"));
        subscribe_MQTT();  // Suscribirse nuevamente
      } else {             // Si falla la conexión
        Serial.print(F("Falló al conectarse, tipo de error: "));
        Serial.print(client.state());  // Mostrar estado del error
        Serial.println(F(" - Intentando nuevamente en 5 segundos"));
      }
    }
  } else {
    client.loop();  // Mantener la conexión activa si está conectado
  }
}

void MQTT::setup_WiFi(void) {
  delay(10);
  Serial.println(F("Configurando WiFi: "));
  WiFi.begin(HOTSPOT_WIFI, HOTSPOT_PWD);  // Iniciar conexión WiFi

  uint8_t intentos = 0;
  // Límite de intentos para evitar que el ESP32 se congele sin red
  while (WiFi.status() != WL_CONNECTED && intentos < 20) {
    delay(500);
    Serial.print(F("."));  // Mostrar progreso
    intentos++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("\nWiFi conectado con éxito"));
  } else {
    Serial.println(F("\nAdvertencia: No se pudo conectar al WiFi. Iniciando modo fuera de línea."));
  }
}

void MQTT::callback(char* topic, byte* message, unsigned int length) {
  Serial.print(F("Ha llegado un mensaje de: "));
  Serial.println(topic);  // Mostrar el topic recibido

  String messageTemp;
  for (unsigned int i = 0; i < length; i++) {
    messageTemp += (char)message[i];  // Convertir bytes a String
  }

  Serial.println(messageTemp);  // Mostrar mensaje recibido
}