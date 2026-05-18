#include "MicroSD.h"

// Definición del objeto global File
File MicroSD_File;

//  Inicializa la tarjeta MicroSD en el pin definido.
//  Si falla la inicialización, muestra un mensaje de error y reintenta hasta 5 veces.
void MicroSD::MicroSD_init(void) {
  uint8_t intentos = 0;
  
  while (!SD.begin(MICROSD_PIN) && intentos < 5) {
    Serial.println(F("Falló la inicialización del módulo MicroSD"));
    delay(1000);  // Espera 1 segundo antes de reintentar
    intentos++;
  }
  
  if (intentos < 5) {
    Serial.println(F("La librería MicroSD ha inicializado con éxito"));
  } else {
    Serial.println(F("Error crítico: Límite de intentos alcanzado. Continuando sin MicroSD."));
  }
}

//  Recibe un mensaje (String) y lo guarda en el archivo definido.
//  Si el archivo no existe, lo crea. Si existe, lo abre en modo append.
void MicroSD::SaveFile(String msg) {

  // Abrir o crear archivo en modo append (agregar al final)
  MicroSD_File = SD.open(filename, FILE_APPEND);
  if (MicroSD_File) {
    // Escribir el mensaje en el archivo
    MicroSD_File.print(msg);
    MicroSD_File.close();  // Cerrar archivo
  } else {
    // Si no se pudo abrir el archivo, mostrar error
    Serial.print(F("Error abriendo "));
    Serial.println(filename);
  }
}

//  Abre el archivo en modo lectura y muestra su contenido en el monitor serie.
//  Si no se puede abrir, muestra un mensaje de error.
void MicroSD::ReadFile(void) {
  MicroSD_File = SD.open(filename);  // Abrir archivo en modo lectura
  if (MicroSD_File) {
    Serial.println("Contenido del archivo:");

    // Leer carácter por carácter hasta que no haya más datos
    while (MicroSD_File.available()) {
      Serial.write(MicroSD_File.read());
    }

    MicroSD_File.close();  // Cerrar archivo
  } else {
    // Si no se pudo abrir el archivo, mostrar error
    Serial.println("Error abriendo el archivo");
  }
}