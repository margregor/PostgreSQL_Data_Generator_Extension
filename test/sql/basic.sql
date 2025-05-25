CREATE EXTENSION my_new_extension;
SELECT * FROM my_set_returning_function(10) AS t("date" DATE, "integer" BIGINT, "float" REAL, "Miasto Zamieszkania" TEXT);
DROP EXTENSION my_new_extension;