#include "Tasks.h"

// Inclusión de módulos
#include "Sensors.h"
#include "Actuators.h"
#include "RTC.h"
#include "MicroSD.h"
#include "JSONS.h"
#include "MQTT.h"

// Referencias a los objetos globales
extern sensores SEN;
extern actuadores ACT;
extern DS1307_RTC MIRTC;
extern MicroSD MSD;
extern JSON_Factory JSON_fac;
extern MQTT mqtt;

void millis_tasks::inicializar_tareas(void) {
  pinMode(PIN_BTN, INPUT_PULLUP);
}

void millis_tasks::actualizar_tareas(void) {
  t_actual = millis();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~ 1.- LECTURA DE SENSORES ~~~~~~~~~~~~~~~~~~~~~~~~ */
void millis_tasks::tarea_sensores(void) {
  if ((t_actual - t_ant_sensores) >= TIEMPO_SENSORES) {
    SEN.leer_ambientales();
    t_ant_sensores = t_actual;
  }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~ 2.- LÓGICA Y JERARQUÍA (EL CEREBRO) ~~~~~~~~~~~~~~~~~~~~~~~~ */
void millis_tasks::tarea_logica(void) {
  if ((t_actual - t_ant_logica) >= TIEMPO_LOGICA) {
    
    // Obtención de condiciones actuales
    bool is_raining = SEN.warning_lluvia;
    bool umbral_temp = (SEN.temp_int > UMB_TEMP_ALTA_INT);
    bool umbral_hum = (SEN.hum_int > UMB_HUMEDAD_ALTA);

    // Evaluación cruzada para la noche 
    // Comparar valor del luxómetro con la hora del RTC
    bool es_noche = SEN.warning_noche && (MIRTC.hora >= 18 || MIRTC.hora < 6);
    if (SEN.lux > 450 && (MIRTC.hora >= 6 && MIRTC.hora < 18)) es_noche = false;

    if (!emergencia) {
      if (!is_raining) {
        if (!es_noche) {
          
          // Apagar ventilador si la ventana está abierta
          if (umbral_temp) {
            if (!ACT.ventana_abierta) ACT.abrir_ventana();
            if (ACT.ventilador_encendido) ACT.control_ventilador(false);
          } else {
            if (ACT.ventana_abierta) ACT.cerrar_ventana();
            
            // Encender ventilador si la ventana está cerrada
            if (umbral_hum) {
              if (!ACT.ventilador_encendido) ACT.control_ventilador(true, 255);
            } else {
              if (ACT.ventilador_encendido) ACT.control_ventilador(false);
            }
          }

        } else {
          // Es de noche
          if (ACT.ventana_abierta) ACT.cerrar_ventana();
          // Encender ventilador si la ventana está cerrada
          if (umbral_temp || umbral_hum) {
            if (!ACT.ventilador_encendido) ACT.control_ventilador(true, 255);
          } else {
            if (ACT.ventilador_encendido) ACT.control_ventilador(false);
          }
        }

      } else {
        // Está lloviendo
        if (ACT.ventana_abierta) ACT.cerrar_ventana();
        
        // Encender ventilador si la ventana está cerrada
        if (umbral_temp || umbral_hum) {
          if (!ACT.ventilador_encendido) ACT.control_ventilador(true, 255);
        } else {
          if (ACT.ventilador_encendido) ACT.control_ventilador(false);
        }
      }

    } else {
      // Estado de Emergencia
      if (!ACT.ventana_abierta) ACT.abrir_ventana();
      
      // Apagar el ventilador
      if (ACT.ventilador_encendido) ACT.control_ventilador(false); 
    }

    t_ant_logica = t_actual;
  }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~ 3.- BOTÓN ~~~~~~~~~~~~~~~~~~~~~~~~ */
void millis_tasks::tarea_boton(void) {
  if ((t_actual - t_ant_boton) >= TIEMPO_BOTON) {
    bool lectura_actual = digitalRead(PIN_BTN);
    
    // Detectar pulsación
    if (lectura_actual == LOW && estado_btn_anterior == HIGH) {
      emergencia = !emergencia; // Alternar estado
      Serial.println(emergencia ? F(">> [BTN] EMERGENCIA ACTIVADA") : F(">> [BTN] EMERGENCIA DESACTIVADA"));
    }
    
    estado_btn_anterior = lectura_actual;
    t_ant_boton = t_actual;
  }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~ 4.- LCD ~~~~~~~~~~~~~~~~~~~~~~~~ */
void millis_tasks::tarea_lcd(void) {
  if ((t_actual - t_ant_lcd) >= TIEMPO_LCD) {
    ACT.mostrar_datos(
      SEN.temp_int,
      SEN.temp_ext,
      SEN.hum_int,
      SEN.porcentaje_lluvia,
      ACT.ventana_abierta,
      ACT.ventilador_encendido);
    t_ant_lcd = t_actual;
  }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~ 5.- RTC ~~~~~~~~~~~~~~~~~~~~~~~~ */
void millis_tasks::tarea_rtc(void) {
  if ((t_actual - t_ant_rtc) >= TIEMPO_RTC) {
    MIRTC.get_time();
    t_ant_rtc = t_actual;
  }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~ 6.- MICRO SD ~~~~~~~~~~~~~~~~~~~~~~~~ */
void millis_tasks::tarea_msd(void) {
  if ((t_actual - t_ant_msd) >= TIEMPO_MSD) {
    MSD.SaveFile(JSON_fac.make_json(
      MIRTC.format_time(),
      MIRTC.format_date(),
      SEN.lux,
      SEN.temp_int,
      SEN.hum_int,
      SEN.warning_lluvia,
      SEN.warning_temp,
      SEN.warning_hum));
    t_ant_msd = t_actual;
  }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~ 7.- MQTT ~~~~~~~~~~~~~~~~~~~~~~~~ */
void millis_tasks::tarea_mqtt(void) {
  if ((t_actual - t_ant_mqtt) >= TIEMPO_MQTT) {
    String json_str = JSON_fac.make_json(
      MIRTC.format_time(),
      MIRTC.format_date(),
      SEN.lux,
      SEN.temp_int,
      SEN.hum_int,
      SEN.warning_lluvia,
      SEN.warning_temp,
      SEN.warning_hum);

    mqtt.publish_MQTT(json_str);

    // Limpiar banderas de alerta una vez transmitidas
    if (SEN.warning_temp) SEN.warning_temp = false;
    if (SEN.warning_hum) SEN.warning_hum = false;
    if (SEN.warning_lluvia && SEN.porcentaje_lluvia < UMB_LLUVIA_CRITICA) SEN.warning_lluvia = false;

    t_ant_mqtt = t_actual;
  }
}