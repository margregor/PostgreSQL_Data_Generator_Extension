#include "postgres.h"
#include "utils/geo_decls.h"

PG_MODULE_MAGIC;

PGDLLEXPORT Datum my_new_extension_test(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(my_new_extension_test);

Datum my_new_extension_test(PG_FUNCTION_ARGS)
{
    char *input = text_to_cstring(PG_GETARG_TEXT_PP(0));

    PG_RETURN_CSTRING(input);
}
