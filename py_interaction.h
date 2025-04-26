#ifndef PY_INTERACTION_H
#define PY_INTERACTION_H

#include <Python.h>

PyObject** generate_multiple_by_types(const char* inputs[], int count, int num_arrays, int* out_size);

#endif //PY_INTERACTION_H
