-- 1. 10 последних показаний
SELECT * FROM readings ORDER BY id DESC LIMIT 10;

-- 2. минимум, среднее и максимум по каждому датчику
SELECT sensor, MIN(value) AS min_v, ROUND(AVG(value), 2) AS avg_v, MAX(value) AS max_v
FROM readings GROUP BY sensor;

-- 3. все события по порядку
SELECT * FROM events ORDER BY id;
