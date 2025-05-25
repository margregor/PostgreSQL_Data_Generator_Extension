CREATE EXTENSION my_new_extension;
SELECT * FROM my_set_returning_function(10) AS t("date" TEXT, "integer" TEXT, "city" TEXT);
DROP EXTENSION my_new_extension;