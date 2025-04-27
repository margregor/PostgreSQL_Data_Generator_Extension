#ifndef PYTHON_INTERFACE_H
#define PYTHON_INTERFACE_H

#include "postgres.h"

void doPythonInitialize();
Datum* doPythonThings(char** type_hints, int count);

#endif //PYTHON_INTERFACE_H
