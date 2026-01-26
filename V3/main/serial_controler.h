#ifndef SERIAL_CONTROLER_H
#define SERIAL_CONTROLER_H

#include "Sensors.h"

struct ValveMap { const char* name; uint8_t id; };

static const ValveMap valveMap[] = {
  {"sv11", SV11}, {"sv12", SV12}, {"sv13", SV13},
  {"sv21", SV21}, {"sv22", SV22}, {"sv24", SV24}, {"sv25", SV25},
  {"sv31", SV31}, {"sv32", SV32}, {"sv33", SV33}, {"sv34", SV34}, {"sv35", SV35}, {"sv36", SV36},
  {"sv51", SV51}, {"sv52", SV52}, {"sv53", SV53},
  {"sv61", SV61}, {"sv62", SV62}, {"sv63", SV63},
  {"sv71", SV71},
};

void serial_loop();
void processCommand(String command);
uint8_t convertValve(const String& t);


#endif