CREATE EXTENSION database_data_generator;
SELECT * FROM generate_data(10) AS t("date" DATE, "integer" BIGINT, "float" REAL, "Miasto Zamieszkania" TEXT);
DROP EXTENSION database_data_generator;