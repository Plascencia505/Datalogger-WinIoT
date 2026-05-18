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
    
    // EVALUACIÓN NOCTURNA CRUZADA
    // Noche física + Noche lógica
    if (SEN.warning_noche && (MIRTC.hora >= 18 || MIRTC.hora < 6)) {
      if (!is_lock) {
        Serial.println(F(">> [GLOBAL] Noche confirmada por RTC -> Bloqueo Activo"));
        is_lock = true;
      }
    } else if (SEN.lux > 450 && (MIRTC.hora >= 6 && MIRTC.hora < 18)) {
      // Liberar el bloqueo si es de día
      is_lock = false;
      SEN.warning_noche = false; 
    }

    // JERARQUÍA DE CONTROL
    
    // LLUVIA (Suprema y Crítica)
    if (SEN.warning_lluvia) {
      if (ACT.ventana_abierta) {
        Serial.println(F(">> [ALERTA] Lluvia -> CERRAR DE EMERGENCIA"));
        ACT.cerrar_ventana();
      }
    }
    // BLOQUEO (Noche)
    else if (is_lock) {
      if (ACT.ventana_abierta) {
        ACT.cerrar_ventana();
      }
    }
    // MODO MANUAL (Obedece al Botón)
    else if (modo_manual) {
      if (comando_manual_abrir && !ACT.ventana_abierta) {
        ACT.abrir_ventana();
      } else if (!comando_manual_abrir && ACT.ventana_abierta) {
        ACT.cerrar_ventana();
      }
    }
    // MODO AUTOMÁTICO (Clima)
    else {
      // Histéresis: Solo abrir si el exterior es notablemente más fresco
      if (SEN.temp_ext < (SEN.temp_int - UMB_TEMP_MARGIN_ABRIR) && !ACT.ventana_abierta) {
        Serial.println(F(">> [AUTO] Clima favorable -> ABRIR"));
        ACT.abrir_ventana();
      } 
      // Cerrar si afuera hace más calor que adentro
      else if (SEN.temp_ext > SEN.temp_int && ACT.ventana_abierta) {
        Serial.println(F(">> [AUTO] Calor exterior -> CERRAR"));
        ACT.cerrar_ventana();
      }
    }

    // CONTROL DEL VENTILADOR
    bool requerir_ventilador = false;
    
    // Si la humedad o temperatura interior son muy altas
    if (SEN.hum_int > UMB_HUMEDAD_ALTA || SEN.temp_int > UMB_TEMP_ALTA_INT) {
      requerir_ventilador = true;
    }
    // Si está cerrado y empieza a sofocar
    if (!ACT.ventana_abierta && SEN.temp_int > UMB_TEMP_SOFOCO) {
      requerir_ventilador = true;
    }

    // Aplicar estado físico (PWM a velocidad máxima 255)
    if (requerir_ventilador && !ACT.ventilador_encendido) {
      ACT.control_ventilador(true, 255);
    } else if (!requerir_ventilador && ACT.ventilador_encendido) {
      ACT.control_ventilador(false);
    }

    t_ant_logica = t_actual;
  }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~ 3.- BOTÓN ~~~~~~~~~~~~~~~~~~~~~~~~ */
void millis_tasks::tarea_boton(void) {
  if ((t_actual - t_ant_boton) >= TIEMPO_BOTON) {
    bool lectura_actual = digitalRead(PIN_BTN);
    
    // Detectar flanco de bajada (Pulsación, asumiendo PULLUP)
    if (lectura_actual == LOW && estado_btn_anterior == HIGH) {
      // Bloquear acciones manuales si hay lluvia o está bloqueado
      if (!SEN.warning_lluvia && !is_lock) {
        modo_manual = true; // Forzamos el control manual temporal
        comando_manual_abrir = !ACT.ventana_abierta; // Invertir estado actual
        Serial.println(comando_manual_abrir ? F(">> [BTN] Petición Manual: ABRIR") : F(">> [BTN] Petición Manual: CERRAR"));
      } else {
        Serial.println(F(">> [BTN] Acción denegada: Sistema bloqueado o lloviendo"));
      }
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
      ACT.ventilador_encendido
    );
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
      SEN.warning_hum
    ));
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
      SEN.warning_hum
    );

    mqtt.publish_MQTT(json_str);

    // Limpiar banderas de alerta una vez transmitidas
    if (SEN.warning_temp) SEN.warning_temp = false;
    if (SEN.warning_hum) SEN.warning_hum = false;
    if (SEN.warning_lluvia && SEN.porcentaje_lluvia < UMB_LLUVIA_CRITICA) SEN.warning_lluvia = false; 

    t_ant_mqtt = t_actual;
  }
}