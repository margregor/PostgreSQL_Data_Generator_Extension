#pragma region Postgres Includes
#pragma warning(push)
#pragma warning(disable : 4057)
#pragma warning(disable : 4100)
#pragma warning(disable : 4200)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#include <postgres.h>
#include <fmgr.h>
#include <funcapi.h>
#include <utils/builtins.h>
#include <executor/spi.h>
#include <miscadmin.h>
#pragma warning(pop)
#pragma endregion

#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include "py_interaction.h"

PyObject** generate_multiple_by_types(char* inputs[], int count, int num_arrays, int* out_size) {


    PyObject *pModule = PyImport_ImportModule("c_interaction");
    if (!pModule) {
        elog(WARNING, "Failed to import myscript.py");
        Py_Finalize();
        return NULL;
    }

    PyObject *pFunc = PyObject_GetAttrString(pModule, "generate_multiple_by_types");
    if (!pFunc || !PyCallable_Check(pFunc)) {
        elog(WARNING, "Function generate_multiple_by_types not found or not callable");
        Py_DECREF(pModule);
        Py_Finalize();
        return NULL;
    }

    // Prepare input list
    PyObject *pyList = PyList_New(count);
    for (int i = 0; i < count; ++i) {
        PyObject *pyStr = PyUnicode_FromString(inputs[i]);
        PyList_SetItem(pyList, i, pyStr);
    }

    PyObject *pArgs = PyTuple_Pack(2, pyList, PyLong_FromLong(num_arrays));
    PyObject *pReturn = PyObject_CallObject(pFunc, pArgs);
    Py_DECREF(pArgs);
    Py_DECREF(pFunc);
    Py_DECREF(pModule);
    Py_DECREF(pyList);

    if (!pReturn || !PyList_Check(pReturn)) {
        PyErr_Print();
        elog(WARNING, "Returned object is not a list");
        Py_Finalize();
        return NULL;
    }

    *out_size = (int)PyList_Size(pReturn);
    PyObject** result_array = palloc(sizeof(PyObject*) * *out_size);

    for (int i = 0; i < *out_size; ++i) {
        result_array[i] = PyList_GetItem(pReturn, i);
        Py_INCREF(result_array[i]);
    }

    Py_DECREF(pReturn);
    return result_array;
}
