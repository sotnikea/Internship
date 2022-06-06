# Умови
Реалізувати простий консольний клієнт-серверний додаток на C++ з використанням однієї з наступних технологій:
- Boost/Asio
- Boost/MPI (OpenMPI)
- RPC: Apache Thrift, gRPC
- HTTP/WEB: PocoLib, Casablanka REST SDK, LibCurl 
- COM/DCOM (Windows)

Додаток має передавати файл через мережу з клієнта на сервер. 

# Вимоги
Назва файлу має передаватися до клієнта як параметр командного рядка. Серверна частина може бути довільною. 

# Формат сдачі
Готові файли проекту, або посилання на github

# Огляд реалізації
Для початку необхідно інсталювати boost. Завантажуємо останню версію бібліотеки за посиланням https://www.boost.org/users/download/#live, розпаковуємо та зберігаємо деінде на комп'ютері  
Створюємо проект в VisualStudio та відкриваємо термінал **View -> Terminal**  
![pic1](https://github.com/sotnikea/Apriorit/raw/main/part12/dz_12/img/pic1.png)   

В терміналі вказуємо шлях до теки з boost, наприклад  
![pic2](https://github.com/sotnikea/Apriorit/raw/main/part12/dz_12/img/pic2.png)    

Та даємо команду на компілювання **.\bootstrap.bat**  
Після чого вводимо команду **.\b2**  
Побудова триває близько 10 хвилин. 
По завершенню в налаштуваннях проекту   
![pic3](https://github.com/sotnikea/Apriorit/raw/main/part12/dz_12/img/pic3.png)  

Вказуємо шлях до завантаженного boost:  
![pic4](https://github.com/sotnikea/Apriorit/raw/main/part12/dz_12/img/pic4.png)  

А також шлях до його бібліотек  
![pic5](https://github.com/sotnikea/Apriorit/raw/main/part12/dz_12/img/pic5.png)  

З використанням boost створено два проекти - сервер, що здатен приймати та зберігати файл. Так клієнт, що під'єднується до серверу та відсилає йому цей файл.  
Перевіримо роботу додатків на трьох файлах, розміщених за адресою: E:\PROJECTS\c++\Some data
- text.txt (15 байт)  
- pic.jpg (2,98 МБ)
- видео.mp4 (91,4 МБ)

За допомогою VisualStudio будуємо проекти клієнту та серверу.  
За допомогою консолі (**Win+R -> cmd**) вказуємо шлях до теки із створенним exe файлом додатку серверу

~~~
E:\>cd E:\PROJECTS\c++\MyServ\Debug

E:\PROJECTS\c++\MyServ\Debug>
~~~

Запускаємо додаток. Зверніть увагу, що після назви файлу для запуску можна вказати порт, який буде прослуховувати сервер. Якщо цього не зробити, за замовчуванням буде встановлено порт 8888  
~~~
E:\PROJECTS\c++\MyServ\Debug>.\MyServ.exe
Server start...
~~~ 

Сервер готовий до прийому файлу. Так само, використовуючи консоль на поточному ПК, або іншому в тій самій мережі, вказуємо шлях до побудованого додатку клієнта  
~~~
E:\>cd E:\PROJECTS\c++\MyClient\Debug

E:\PROJECTS\c++\MyClient\Debug>
~~~

При його запуску обов'язковою умовою (згідно завдання) є введення назви файлу для копіювання. При його відсутності программа завершиться виключенням:  
~~~
E:\PROJECTS\c++\MyClient\Debug>.\MyClient.exe
Exception: Need path to file in command line arguments
~~~ 

Запускаємо клієнт, вказав файлом для копіювання text.txt розміром 15 байт
~~~
E:\PROJECTS\c++\MyClient\Debug>.\MyClient.exe E:\PROJECTS\c++\Some data\text.txt
~~~

Наступним етапом додаток запропонує вказати IP за яким працює сервер, та порт, який він прослуховує. Дізнатись ip можна за допомогою консолі та команди ipconfig  
![pic6](https://github.com/sotnikea/Apriorit/raw/main/part12/dz_12/img/pic6.png) 

Вказуємо необхідні IP та порт:
~~~
Enter server IP: 192.168.56.1
Enter port that server listen for: 8888
~~~

Відразу після введення виконується з'єднання з сервером та передача файлу, що видно з логів  
Клієнта:
~~~
Connection with server detected...
Sending file info: text.txt
Data transfer started...
Data transfer successfully end...
Sended: 15 bytes
~~~
Та сервера:  
~~~
Connection detected...
File name received...
Create file for writing: text.txt
Data transfer started...
Data transfer successfully end...
Received: 15 bytes
~~~
Як бачимо розмір файлів до відправки та після співпадають, як і їх дані при відкритті. Також варто відзначити, що файл копіюється в теку, в якій побудовано проект додатку-серверу (оскільки інших умов для нього не було вказано). Скопійований файл має те саме ім'я та розширення. Ці данні виокремлені в додатку клієнта з повного шляху до файлу та відправлені окремим соккетом, щоб не трапилось їх з'єднання з даними самого файлу.  
Сам процесс копіювання файлу містить ряд перевірок, що завершують процесс виключенням, накшталт порожніх IP та порту, проблем з відкриттям файлу, чи під'єднання к соккету.  
Також в процессі копіювання підраховується розмір переданих даних. Саме копіювання базується на операціях з бінарними файлами.

Спробуємо скопіювати більший за розміром та інший за контекстом pic.jpg розміром 2,98 МБ. Знову запускаємо сервер та клієнт. До клієнта подаємо наступну команду

~~~
E:\PROJECTS\c++\MyClient\Debug>.\MyClient.exe E:\PROJECTS\c++\Some data\pic.jpg
Enter server IP: 192.168.56.1
Enter port that server listen for: 8888
~~~

Отримаємо наступні логи від клієнта:
~~~
Connection with server detected...
Sending file info: pic.jpg
Data transfer started...
Data transfer successfully end...
Sended: 2.98316 Mb
~~~
Та від сервера:
~~~
Connection detected...
File name received...
Create file for writing: pic.jpg
Data transfer started...
Data transfer successfully end...
Received: 2.98316 Mb
~~~
І знов розмір файлів, їх назва, розширення та вміст до відправки та після повністю співпадають.

Протестуємо роботу програми з найбільшим відео файлом, що також містить в назві кирилицю. Знов запускаємо сервер та клієнт. Подаємо в клієнт наступну команду
~~~
E:\PROJECTS\c++\MyClient\Debug>.\MyClient.exe E:\PROJECTS\c++\Some data\видео.mp4
Enter server IP: 192.168.56.1
Enter port that server listen for: 8888
~~~
Отримаємо наступні логи від клієнта:
~~~
Connection with server detected...
Sending file info: видео.mp4
Data transfer started...
Data transfer successfully end...
Sended: 91.47 Mb
~~~
Та від сервера:
~~~
Connection detected...
File name received...
Create file for writing: видео.mp4
Data transfer started...
Data transfer successfully end...
Received: 91.47 Mb
~~~
Як і в попередніх випадках розмір файлів, їх назва, розширення та вміст до відправки та після повністю співпадають.

Тека, з якох проводилось копіювання:  
![pic7](https://github.com/sotnikea/Apriorit/raw/main/part12/dz_12/img/pic7.png) 

Та тека з додатком-сервером, та результатами прийому файлів:  
![pic8](https://github.com/sotnikea/Apriorit/raw/main/part12/dz_12/img/pic8.png) 

# Посилання
Download boost - https://www.boost.org/users/download/#live  
Пример сервера из документации boost - https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/example/cpp11/echo/blocking_tcp_echo_server.cpp  
Пример клиента из документации boost - https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/example/cpp11/echo/blocking_tcp_echo_client.cpp  

Разворачивание библиотеки boost - [Ссылка](https://ru.stackoverflow.com/questions/368975/%D0%9A%D0%B0%D0%BA-%D1%83%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%B8%D1%82%D1%8C-%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA%D1%83-boost)  
Компиляция и установка boost - http://kaktusenok.blogspot.com/2013/08/boost-visual-studio.html  
Networking in C++ Part #1: MMO Client/Server, ASIO & Framework Basics - https://www.youtube.com/watch?v=2hNdkYInj4g  
TCP Client Server Application | C++ in 2021 - https://www.youtube.com/watch?v=DVMHEDhYEr4  
Литература по boost - https://www.cyberforum.ru/blogs/131347/blog1078.html  
asio. Статья на хабре - https://habr.com/ru/post/192284/  


