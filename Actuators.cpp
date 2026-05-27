#include "Actuators.h"

// Definición de los objetos globales
actuadores ACT;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void actuadores::config(void) {
  // Configuración de pines del Puente H
  pinMode(PIN_FAN_PWM, OUTPUT);
  pinMode(PIN_FAN_IN1, OUTPUT);
  pinMode(PIN_FAN_IN2, OUTPUT);

  // Asegurar que el ventilador inicie apagado
  control_ventilador(false);

  // Configuración de los Servos
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  servoDer.setPeriodHertz(50);
  servoIzq.setPeriodHertz(50);

  // Canales PWM para la ventana
  servoDer.attach(PIN_SERVO_DER, 500, 2400);
  servoIzq.attach(PIN_SERVO_IZQ, 500, 2400);

  // Evitar que suenen al inicio
  relajar_servos();

  // Inicialización de Pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Fallo al inicializar la pantalla OLED"));
  } else {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 25);
    display.println(F("Datalogger - WinIoT"));
    display.setCursor(10, 35);
    display.println(F("Iniciando..."));
    display.display();
  }
}

void actuadores::abrir_ventana(void) {
  if (!ventana_abierta) {
    Serial.println(F(">> MOTOR: Abriendo..."));

    //servoDer.attach(PIN_SERVO_DER, 500, 2400);
    //servoIzq.attach(PIN_SERVO_IZQ, 500, 2400);

    // Escribir los ángulos en modo espejo
    servoDer.write(ANGULO_ABIERTO_DER);
    servoIzq.write(ANGULO_ABIERTO_IZQ);

    // Esperar a que los servos alcancen su posición mecánicamente
    delay(1500);

    relajar_servos();
    ventana_abierta = true;
  }
}

void actuadores::cerrar_ventana(void) {
  if (ventana_abierta) {
    Serial.println(F(">> MOTOR: Cerrando..."));

    //servoDer.attach(PIN_SERVO_DER, 500, 2400);
    //servoIzq.attach(PIN_SERVO_IZQ, 500, 2400);

    // Escribir los ángulos de cierre en modo espejo
    servoDer.write(ANGULO_CERRADO_DER);
    servoIzq.write(ANGULO_CERRADO_IZQ);

    delay(1500);

    relajar_servos();
    ventana_abierta = false;
  }
}

void actuadores::relajar_servos(void) {
  Serial.println(F(">> MOTOR: Relax (OFF)"));
  //servoDer.detach();
  //servoIzq.detach();
  servoDer.writeMicroseconds(0);
  servoIzq.writeMicroseconds(0);
}

void actuadores::control_ventilador(bool encender, int velocidad_pwm) {
  if (encender) {
    // Configurar dirección de giro (IN1=HIGH, IN2=LOW)
    digitalWrite(PIN_FAN_IN1, HIGH);
    digitalWrite(PIN_FAN_IN2, LOW);
    // Aplicar velocidad mediante PWM (0 a 255)
    analogWrite(PIN_FAN_PWM, velocidad_pwm);
    ventilador_encendido = true;
  } else {
    // Freno del motor (IN1=LOW, IN2=LOW) y apagar PWM
    digitalWrite(PIN_FAN_IN1, LOW);
    digitalWrite(PIN_FAN_IN2, LOW);
    analogWrite(PIN_FAN_PWM, 0);
    ventilador_encendido = false;
  }
}

void actuadores::mostrar_datos(
  float temp_in,
  float temp_out,
  float hum,
  uint8_t lluvia,
  bool w_abierta,
  bool fan_on) {
  display.clearDisplay();

  // Encabezado
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(F("--- DATOS CLIMA ---"));

  // Bloque de Temperaturas y Humedad
  display.setCursor(0, 15);
  display.print(F("Tin:"));
  display.print(temp_in, 1);
  display.print(F("C "));
  display.print(F("H:"));
  display.print(hum, 0);
  display.println(F("%"));

  display.setCursor(0, 27);
  display.print(F("Tex:"));
  display.print(temp_out, 1);
  display.print(F("C "));

  display.setCursor(0, 39);
  display.print(F("Lluvia: "));
  display.print(lluvia);
  display.println(F("%"));

  // Indicadores de Estado Físico
  display.setCursor(0, 51);
  display.print(F("Ventana: "));
  display.print(w_abierta ? F("ABIERTA") : F("CERRADA"));

  // Indicador del ventilador
  if (fan_on) {
    display.setCursor(100, 51);
    display.print(F("FAN"));
  }

  display.display();
}