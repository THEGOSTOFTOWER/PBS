## Реализация задания: Система обработки документов с плагинной архитектурой(document_processing)

Запуск

mkdir build & cd build
make .. -DBUILD_TEST=ON (для тестов)

bash ../test/run_style_check.sh - проверка на стиль

./test - тесты

./main {путь к файлу} {фильтр1 фильтр2 ...} - запуск программы (фильтры - lowercase uppercase nopunctuation wordcount)
