# Умови
Необхідно створити простий застосунок під Linux із застосуванням функціоналу з "socket.h", який вміє обслуговувати декількох TCP клієнтів та вміє відповідати "world" на повідомлення "hello", а будь-яке інше повідомлення пересилає назад клієнту.   
Також необхідно реалізувати клієнт, який дозволяє підключатися до сервера та надсилати йому повідомлення.

# Способ оцінювання
* 60% - робоча програма.
* 40% - якісний код.

# Формат сдачи
Готові файли проекту, або посилання на github

# Огляд рішення
Для реалізації черги клієнтів у додатку-серверу була використана функція select. Дана функція використовується для ефективного стеження за декількома файловими дескрипторами — для очікування, коли один із них не стане "готовим", тобто з'явиться можливість читання-запису даних, або з файловим дескриптором не виникне "виключна" ситуація.

Рішення містить два файли:
- server.c - містить код додатку-серверу
- client.c - містить код додатку-клієнту

Для побудови проекту необхідно викликати термінал в теці з файлами додатків та ввести команди

~~~
gcc -o server server.c
gcc -o client client.c
~~~

Для початку перевіримо роботу сервера з одним клієнтом, який кілька разів перезапускається та надсилає серверу різні повідомлення. За умовами програми така поведінка клієнта допустима, тоді як сервер мусить працювати без перезапусків, постійно прослуховуючи заданий порт.

Для запуска серверу вводимо команду:
~~~
./server
~~~

Зверніть увагу, что за замовчуванням сервер для прослуховування використовує порт 8888. Якщо необхідно змінити номер порту, його можна задати як аргумен під час запуску
~~~
./server 23162
~~~

Після запуску отримаємо лог
~~~
Server start listening on port: 8888...
~~~

Запустимо ще один термінал та запустимо додаток-клієнт командою:
~~~
./client
~~~

Якщо при виклику даної команди не вказати повідомлення, що необхідно надіслати до серверу, така можливість буде запропонована в процессі виконання. Скористаємося цим и вкажемо повідомленням слово "hello"
~~~
Input message for sending: hello
~~~

На наступному етапі клієнт запропонує вказати порт, за яким можливе підключення до серверу. Використаємо 8888
~~~
Input port for connection with server: 8888
~~~

В результаті виконання отримаємо наступні логи

Від серверу:
~~~
Server start listening on port: 8888...
Adding client on fd 4
Server start listening on port: 8888...
Client connected on fd 4
Get message from fd: 4 - hello
Sent a response to client
Server start listening on port: 8888...
removing client on fd 4
~~~
Від клієнту:
~~~
Connect to server on port: 8888...
Sending message to server...
Get message from server: world
~~~

Як і потребувало завдання, сервер на слово "hello" повернув "world". Також, як бачимо в логах серверу, в процессі виконання він спочатку додав до черги нового клієнта з дескриптором 4, після чого знову почав прослуховувати порт. Після визначення активності з боку клієнта, проведено з'єднання з ним та отримано повідомлення. Відправлена відповідь після чого знову почато прослуховування порту. Далі отримана активність клієнта з інформацією про завершення з'єднання. Клієнт видаляється з черги і сервер знову починає прослуховування порту.

Спробуємо знову запустити клієнта та надіслати ще одне повідомлення на сервер, який досі працює. Скористаємося аргументами при виклику:
~~~
./client hi there my deer friend
Input port for connection with server: 8888
~~~

Після чого отримуємо наступні логи 

Від серверу:
~~~
Server start listening on port: 8888...
Adding client on fd 4
Server start listening on port: 8888...
Client connected on fd 4
Get message from fd: 4 -  hi there my deer friend
Sent a response to client
Server start listening on port: 8888...
removing client on fd 4
Server start listening on port: 8888...
~~~

Від клієнта:
~~~
Connect to server on port: 8888...
Sending message to server...
Get message from server:  hi there my deer friend
~~~

Бачимо що це повідомлення також оброблено. Відповіддю відправлене те саме повідомлення, що відсилав клієнт (згідно з умовами завдання). Після чого сервер знову почав прослуховування порту.

Спробуємо протестувати роботу серверу при одночасному підлюченні кількох клієнтів. Для даного тесту в код серверу (на етапі обробки повідомлення) додане очікування в 5 секунд перед виконанням операції. Це необхідно, щоб встигнути запустити кілька клієнтів та почати їх з'єднання з сервером.

Отже маємо чотири термінали.

В першому запускаємо сервер:
~~~
$ ./server
Server start listening on port: 8888...
~~~

В трьох інших терміналах вводимо команди і запускаємо їх по можливості максимально одночасно  
Клієнт 1:
~~~
$ ./client hello
Input port for connection with server: 8888
~~~

Клієнт 2:
~~~
$ ./client something
Input port for connection with server: 8888
~~~

Клієнт 3:
~~~
$ ./client 11111
Input port for connection with server: 8888
~~~

Після виконання маємо наступні логи:

Сервер:
~~~
Server start listening on port: 8888...
Adding client on fd 4
Server start listening on port: 8888...
Client connected on fd 4
Get message from fd: 4 -  hello
Sent a response to client
Server start listening on port: 8888...
Adding client on fd 5
Server start listening on port: 8888...
Adding client on fd 6
Client connected on fd 5
Get message from fd: 5 -  something
Sent a response to client
Server start listening on port: 8888...
removing client on fd 4

Client connected on fd 6
Get message from fd: 6 -  11111
Sent a response to client
Server start listening on port: 8888...
removing client on fd 5

Server start listening on port: 8888...
removing client on fd 6

Server start listening on port: 8888...
~~~

Клієнт 1:
~~~
Connect to server on port: 8888...
Sending message to server...
Get message from server:  hello
~~~

Клієнт 2:
~~~
Connect to server on port: 8888...
Sending message to server...
Get message from server:  something
~~~

Клієнт 3:
~~~
Connect to server on port: 8888...
Sending message to server...
Get message from server:  11111
~~~

Як бачимо, кожен клієнт отримав очікувану відповідь згідно з умовами завдання.
По логам серверу видно, що на етапі обробки повідомлення першого клієнта, виявлено клієнти 2 та 3, які додані в чергу. Після обробки повідомлення 2 клієнту, 1 видалено з черги через завершення його активності. Після чого оброблене повідомлення 3 клієнту. Далі оба клієнта видалені з черги.

Таким чином сервер успішно обробив повідомлення трьох клієнтів використовуючи чергу та один потік даних.

# Посилання
- Нейл Мэтью, Ричард Стоунс. Основы программирования в Linux
- function **select** https://ru.manpages.org/fd_set/2
