# Тихонов Тимофей Павлович БПИ212 ОС ИДЗ 4 ВАРИАНТ 19. #
## UDP. ##

### Условие задачи: ###

Задача о болтунах. N болтунов имеют телефоны. Они либо ждут
звонков, либо звонят друг другу, чтобы побеседовать. Выбор этого
состояния осуществляется случайно и равновероятно. Если телефон случайного абонента занят, болтун будет звонить другому абоненту, пока ему кто-нибудь не ответит. Побеседовав некоторое время, болтун равновероятно или ждет звонка, или звонит на другой случайный номер. Создать приложение, моделирующее поведение болтунов. Каждый болтун — отдельный клиент. Сервер
получает число болтунов при запуске и является коммутатором.

### Общий комментарий ###

Для обеспечения корректного взаимодействия сетевых приложений и существующих в них процессов используются неименованные семафоры (имитирующие работу мьютексов): для регулирования доступа к пуллу задач и для нехаотичного вывода информации о каждой задаче по завершению очередного взаимодействия.
Разработанное приложение позволяет задавать ip-адрес сервера, к которому подключаются клиенты. Запускать сначала сервер - потом клиентов.
Код документирован комментариями на английском и русском. 

Компиляция через эти команды(компилируем вместе с библиотекой):
```sh
gcc UDPLib.c Client.c -o Client
gcc UDPLib.c Server.c -o Server
```
### Запуск сервера (port) (кол-во болтунов не менять - всегда 5): ###
```sh
./Server 5000 5
```
### Запуск клиентов можно осуществлять вручную. (ip server) (port) (unique boltun id) ###
```sh
./Client 127.0.0.1 5000 1
./Client 127.0.0.1 5000 2
./Client 127.0.0.1 5000 3
./Client 127.0.0.1 5000 4
./Client 127.0.0.1 5000 4```

### Работа на 4-5 балла : ###
Сервер разделяется на потоки для каждого из клиентов, при этом храня о них информацию в статической памяти. Изначально программа сделана для 5 болтунов. Общение между клиентами и сервером-коммутатором происходит посредство сигналов. Структура данных Call содержит id (чтобы различать запросы от каждого из болтунов хранящиеся на сервере), caller_id (равен -1 если болтун ждет звонка) и receiver_id - задан айди болтуна если он ожидает звонок или айди того, кому совершается звонок от самого болтуна.

Болтун запускаясь, реашет ожидать ему звонка или звонить самому, он возвращается к этому снова и снова после любого удачного звонка или после того как ему не ответил ни один из болтунов. Обычно выполнение программы зависает на этапе когда все болтуны ждут звонка, так как в условии не было сказано что им делать в такой ситуации - они будут ждатьь пока кто-нибудь не позвонит.
![photo_2023-09-25_17-56-24](https://github.com/timtikh/OS_IDZ_3/assets/95489979/78806606-1e20-414d-9359-56861179da5a)
Вся логика работает корректно, есть иногда ошибки в самом выводе, думаю это из-за переполнения буфера.
![photo_2023-09-25_17-56-50](https://github.com/timtikh/OS_IDZ_3/assets/95489979/1e61daa3-2cb4-43d9-b69e-f416bbff5d62)

Логика расписана на enum - понятные статусы на английском для каждого из запросов или ответов.
![photo_2023-09-25_17-57-49](https://github.com/timtikh/OS_IDZ_3/assets/95489979/b048ecf4-5a09-454b-8670-91c591bdd5c0)



При завершении сервера - выводится массив calls в консоль.
