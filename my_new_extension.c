#include "postgres.h"
#include "fmgr.h"
#include "funcapi.h"
#include "utils/builtins.h"
#include "executor/spi.h"
#include "access/htup_details.h"
#include "miscadmin.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(my_set_returning_function);

Datum
my_set_returning_function(PG_FUNCTION_ARGS)
{
    ReturnSetInfo *rsinfo = (ReturnSetInfo *) fcinfo->resultinfo;
    TupleDesc tupdesc;
    Tuplestorestate *tupstore;
    MemoryContext per_query_ctx;
    MemoryContext oldcontext;
    int col_count, i;

    elog(DEBUG1, "Function my_set_returning_function called");

    /* Ensure function is called in a valid SRF context */
    if (rsinfo == NULL || !IsA(rsinfo, ReturnSetInfo))
        elog(ERROR, "table-function protocol requires ReturnSetInfo");

    /* Ensure correct return mode */
    if (rsinfo->expectedDesc == NULL)
        elog(ERROR, "table-function protocol requires expectedDesc");

    /* Get tuple descriptor dynamically */
    if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
        elog(ERROR, "return type must be a composite type");

    col_count = tupdesc->natts;  // Get number of requested columns

    /* Switch to per-query memory context */
    per_query_ctx = rsinfo->econtext->ecxt_per_query_memory;
    oldcontext = MemoryContextSwitchTo(per_query_ctx);

    /* Initialize tuplestore */
    tupstore = tuplestore_begin_heap(true, false, work_mem);
    rsinfo->returnMode = SFRM_Materialize;
    rsinfo->setResult = tupstore;
    rsinfo->setDesc = tupdesc;  /* 🔴 Ensure setDesc is assigned */

    MemoryContextSwitchTo(oldcontext);

    /* Allocate space for values and nulls dynamically */
    Datum *values = (Datum *) palloc0(col_count * sizeof(Datum));  /* 🔴 Ensure zero-initialization */
    bool *nulls = (bool *) palloc0(col_count * sizeof(bool));

    /* Fill rows dynamically */
    for (int row = 1; row <= 2; row++)
    {
        for (i = 0; i < col_count; i++)
        {
            char buf[64];
            snprintf(buf, sizeof(buf), "Row%d_%s", row, NameStr(tupdesc->attrs[i].attname)); // Use column name
            values[i] = CStringGetTextDatum(buf);
            nulls[i] = false;
        }
        tuplestore_putvalues(tupstore, tupdesc, values, nulls);
    }

    /* Cleanup */
    pfree(values);
    pfree(nulls);

    /* 🔴 Return correctly: Must be (Datum) 0 */
    PG_RETURN_VOID();
}
