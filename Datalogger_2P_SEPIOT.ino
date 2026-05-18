#include "Datalogger.h"   // Archivo central que integra todos los módulos del sistema

//  Configura cada uno de los módulos: comunicación serial, almacenamiento,
//  reloj, sensores, actuadores, WiFi y servidor MQTT.
void setup ( void ) {
  
  Serial.begin ( 115200 );

  // Inicialización de cada módulo con mensajes informativos
  Serial.println ( F ( "Configurando: MicroSD" ) );        MSD.MicroSD_init();
  Serial.println ( F ( "Configurando: Reloj DS1307" ) );   MIRTC.RTC_init();   
  Serial.println ( F ( "Configurando: Sensores" ) );       SEN.config();
  Serial.println ( F ( "Configurando: Actuadores" ) );     ACT.config();
  Serial.println ( F ( "Configurando: WiFi" ) );           mqtt.setup_WiFi();
  Serial.println ( F ( "Configurando: Servidor MQTT" ) );  mqtt.set_MQTT_server();
  Serial.println ( F ( "Configurando: Receptor MQTT" ) );  mqtt.set_MQTT_callback();
}

void loop ( void ) {

  TSK.actualizar_tareas();     // Actualizar tiempo actual con millis()
  TSK.tarea_lcd();             // Mostrar información en la pantalla LCD
  //TSK.tarea_sen();           // (Opcional) Lectura periódica de sensores
  TSK.tarea_buzzwarning();     // Verificar advertencias y activar buzzer si es necesario
  TSK.tarea_mqtt();            // Publicar datos en el servidor MQTT
  TSK.tarea_amb();
  mqtt.reconnect_MQTT();       // Mantener conexión con el servidor MQTT
}
