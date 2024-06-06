Формат команд:

Добавление записи
ADD "pool_name" "scheme_name" "collection_name" "customer_id" "customer_name" "customer_money"

Чтение записи
READ "pool_name" "scheme_name" "collection_name" "customer_id"

Чтение в диапазоне
READBETWEEN "pool_name" "scheme_name" "collection_name" "customer_id_min" "cutomer_id_max"

Обновление существующей записи
UPDATE "pool_name" "scheme_name" "collection_name" "customer_id" "new_customer_name" "new_customer_money"

Удаление записи
DELETE "pool_name" "scheme_name" "collection_name" "customer_id"

Добавление пула
ADDPOOL "pool_name" "allocator_type"

Удаление пула
DELETEPOOL "pool_name"

Добавление схемы в пул
ADDSCHEME "pool_name" "scheme_name" "allocator_type"

Удаление схема из пула
DELETESCHEME "pool_name" "scheme_name"

Добавление коллекции
ADDCOLLECTION "pool_name" "scheme_name" "collection_name" "allocator_type"

Удаление коллекции
DELETECOLLECTION "pool_name" "scheme_name" "collection_name"

Ключами в коллекциях являются идентификаторы клиентов (uint), в схемах - имя коллекции (string), в пулах - имя схемы (string)

Имя любого контейнера - строка только из букв латинского алфавита

При флаге --memory-cache - работа с оперативной памятью
При --file-system - работа с файловой системой

Работа файловой системы:
Создается папка database, которая будет хранить папки, хранящие в себе схемы (это пулы). Схемы - это папки, хранящие в себе коллекции (тоже папки). В коллекциях находятся файлы, у которых название - ключ клиента, а внутри находится имя клиента и его баланс

Выполнения дополнительные задания:
1. Интерактивный диалог с пользователем. Реализация в handler.h и dialog.h. Для запуска интерактивного диалога необходимо вместо имени файла ввести флаг --dialog
2. Персистентность
3. Вторичное индексирование. Поиск по клиентам можно задать по: их идентификатору, имени и балансу