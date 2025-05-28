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
#include <utils/fmgroids.h>
#include <catalog/pg_type.h>
#pragma warning(pop)
#pragma endregion

#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include "cjson/cJSON.h"
#include "utils/palloc.h"
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
    join_path_components(extension_share_path, share_path, "database_data_generator");
    char* command = psprintf("import sys; sys.path.append(\"%s\")", extension_share_path);
    PyRun_SimpleString(command);
    pfree(command);

    cJSON_Hooks hooks = {
        .malloc_fn = palloc,
        .free_fn = pfree
    };
    cJSON_InitHooks(&hooks);

    inited = true;
}

Datum convert_python_item_to_datum(PyObject *date_class, PyObject *item, Oid expected_type) {
    Datum ret = CStringGetTextDatum("");

    if (PyBool_Check(item)) {
        if (expected_type == BOOLOID)
            ret = BoolGetDatum(item == Py_True);
        else if (expected_type == TEXTOID) {
            PyObject *str_repr = PyObject_Str(item);
            ret = CStringGetTextDatum(PyUnicode_AsUTF8(str_repr));
            Py_DECREF(str_repr);
        } else {
            elog(ERROR, "Unexpected type for boolean value");
        }
    } else if (PyFloat_Check(item)) {
        if (expected_type == FLOAT4OID)
            ret = Float4GetDatum((float4) PyFloat_AsDouble(item));
        else if (expected_type == FLOAT8OID)
            ret = Float8GetDatum(PyFloat_AsDouble(item));
        else if (expected_type == TEXTOID) {
            PyObject *str_repr = PyObject_Str(item);
            ret = CStringGetTextDatum(PyUnicode_AsUTF8(str_repr));
            Py_DECREF(str_repr);
        } else {
            elog(ERROR, "Unexpected type for floating point value");
        }
    } else if (PyLong_Check(item)) {
        if (expected_type == INT2OID)
            ret = Int32GetDatum((int16) PyLong_AsLong(item));
        else if (expected_type == INT4OID)
            ret = Int32GetDatum((int32) PyLong_AsLong(item));
        else if (expected_type == INT8OID)
            ret = Int64GetDatum(PyLong_AsLong(item));
        else if (expected_type == TEXTOID) {
            PyObject *str_repr = PyObject_Str(item);
            ret = CStringGetTextDatum(PyUnicode_AsUTF8(str_repr));
            Py_DECREF(str_repr);
        } else {
            elog(ERROR, "Unexpected return type for integer value");
        }
    } else if (PyUnicode_Check(item)) {
        if (expected_type == TEXTOID) {
            ret = CStringGetTextDatum(PyUnicode_AsUTF8(item));
        } else {
            elog(ERROR, "Unexpected return type for string value");
        }
    } else if (PyObject_IsInstance(item, date_class)) {
        if (expected_type == DATEOID) {
            PyObject *isoFormatFunc = PyObject_GetAttrString(item, "isoformat");
            PyObject *str_repr = PyObject_CallObject(isoFormatFunc, NULL);
            ret = DirectFunctionCall1(date_in, CStringGetDatum(PyUnicode_AsUTF8(str_repr)));
            Py_DECREF(str_repr);
            Py_DECREF(isoFormatFunc);
        } else if (expected_type == TEXTOID) {
            PyObject *isoFormatFunc = PyObject_GetAttrString(item, "isoformat");
            PyObject *str_repr = PyObject_CallObject(isoFormatFunc, NULL);
            ret = CStringGetTextDatum(PyUnicode_AsUTF8(str_repr));
            Py_DECREF(str_repr);
            Py_DECREF(isoFormatFunc);
        } else {
            elog(ERROR, "Unexpected type for date value");
        }
    } else if (expected_type == TEXTOID) {
        elog(WARNING, "Unknown type");
        PyObject *str_repr = PyObject_Str(item);
        ret = CStringGetTextDatum(PyUnicode_AsUTF8(str_repr));
        Py_DECREF(str_repr);
    } else {
        elog(ERROR, "Unknown type with unexpected return type");
    }

    return ret;
}

void doPythonGenerate(char **type_hints, const int col_count, const int row_count,
                    Tuplestorestate *tupstore, const TupleDesc tupdesc, const bool *nulls)
{
    char alias_config_path[MAXPGPATH];
    get_share_path(my_exec_path, alias_config_path);
    join_path_components(alias_config_path, alias_config_path, "database_data_generator");
    join_path_components(alias_config_path, alias_config_path, "aliases.json");

    if (_access(alias_config_path, 0) == -1) {
        elog(INFO, "Alias configuration file %s not found", alias_config_path);
    }
    else {
        FILE *fp = fopen(alias_config_path, "r");
        if (fp == NULL) {
            elog(ERROR, "Unable to open aliases file");
        }
        if (fseek(fp, 0, SEEK_END) != 0) {
            fclose(fp);
            elog(ERROR, "Failed to seek end of file");
        }
        const long length = ftell(fp);
        rewind(fp);
        char *buffer = palloc(length + 1);
        if (fread(buffer, 1, length, fp) != length) {
            fclose(fp);
            pfree(buffer);
            elog(ERROR, "Failed to read file");
        }
        buffer[length] = '\0';

        cJSON *root = cJSON_Parse(buffer);

        fclose(fp);
        pfree(buffer);

        if (root == NULL) {
            elog(ERROR, "Error parsing aliases file");
        }

        cJSON const* aliases = cJSON_GetObjectItem(root, "aliases");
        if (aliases == NULL) {
            cJSON_Delete(root);
            elog(ERROR, "Error parsing aliases file");
        }

        for (int i = 0; i < col_count; ++i) {
            cJSON const* alias = cJSON_GetObjectItem(aliases, type_hints[i]);
            if (alias == NULL)  continue;

            char const* replacement = cJSON_GetStringValue(alias);
            if (replacement == NULL) {
                cJSON_Delete(root);
                elog(ERROR, "Alias replacement must be string");
            }
            pfree(type_hints[i]);
            type_hints[i] = pstrdup(replacement);
        }

        cJSON_Delete(root);
    }

    int out_size = 0;
    PyObject **results = generate_multiple_by_types(type_hints, col_count, row_count, &out_size);

    if (results == NULL) {
        elog(ERROR, "Error calling Python");
    }

    Datum *ret = palloc0_array(Datum, col_count);

    // Import datetime module to check for date type
    PyObject *datetime_module = PyImport_ImportModule("datetime");
    PyObject *date_class = PyObject_GetAttrString(datetime_module, "date");

    printf("Generated values:\n");

    for (int i = 0; i < out_size; ++i) {
        PyObject *array_item = results[i];


        printf("Array %d:\n", i + 1);
        const int array_size = (int)PyList_Size(array_item);

        for (int j = 0; j < array_size; ++j) {
            PyObject *item = PyList_GetItem(array_item, j);
            Oid expected_type = TupleDescAttr(tupdesc, j)->atttypid;

            ret[j] = convert_python_item_to_datum(date_class, item, expected_type);
        }

        Py_DECREF(array_item);
        tuplestore_putvalues(tupstore, tupdesc, ret, nulls);
    }

    Py_DECREF(date_class);
    Py_DECREF(datetime_module);
    pfree(results);

    pfree(ret);
}
