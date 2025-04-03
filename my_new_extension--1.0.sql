CREATE FUNCTION my_set_returning_function(integer)
RETURNS SETOF RECORD AS 'MODULE_PATHNAME', 'my_set_returning_function'
LANGUAGE C STRICT;
