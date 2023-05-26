#include "Server.h"

/////////// SETUP ////////////////
void setup()
{
  Serial.begin(115200);
  //Inicia los recursos necesarios para hacer llamadas al servidor
  ServerSetup();
}
/////////// LOOP ////////////////
void loop ()
{
  //Importamos las funciones necesarias para escuchar y emitir datos del servidor
  ServerLoop();
  ///////////////////////////////////////////////////////////////////////////////
}
