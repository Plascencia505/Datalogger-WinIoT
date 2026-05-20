#ifndef Tasks_h
#define Tasks_h

#include <Arduino.h>

#define PIN_BTN 14  // Botón de emergencia

// CONSTANTES LÓGICAS
#define UMB_TEMP_MARGIN_ABRIR 3.0  // Histéresis de temperatura (°C)
#define UMB_TEMP_SOFOCO 25.0       // Temperatura para encender ventilador si está cerrado

// TIEMPOS DE TAREAS (ms)
#define TIEMPO_SENSORES 2000  // Lectura ambiental cada 2s
#define TIEMPO_LOGICA 1000    // Evaluación de jerarquía cada 1s
#define TIEMPO_BOTON 50       // Polling del botón (Antirrebote)
#define TIEMPO_LCD 5000       // Actualización de pantalla cada 5s
#define TIEMPO_MSD 10000      // Guardado en SD cada 10s
#define TIEMPO_MQTT 10000     // Publicación MQTT cada 10s
#define TIEMPO_RTC 1000       // Actualización de reloj cada 1s

class millis_tasks {
public:
  // Control de tiempo exacto con uint32_t para coincidir con millis()
  uint32_t t_ant_sensores = 0,
           t_ant_logica = 0,
           t_ant_boton = 0,
           t_ant_lcd = 0,
           t_ant_msd = 0,
           t_ant_mqtt = 0,
           t_ant_rtc = 0,
           t_actual = 0;

  // Variables de Estado Global del Sistema
  bool emergencia = false;

  // Variables auxiliares para el botón
  bool estado_btn_anterior = HIGH;

public:
  void inicializar_tareas(void);  // Configura pines locales de tareas
  void actualizar_tareas(void);   // Mantiene el reloj interno

  // Métodos de tareas concurrentes
  void tarea_sensores(void);
  void tarea_logica(void);
  void tarea_boton(void);
  void tarea_lcd(void);
  void tarea_rtc(void);
  void tarea_msd(void);
  void tarea_mqtt(void);
};

#endif