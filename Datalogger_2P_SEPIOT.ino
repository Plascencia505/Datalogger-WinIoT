#include "Datalogger.h"   // Archivo central que integra todos los módulos del sistema

// Instancia global del despachador de tareas
millis_tasks TSK;

// Configura cada uno de los módulos: comunicación, almacenamiento,
// reloj, sensores, actuadores, WiFi y servidor MQTT.
void setup(void) {
  
  Serial.begin(115200);

  // Inicialización de cada módulo con mensajes informativos
  Serial.println(F("Configurando: Tareas Locales")); TSK.inicializar_tareas();
  Serial.println(F("Configurando: Reloj DS1307"));   MIRTC.RTC_init();   
  Serial.println(F("Configurando: Sensores"));       SEN.config();
  Serial.println(F("Configurando: Actuadores"));     ACT.config();
  Serial.println(F("Configurando: MicroSD"));        MSD.MicroSD_init();
  Serial.println(F("Configurando: WiFi"));           mqtt.setup_WiFi();
  Serial.println(F("Configurando: Servidor MQTT"));  mqtt.set_MQTT_server();
  Serial.println(F("Configurando: Receptor MQTT"));  mqtt.set_MQTT_callback();
}

void loop(void) {

  TSK.actualizar_tareas();     // Actualizar tiempo actual de procesamiento
  
  TSK.tarea_sensores();        // Capturar datos físicos y evaluar umbrales
  TSK.tarea_boton();           // Escuchar estado del botón de emergencia
  TSK.tarea_logica();          // Procesar jerarquía de la ventana inteligente
  
  TSK.tarea_lcd();             // Mostrar información en la pantalla OLED
  TSK.tarea_rtc();             // Sincronizar lectura del tiempo
  TSK.tarea_msd();             // Almacenar el registro localmente en la SD
  TSK.tarea_mqtt();            // Publicar datos climáticos en Mosquitto
  
  mqtt.reconnect_MQTT();       // Mantener conexión de red sin bloquear el sistema
}