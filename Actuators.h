#ifndef Actuators_h
#define Actuators_h

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

// PINES
#define PIN_SERVO_DER 18
#define PIN_SERVO_IZQ 19

// Puente H TB6612FNG para el motor DC
#define PIN_FAN_PWM 12
#define PIN_FAN_IN1 27  // Pin de dirección 1
#define PIN_FAN_IN2 26  // Pin de dirección 2

// Pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// CONFIGURACIÓN DE SERVOS 
// Actuan como espejo, por eso ángulos opuestos
#define ANGULO_ABIERTO_DER 165
#define ANGULO_ABIERTO_IZQ 15
#define ANGULO_CERRADO_DER 90
#define ANGULO_CERRADO_IZQ 90

class actuadores {
private:
  Servo servoDer;
  Servo servoIzq;

public:
  // Variables de estado físico
  bool ventana_abierta = false;
  bool ventilador_encendido = false;

  // Métodos principales
  void config(void);

  // Control de Ventana
  void abrir_ventana(void);
  void cerrar_ventana(void);
  void relajar_servos(void);

  // Control de Ventilador
  void control_ventilador(bool encender, int velocidad_pwm = 255);

  // Visualización
  void mostrar_datos(
    float temp_in,
    float temp_out,
    float hum,
    uint8_t lluvia,
    bool w_abierta,
    bool fan_on);
};

// Declaración de objetos globales
extern actuadores ACT;
extern Adafruit_SSD1306 display;

#endif