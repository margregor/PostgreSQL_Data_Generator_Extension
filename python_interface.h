#ifndef PYTHON_INTERFACE_H
#define PYTHON_INTERFACE_H

#include <postgres.h>

void doPythonInitialize();
void doPythonGenerate(char** type_hints, int col_count, int row_count,
    Tuplestorestate *tupstore, TupleDesc tupdesc, const bool *nulls);

#endif //PYTHON_INTERFACE_H
