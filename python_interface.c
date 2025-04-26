#pragma region Postgres Includes
#pragma warning(push)
#pragma warning(disable : 4057)
#pragma warning(disable : 4100)
#pragma warning(disable : 4200)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#include "postgres.h"
#include "fmgr.h"
#include "funcapi.h"
#include "utils/builtins.h"
#include "executor/spi.h"
#include "miscadmin.h"
#pragma warning(pop)
#pragma endregion

#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include "py_interaction.h"
#include "python_interface.h"

void doPythonThings(char** type_hints, int count) {
    int num_arrays = 3;  // Specify the number of arrays to generate
    int out_size = 0;

    PyObject **results = generate_multiple_by_types(type_hints, count, num_arrays, &out_size);

    if (results == NULL) {
        elog(ERROR, "Error calling Python");
    }

    // Import datetime module to check for date type
    PyObject *datetime_module = PyImport_ImportModule("datetime");
    PyObject *date_class = PyObject_GetAttrString(datetime_module, "date");

    printf("Generated values:\n");

    for (int i = 0; i < out_size; ++i) {
        PyObject *array_item = results[i];

        if (!PyList_Check(array_item)) {
            elog(WARNING, "Expected a list of values");
            continue;
        }

        printf("Array %d:\n", i + 1);
        int array_size = (int)PyList_Size(array_item);

        for (int j = 0; j < array_size; ++j) {
            PyObject *item = PyList_GetItem(array_item, j);

            if (PyLong_Check(item)) {
                elog(NOTICE, " - [%s] int: %ld\n", type_hints[j], PyLong_AsLong(item));
            } else if (PyFloat_Check(item)) {
                elog(NOTICE, " - [%s] float: %f\n", type_hints[j], PyFloat_AsDouble(item));
            } else if (PyBool_Check(item)) {
                elog(NOTICE, " - [%s] bool: %s\n", type_hints[j], PyObject_IsTrue(item) ? "True" : "False");
            } else if (PyUnicode_Check(item)) {
                elog(NOTICE, " - [%s] string: %s\n", type_hints[j], PyUnicode_AsUTF8(item));
            } else if (PyObject_IsInstance(item, date_class)) {
                PyObject *str_repr = PyObject_Str(item);
                elog(NOTICE, " - [%s] date: %s\n", type_hints[j], PyUnicode_AsUTF8(str_repr));
                Py_DECREF(str_repr);
            } else {
                elog(NOTICE, " - [%s] unknown type\n", type_hints[j]);
            }

            Py_DECREF(item);
        }
    }

    Py_DECREF(date_class);
    Py_DECREF(datetime_module);
    pfree(results);

    Py_Finalize();
}
