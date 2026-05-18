#include "Tasks.h"

// Inclusión de dependencias para acceder a los métodos de cada módulo
#include "Sensor.h"
#include "Actuators.h"
#include "RTC.h"
#include "MicroSD.h"
#include "JSONS.h"
#include "MQTT.h"

// Referencias a los objetos globales declarados en el archivo principal
extern sensores SEN;
extern actuadores ACT;
extern DS1307_RTC MIRTC;
extern MicroSD MSD;
extern JSON_Factory JSON_fac;
extern MQTT mqtt;

/*~~~~~~~~~~~~~~~~~~~~~~~~ 1.- tarea_pot - Lectura periódica del potenciómetro ~~~~~~~~~~~~~~~~~~~~~~~~ */
void millis_tasks::tarea_pot(void) {
  if ((tiempo_actual - tiempo_anterior_pot) >= TIEMPO_POT) {
    Serial.print("1.- Ejecutando tarea pot: ");
    Serial.println(SEN.get_potV());  // Lectura del valor del potenciómetro
    tiempo_anterior_pot = tiempo_actual;
  }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~ 2.- tarea_buzz - Activación periódica del buzzer ~~~~~~~~~~~~~~~~~~~~~~~~ */
void millis_tasks::tarea_buzz(void) {
  if ((tiempo_actual - tiempo_anterior_buzz) >= TIEMPO_BUZZ) {
    Serial.println("2.- Ejecutando tarea buzzer");
    ACT.play_buzzer();  // Activar buzzer
    tiempo_anterior_buzz = tiempo_actual;
  }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~ 3.- tarea_buzzwarning - Verificación de alerta con buzzer ~~~~~~~~~~~~~~~~~~~~~~~~ */
void millis_tasks::tarea_buzzwarning(void) {
  if ((tiempo_actual - tiempo_anterior_buzzwarning) >= TIEMPO_WARNING) {
    Serial.print("3.- Ejecutando tarea buzzer warning: ");
    Serial.println(SEN.get_potV());
    tiempo_anterior_buzzwarning = tiempo_actual;

    // Si el potenciómetro supera el umbral, activar buzzer y registrar alerta
    if (SEN.get_potV() >= 1500) {
      ACT.play_buzzer();
      MIRTC.get_time();
      SEN.fecha_pot = MIRTC.format_date();
      SEN.hora_pot = MIRTC.format_time();
      SEN.warning_pot = true;
    }

    if (SEN.temperatura > 32) {
      ACT.play_buzzer();
      MIRTC.get_time();
      SEN.fecha_temp = MIRTC.format_date();
      SEN.hora_temp = MIRTC.format_time();
      SEN.warning_temp = true;
    }

    if (SEN.humedad > 80.0) {
      ACT.play_buzzer();
      MIRTC.get_time();
      SEN.fecha_hum = MIRTC.format_date();
      SEN.hora_hum = MIRTC.format_time();
      SEN.warning_hum = true;
    }
  }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~ 4.- tarea_lcd - Actualización periódica de la pantalla LCD ~~~~~~~~~~~~~~~~~~~~~~~~ */
void millis_tasks::tarea_lcd(void) {
  if ((tiempo_actual - tiempo_anterior_lcd) >= TIEMPO_LCD) {
    Serial.println("4.- Ejecutando tarea LCD");
    ACT.mostrar_datos(
      SEN.get_potV(), 
      SEN.temperatura, 
      SEN.humedad,
      SEN.warning_pot, 
      SEN.warning_temp, 
      SEN.warning_hum
    );   // Ejemplo de impresión en pantalla
    tiempo_anterior_lcd = tiempo_actual;
  }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~ 5.- tarea_rtc - Actualización periódica del reloj RTC ~~~~~~~~~~~~~~~~~~~~~~~~ */
void millis_tasks::tarea_rtc(void) {
  if ((tiempo_actual - tiempo_anterior_rtc) >= TIEMPO_RTC) {
    Serial.println("5.- Ejecutando tarea RTC");
    MIRTC.get_time();   // Obtener hora actual
    MIRTC.show_time();  // Mostrar hora en consola
    tiempo_anterior_rtc = tiempo_actual;
  }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~ 6.- actualizar_tareas - Actualiza el contador de millis() ~~~~~~~~~~~~~~~~~~~~~~~~ */
void millis_tasks::actualizar_tareas(void) {
  tiempo_actual = millis();  // Actualizar tiempo actual
}

/*~~~~~~~~~~~~~~~~~~~~~~~~ 7.- tarea_msd - Guardado periódico en microSD ~~~~~~~~~~~~~~~~~~~~~~~~ */
void millis_tasks::tarea_msd(void) {
  if ((tiempo_actual - tiempo_anterior_msd) >= TIEMPO_MSD) {
    Serial.println("6.- Ejecutando tarea MSD");
    MSD.SaveFile(JSON_fac.make_json(
      MIRTC.format_time(), 
      MIRTC.format_date(), 
      SEN.get_potV(), 
      SEN.temperatura, 
      SEN.humedad, 
      false,  //warn pot
      false, //warn temp
      false //warn hum
    ));
    tiempo_anterior_msd = tiempo_actual;
  }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~ 8.- tarea_mqtt - Publicación periódica en servidor MQTT ~~~~~~~~~~~~~~~~~~~~~~~~ */
void millis_tasks::tarea_mqtt(void) {
  if ((tiempo_actual - tiempo_anterior_mqtt) >= TIEMPO_MQTT) {
    MIRTC.get_time();
    Serial.println("7.- Ejecutando tarea MQTT");

    //Generar el json para MQTT
    String json_str = JSON_fac.make_json(
      MIRTC.format_time(), 
      MIRTC.format_date(), 
      SEN.get_potV(), 
      SEN.temperatura, 
      SEN.humedad, 
      SEN.warning_pot, 
      SEN.warning_temp,
      SEN.warning_hum
    );

    // Publicamos y guardamos siempre
    mqtt.publish_MQTT(json_str);
    MSD.SaveFile(json_str);

    // Limpiamos las banderas de las alertas ya transmitidas
    if (SEN.warning_pot) {
      SEN.warning_pot = false;
      SEN.fecha_pot = "";
      SEN.hora_pot = "";
    }
    
    if (SEN.warning_temp) {
      SEN.warning_temp = false;
      SEN.fecha_temp = "";
      SEN.hora_temp = "";
    }

    if (SEN.warning_hum) {
      SEN.warning_hum = false;
      SEN.fecha_hum = "";
      SEN.hora_hum = "";
    }

    tiempo_anterior_mqtt = tiempo_actual;
  }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~ 9.- tarea_amb - Lectura periódica de sensores ambientales ~~~~~~~~~~~~~~~~~~~~~~~~ */
void millis_tasks::tarea_amb(void) {
  if ((tiempo_actual - tiempo_anterior_amb) >= TIEMPO_AMB) {
    SEN.leer_ambientales();
    Serial.print("9.- Ejecutando tarea Sensores Ambientales: ");
    Serial.println(SEN.temperatura);
    
    tiempo_anterior_amb = tiempo_actual;
  }
}