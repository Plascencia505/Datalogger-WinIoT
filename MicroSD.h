#ifndef MicroSD_h
#define MicroSD_h

#include <SD.h>
#include <SPI.h>

// Pin de control de la MicroSD (CS - Chip Select)
#define MICROSD_PIN 5

// Objeto global declarado como externo para evitar redefiniciones
// Solo existe uno en toda la memoria
extern File MicroSD_File;

class MicroSD {
  // VARIABLES
public:
  uint8_t contador = 0;
  /* Contador auxiliar (puede usarse para llevar registro de operaciones). */

  String filename = "/TestDatalogger.txt";

  // MÉTODOS
public:
  void MicroSD_init(void);  // Inicializa la tarjeta MicroSD
  void SaveFile(String);    // Guarda datos en el archivo
  void ReadFile(void);      // Lee datos del archivo
};

extern MicroSD MSD;

#endif