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

void doPythonInitialize()
{
    static bool inited = false;
    
    if (inited)
        return;

    Py_Initialize();

    char share_path[MAXPGPATH];
    char extension_share_path[MAXPGPATH];

    get_share_path(my_exec_path, share_path);
    join_path_components(extension_share_path, share_path, "my_new_extension");
    char* command = psprintf("import sys; sys.path.append(\"%s\")", extension_share_path);
    PyRun_SimpleString(command);
    pfree(command);
    inited = true;
}

Datum *doPythonThings(char **type_hints, int count)
{
    int num_arrays = 3;  // Specify the number of arrays to generate
    int out_size = 0;

    PyObject **results = generate_multiple_by_types(type_hints, count, num_arrays, &out_size);

    if (results == NULL) {
        elog(ERROR, "Error calling Python");
    }

    Datum *ret = palloc0_array(Datum, count);

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
                ret[j] = CStringGetTextDatum(PyUnicode_AsUTF8(PyObject_Str(item)));
            } else if (PyFloat_Check(item)) {
                ret[j] = CStringGetTextDatum(PyUnicode_AsUTF8(PyObject_Str(item)));
            } else if (PyBool_Check(item)) {
                ret[j] = CStringGetTextDatum(PyUnicode_AsUTF8(PyObject_Str(item)));
            } else if (PyUnicode_Check(item)) {
                ret[j] = CStringGetTextDatum(PyUnicode_AsUTF8(item));
            } else if (PyObject_IsInstance(item, date_class)) {
                PyObject *isoFormatFunc = PyObject_GetAttrString(item, "isoformat");
                PyObject *str_repr = PyObject_CallObject(isoFormatFunc, NULL);
                ret[j] = CStringGetTextDatum(PyUnicode_AsUTF8(str_repr));
                Py_DECREF(str_repr);
            } else {
                ret[j] = CStringGetTextDatum(type_hints[j]);
            }

            Py_DECREF(item);
        }
    }

    Py_DECREF(date_class);
    Py_DECREF(datetime_module);
    pfree(results);

    

    return ret;
}
