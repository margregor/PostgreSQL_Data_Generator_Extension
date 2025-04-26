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

#include "python_interface.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(my_set_returning_function);

PGDLLEXPORT Datum my_set_returning_function(PG_FUNCTION_ARGS)
{
    ReturnSetInfo *rsinfo = (ReturnSetInfo *) fcinfo->resultinfo;
    TupleDesc tupdesc;
    const int row_count = PG_GETARG_INT32(0);

    elog(DEBUG1, "Function my_set_returning_function called");

    if (row_count < 0)
        elog(ERROR, "Row count must be a non-negative integer");

    if (rsinfo == NULL || !IsA(rsinfo, ReturnSetInfo))
        elog(ERROR, "table-function protocol requires ReturnSetInfo");

    if (rsinfo->expectedDesc == NULL)
        elog(ERROR, "table-function protocol requires expectedDesc");

    if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
        elog(ERROR, "return type must be a composite type");

    const int col_count = tupdesc->natts;

    MemoryContext per_query_ctx = rsinfo->econtext->ecxt_per_query_memory;
    MemoryContext oldcontext = MemoryContextSwitchTo(per_query_ctx);

    Tuplestorestate *tupstore = tuplestore_begin_heap(true, false, work_mem);
    rsinfo->returnMode = SFRM_Materialize;
    rsinfo->setResult = tupstore;
    rsinfo->setDesc = tupdesc;

    MemoryContextSwitchTo(oldcontext);

    Datum *values = (Datum *) palloc0(col_count * sizeof(Datum));
    bool *nulls = (bool *) palloc0(col_count * sizeof(bool));

    

    char* types[] = {"string", "string"};
    doPythonThings(types, 2);

    for (int row = 1; row <= row_count; row++)
    {
        for (int i = 0; i < col_count; i++)
        {
            char buf[64];
            
            if (strlen(NameStr(tupdesc->attrs[i].attname)) > 30) //Low limit to be safe
                elog(ERROR, "Name of attribute %s is too long", NameStr(tupdesc->attrs[i].attname));

            snprintf(buf, sizeof(buf), "Row%d_%s", row, NameStr(tupdesc->attrs[i].attname));
            values[i] = CStringGetTextDatum(buf);
            nulls[i] = false;
        }
        tuplestore_putvalues(tupstore, tupdesc, values, nulls);
    }

    pfree(values);
    pfree(nulls);

    PG_RETURN_VOID();
}
