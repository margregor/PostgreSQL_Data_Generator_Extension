#include "postgres.h"
#include "fmgr.h"
#include "funcapi.h"
#include "utils/builtins.h"
#include "executor/spi.h"
#include "access/htup_details.h"
#include "miscadmin.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(my_set_returning_function);

PGDLLEXPORT Datum my_set_returning_function(PG_FUNCTION_ARGS)
{
    ReturnSetInfo *rsinfo = (ReturnSetInfo *) fcinfo->resultinfo;
    TupleDesc tupdesc;
    Tuplestorestate *tupstore;
    MemoryContext per_query_ctx;
    MemoryContext oldcontext;
    int col_count;
    int row_count = PG_GETARG_INT32(0);

    elog(DEBUG1, "Function my_set_returning_function called");

    if (row_count < 0)
        elog(ERROR, "Row count must be a non-negative integer");

    if (rsinfo == NULL || !IsA(rsinfo, ReturnSetInfo))
        elog(ERROR, "table-function protocol requires ReturnSetInfo");

    if (rsinfo->expectedDesc == NULL)
        elog(ERROR, "table-function protocol requires expectedDesc");

    if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
        elog(ERROR, "return type must be a composite type");

    col_count = tupdesc->natts;

    per_query_ctx = rsinfo->econtext->ecxt_per_query_memory;
    oldcontext = MemoryContextSwitchTo(per_query_ctx);

    tupstore = tuplestore_begin_heap(true, false, work_mem);
    rsinfo->returnMode = SFRM_Materialize;
    rsinfo->setResult = tupstore;
    rsinfo->setDesc = tupdesc;

    MemoryContextSwitchTo(oldcontext);

    Datum *values = (Datum *) palloc0(col_count * sizeof(Datum));
    bool *nulls = (bool *) palloc0(col_count * sizeof(bool));

    for (int row = 1; row <= row_count; row++)
    {
        for (int i = 0; i < col_count; i++)
        {
            char buf[64];
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
