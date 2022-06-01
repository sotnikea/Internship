# 10. Многопоточность
## Оглавление
1. [Основные понятия](#r1)
2. [Условная классификация средств синхронизации](#r2)
    1. [Основные примитивы(Mutable execution)](#r2_1)
    2. [Condition variables](#r2_2)
    3. [Дополнительные средства](#r2_3)
3. [Распространенные проблемы при разработке многопоточных программ](#r3)
4. [Рекомендации по разработке многопоточных приложений](#r4)
5. [Литература](#r5)
6. [Упражнения для самостоятельной работы](#r6)
7. [Основные понятия кратко](#r7)
    - [thread](#thread)
    - [this_thread](#this_thread)
    - [mutex](#mutex)
    - [unique_lock](#unique_lock)
    - [dif](#dif)
    - [condition_variable](#condition_variable)
    - [semaphore](#semaphore)
8. [Примеры](#r8)
    - [Создание треда и join](#e1)
    - [detach](#e2)
    - [Потеря в потоке связи с объектом из-за его удаления](#e3)
    - [Падение программы в случае исключения отработанного до join или detach](#e4)
    - [Два потока работающие с одним и тем же значением](#e5)
    - [Передача одной и той же фунции с разными аргументами в разные потоки](#e6)
    - [Race condition и Data race](#e7)
    - [mutex для синхронизации при работе с общей функцией](#e8)
    - [lock_guard вместо unlock для mutex](#e9)
    - [shared_lock](#e10)
    - [Deadlock](#e11)
    - [Deadlock в классе](#e12)
    - [scoped_lock для решения проблемы Deadlock](#e13)
    - [Livelock и try_to_lock](#e14)
    - [Starvation](#e15)
    - [condition_variables](#e16)
    - [semaphore](#e17)
    - [future](#e18)
    - [promice](#e19)
    - [std::packaged_task](#e20)


        

    


## <a name="r1">Основные понятия</a>
**Многопоточность** — свойство платформы (например, операционной системы, виртуальной машины и т. д.) или приложения, состоящее в том, что процесс, порождённый в операционной системе, может состоять из нескольких потоков, выполняющихся «параллельно», то есть без предписанного порядка во времени. При выполнении некоторых задач такое разделение может достичь более эффективного использования ресурсов вычислительной машины (с) Wiki.  

**Поток (thread)** – средство выполнения кода программы — наименьшая единица обработки, исполнение которой может быть назначено ядром операционной системы.

Несколько потоков выполнения могут существовать в рамках одного и того же процесса и совместно использовать ресурсы, такие как память, тогда как процессы не разделяют этих ресурсов.  

Для компьютеров с одним процессором истинная многопоточность невозможна, для них возможна только **вытесняющая многозадачность** – когда диспетчер задач отдает квант времени одному потоку, затем приостанавливает его, отдает процессорное время другому потоку и т.д.  

**Многопоточность в условиях нескольких ядер** – одновременно может выполняться количество потоков равное количеству процессоров.  

**Синхронизация** – разграничение доступа к каким-либо ресурсам программы/системы для нескольких потоков либо программ.  

**Что синхронизировать**: работу с контейнерами/объектами, к которым происходит доступ из нескольких потоков, работу системными ресурсами (файлы/память), если это необходимо.  

**Как реализовать**: с помощью средств синхронизации, в некоторых случаях с помощью умных указателей и определения стратегии владения объектами.

## <a name="r2">Условная классификация средств синхронизации</a>
### <a name="r2_1">Основные примитивы(Mutable execution)</a>
- `mutex`
- `timed_mutex`
- `recursive_mutex`
- `recursive_timed_mutex`
- `shared_mutex`
- `shared_timed_mutex`

Название происходит от использования – для синхронизации блока выполняемого кода (функции, блока внутри функции).   
Особенности: 
- наличие владельца: объект синхронизации содержит в себе указатель на поток либо id потока;
- повторное вхождение: объект синхронизации может быть захвачен более одного раза одним потоком. Освободить нужно столько же раз, сколько раз объект был захвачен.

### <a name="r2_2">Condition variables</a>
- `condition_variable`
- `condition_variable_any`

Примитив, позволяющий ожидать, пока не случится какое-то событие с последующим атомарным захватом лока при выходе из функции ожидания.   
Рассмотрим на примере простого случая распространенной задачи **producer/consumer**:

~~~C++
std::mutex mutex;
std::condition_variable cv;
int g_x = 0;
bool ready = false;

void Producer() {
    std::unique_lock<std::mutex> lock(mutex); // acquire lock
    for (; g_x < 100;) {
        cv.wait(lock, [=] { return !ready; });
        ++g_x;
        ready = true;
        cv.notify_one();
    }
}
void Consumer() {
    std::unique_lock<std::mutex> lock(mutex);
    for (; g_x < 100;) {
        cv.wait(lock, [=] { return ready; });
        std::cout << g_x << "\n";
        ready = false;
        cv.notify_one();
    }
~~~
### <a name="r2_3">Дополнительные средства</a>
**Semaphore(`std::counting_semaphore, std::binary_semaphore`)**  – примитив, позволяющий войти в заданный участок кода не более чем N потокам. Содержит счетчик, который инкрементится/декрементится с вызовом enter/leave функций.  

**Read-Write locks**. Применяется в случае, когда чтение ресурса происходит много чаще его изменения. Потоков, захвативших `read lock`, может быть несколько, `write lock` может захватить только один поток. 

## <a name="r3">Распространенные проблемы при разработке многопоточных программ</a>
- **deadlock** - неосвобожденный лок, как результат – вечное ожидание в одном/нескольких потоках; 
- **livelock** - почти то же, что и дедлок, но лок проверяется в цикле с каким-то интервалом времени;
- **starvation** – проблема, когда поток не может получить доступ к ресурсу. Чаще всего спровоцирована слишком простым алгоритмом планировшщика, когда он, например, всегда отдает ресурс потоку с наивысшим приоритетом и у потока с более низким приоритетом просто нет возможности захватить ресурс.
- **race condition** – гонки потоков – ошибка проектирования многопоточной системы или приложения, при которой работа системы или приложения зависит от того, в каком порядке выполняются части кода.

## <a name="r4">Рекомендации по разработке многопоточных приложений</a>
- Не совершайте под локом сетевых вызовов или любых других вызовов, которые могут длиться неопределенно долго.
- Не ждите какого-либо события под локом.
- Всегда четко определяйте к каким ресурсам будет происходить доступ из нескольких потоков. Разграничивайте этот доступ с помощью примитивов синхронизации, дабы в любой момент времени, когда поток получает доступ к объекту, объект находился в согласованном состоянии. Чаще всего в роли объектов будут выступать контейнеры и указатели. Тщательно следите за временем жизни объекта, дабы поток не обратился к удаленному объекту либо удаленному элементу контейнера.
- Используйте умные указатели для совместного владения.
- Используйте гарды-врапперы для освобождения объекта синхронизации. Guard представляет собой класс, принимающий в конструкторе константную ссылку на объект синхронизации (cs, mutex etc), и вызывающий в деструкторе освобождение лока. Это даст вам гарантию того, что ваш объект не останется неосвобожденным, если в функции вылетит exception.
- Помните, что в любой момент у потока могут отобрать управление.

## <a name="r5">Литература</a>
- С++ concurrency in action, Anthony Williams
- "Джеффри Рихтер. Windows для профессионалов. Создание эффективных WIN32-приложений с учетом специфики 64-разрядной версии Windows”. 2001 год
- MSDN
- Wikipedia
- https://en.cppreference.com/w/cpp/thread 

## <a name="r6">Упражнения для самостоятельной работы</a>
- Реализуйте задачу single producer/single consumer
- Реализуйте простейший thread pool
- Напишите свою реализацию rwlocks


## <a name="r7">Основные понятия кратко</a>
### <a name="thread">thread</a>
**thread**
- Поток нельзя копировать
- Поток можно перемещать по rValue
- В поток можно передать функцию и аргументы этой функции
- Чтобы передать в функцию ссылку, использовать `std::ref`, иначе это будет ссылка на копию объекта
- Если не использовано `join` ил `detach`, будет вызван `std::termination`

### <a name="this_thread">this_thread</a>
**this_thread**:
- this_thread.get_id – возвращает идентификатор потока исполнения, в котором она вызвана
- this_thread.yield – сигнализирует ОС, что поток желает приостановить свое выполнение и дать шанс на исполнение другим потокам
- this_thread.sleep_until – поток приостанавливает выполнение до наступления момента, переданного в качестве аргумента
- this_thread.sleep_for – поток приостанавливает выполнение на некий, заданный промежуток времени

### <a name="mutex">mutex</a>
**mutex**
- lock – если мьютекс не принадлежит никакому потоку, тогда поток, вызвавший lock, становится его обладателем
- try_lock - если мьютекс не принадлежит никакому потоку, тогда поток, вызвавший try_lock, становится его обладателем и метод возвращает true. В противном случае возвращает false. try_lock не блокирует текущий поток
- unlock – освобождает ранее захваченный мьютекс.

### <a name="unique_lock">unique_lock</a>
**unique_lock** может
- Принимать не захваченный мьютекс в конструкторе
- Захватывать и освобождать мьютекс непосредственными вызовами lock/unlock
- Выполнять временной захват
- Может быть перемещен в другой объект unique_lock

### <a name="dif">Разница между lock_guard и unique_lock</a>
**Разница между lock_guard и unique_lock**  
`lock_guard` и `unique_lock` - почти то же самое. Разница в том, что вы можете заблокировать и разблокировать `std :: unique_lock`. `std :: lock_guard` будет заблокирован только один раз при построении и разблокирован при уничтожении.
- `lock_guard`: когда объект создан, он пытается получить мьютекс (вызывая `lock()`), а когда объект уничтожен, он автоматически освобождает мьютекс (вызывая `unlock()`).
- `unique_lock`: также поддерживает отложенную блокировку, временную блокировку, рекурсивную блокировку и использование условных переменных.

### <a name="condition_variable">condition_variable</a>
**condition_variable** - условные переменные
- `condition_variable`: требует от любого потока перед ожиданием сначала выполнить std::unique_lock
- `condition_variable_any`: более общая реализация, которая работает с любым типом, который можно заблокировать. Эта реализация может быть более дорогим (с точки зрения ресурсов и производительности) для использования, поэтому ее следует использовать только если необходима те дополнительные возможности, которые она обеспечивает

Как работают условные переменные:
- Должен быть хотя бы один поток, ожидающий, пока какое-то условие станет истинным. Ожидающий поток должен сначала выполнить `unique_lock`. Эта блокировка передается методу `wait()`, который освобождает мьютекс и приостанавливает поток, пока не будет получен сигнал от условной переменной. Когда это произойдет, поток пробудится и снова выполнится `lock`.
- Должен быть хотя бы один поток, сигнализирующий о том, что условие стало истинным. Сигнал может быть послан с помощью `notify_one()`, при этом будет разблокирован один (любой) поток из ожидающих, или `notify_all()`, что разблокирует все ожидающие потоки.
- В виду некоторых сложностей при создании пробуждающего условия, которое может быть предсказуемым в многопроцессорных системах, могут происходить ложные пробуждения (`spurious wakeup`). Это означает, что поток может быть пробужден, даже если никто не сигнализировал условной переменной. Поэтому необходимо еще проверять, верно ли условие пробуждение уже после того, как поток был пробужден. Т.к. ложные пробуждения могут происходить многократно, такую проверку необходимо организовывать в цикле.

### <a name="semaphore">semaphore</a>
**semaphore**  
Семафор (semaphore) – примитив синхронизации работы процессов и потоков, в основе которого лежит счётчик, над которым можно производить две атомарные операции: увеличение и уменьшение значения на единицу, при этом операция уменьшения для нулевого значения счётчика является блокирующей. Служит для построения более сложных механизмов синхронизации и используется для синхронизации параллельно работающих задач, для защиты передачи данных через разделяемую память, для защиты критических секций, а также для управления доступом к аппаратному обеспечению.

## <a name="r8">Примеры</a>
___
## <a name="e1">Создание треда и **join**</a>
~~~C++
#include <iostream>
#include <thread>

//Поток с некоторым заданием
void task() {
	std::cout << "task start" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(3));	//Некая полезная работа
	std::cout << "task finished" << std::endl;

}

int main(void) {
	std::thread t(task);		//Создаем поток t и помещаем в него функцию task
								//С этого места в программе работает 2 потока

	std::cout << "Main thread" << std::endl;

	t.join();	//Ожидаем, пока поток t завершится.
				//Последующие действия не будут продолжаться до его завершения
	
	std::cout << "Main thread ended" << std::endl;
}
~~~
~~~
Вывод:
Main thread
task start
task finished
Main thread ended
~~~
___
## <a name="e2">**detach**</a>
~~~C++
#include <iostream>
#include <thread>

void task()
{
    while (true)
    {
        std::cout << "some_useful_job" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main()
{
    std::thread t(task);    //Создаем дополнительный поток и помещаем 
                            //в него функцию task
    t.detach();             //Открепляем созданный поток от основного
                            //Теперь потоки работают независимо друг от друга

    
    std::cout << "Main thread start" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Main thread end" << std::endl;
}
~~~
___
## <a name="e3">Потеря в потоке связи с объектом из-за его удаления</a>
~~~C++
#include <iostream>
#include <thread>

class some_resource { /* содержит выделение памяти в куче */ };

void background_task(int& data)
{
    while (true)
    {
        //Вызываем некий метод 
        std::cout << data.getValueByIndex(3) << std::endl;
    }
}

void background_task_launcher()
{
    some_resource resource; //Создаем объекта класса

    //И передаем его по ссылке в функцию, выделяя отдельный поток
    std::thread task([&]()
        {
            background_task(resource);
        });

    task.detach();  //Открепляем текущего потока
}   //Здесь срабатывает деструктор класса, и объект удаляется
    //при этом запущенный поток продолжает пытаться обращаться к данным этого объекта

int main()
{
    background_task_launcher();
}

~~~
___
## <a name="e4">Падение программы в случае исключения отработанного до join или detach</a>
~~~C++
#include <thread>
#include <iostream>


void background_task(int& data)
{
    while (true)
    {
        //Некоторая полезная работа
        std::cout << ++data << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void background_task_launcher()
{
    auto some_local_state = 0;

    //Запускаем в отдельном потоке функцию
    std::thread task([&]()
        {
            background_task(some_local_state);
        });

    //task.detach();    //Если разместить здесь, поток будет продолжать работать
                        //даже с учетом брошенного исключения

    //Если в этом месте возникнет исключение
    //идущий далее detach не отработает
    throw std::runtime_error("oops");

    task.detach();  //Не отработает из-за исключения, что приведет к падению программы
}

int main()
{
    try {
        background_task_launcher();
    }
    catch (...) {
        std::cout << "Error" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(10));
}
~~~
___
## <a name="e5">Два потока работающие с одним и тем же значением</a>

~~~C++
#include <iostream>
#include <thread>

uint32_t Y = 5; //Некоторая глобальная переменная

void foo()
{
    Y += 1;
}

void bar()
{
    Y *= 2;
}

int main()
{
    //Создаем два потока, каждый из которых 
    //работает с одной и той же глобальной переменной
    std::thread t1(foo);
    std::thread t2(bar);

    t1.join();
    t2.join();

    //Выводим результат на экран
    std::cout << Y << std::endl;
}
~~~
Возможный вывод:
~~~
Если первой отработал foo, потом bar
12 
~~~
~~~
Если первой отработал bar, потом foo
11 
~~~
~~~
Если за то время, пока foo взял значение и обработал, bar так же успел взять значение и обработал его раньше. Фактически foo перезаписал значение не имея изменений внесенных bar
6 
~~~
___
## <a name="e6">Передача одной и той же фунции с разными аргументами в разные потоки</a>

~~~C++
#include <iostream>
#include <thread>
#include <vector>

//Функция для вывода вектора на экран
void foo(const std::vector<int>& v) {
    for (const auto& i : v) {
        std::cout << i << std::endl;
    }
}

int main()
{
    //Создаем два вектора с разными значениями
    std::vector<int> v1{ 1,2,3,4,5,6,7,8,9 };
    std::vector<int> v2{ 10,11,12,13,14,15,16,17,18,19,20 };

    //Создаем два потока и помещаем в них одну и ту же функцию
    //но передаем в функции разные векторы
    //При этом вывод смешает значения обоих векторов
    std::thread t1(foo, v1);
    std::thread t2(foo, v2);

    t1.join();
    t2.join();

}

~~~
___
## <a name="e7">Race condition и Data race</a>

Race condition - Когда результат программи зависит от того, в каком порядке будет выполняться поток
Data race - Когда несколько потоков пытаются использовать общие данные
Пример проблемы
~~~C++
#include <list>

std::list<int> some_list;

//Если один поток производит удаление из списка 
void remove_from_list(int value)
{
    some_list.remove(value);
}

//А другой в это же время пытает значение добавить
void add_to_list(int new_value)
{
    some_list.push_back(new_value);
}
~~~
___
## <a name="e8">mutex для синхронизации при работе с общей функцией</a>


~~~C++
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

std::mutex v_mutex; 

//Функция для вывода вектора на экран
void foo(const std::vector<int>& v) {
    v_mutex.lock(); //Первый поток захватывает mutex, 
                    //и следующий поток сможет начать выполнение этого фрагмента
                    //только после освобождения mutex
                    //Фактически стоит в очереди в ожидании
    for (const auto& i : v) {
        std::cout << i << std::endl;
    }
    v_mutex.unlock();   //Освобождаем mutex
}

int main()
{
    //Создаем два вектора с разными значениями
    std::vector<int> v1{ 1,2,3,4,5,6,7,8,9 };
    std::vector<int> v2{ 10,11,12,13,14,15,16,17,18,19,20 };

    //Создаем два потока и помещаем в них одну и ту же функцию
    //но передаем в функции разные векторы
    //При этом вывод НЕ смешает значения обоих векторов, т.к. использован mutex
    std::thread t1(foo, v1);
    std::thread t2(foo, v2);

    t1.join();
    t2.join();

}
~~~
___
## <a name="e9">lock_guard вместо unlock для mutex</a>

~~~C++
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

std::mutex v_mutex; 

//Функция для вывода вектора на экран
void foo(const std::vector<int>& v) {

    //Используем lock_guard, который сам разблокирует объект при выходе из области видимости
    std::lock_guard<std::mutex> v_guard(v_mutex); 
    for (const auto& i : v) {
        std::cout << i << std::endl;
    }

    //Здесь теперь не нужен unlock
}

int main()
{
    //Создаем два вектора с разными значениями
    std::vector<int> v1{ 1,2,3,4,5,6,7,8,9 };
    std::vector<int> v2{ 10,11,12,13,14,15,16,17,18,19,20 };

    
    std::thread t1(foo, v1);
    std::thread t2(foo, v2);

    t1.join();
    t2.join();
}
~~~
___
## <a name="e10">shared_lock</a>
shared_lock - позволяет читать данные нескольким потокам использующим общий мьютекс, а менять данные только одному, использующему тот же общий мьютекс

~~~C++
#include <iostream>
#include <thread>
#include <string>
#include <shared_mutex>
#include <sstream>

std::shared_mutex s_mutex;  //Создаем общий мутекс
std::string s = "Some text";    //Общий ресурс

void Write(int num) {
    std::lock_guard<std::shared_mutex> guard(s_mutex);
    std::cout << "Write work ---> "; //Эта строка выведется одновременно (не разобьется на части) с остальными
                                            //выводами функции, т.к. использован lock_guard блокирующий остальных
    std::cout << "Write " << num << std::endl;
    std::stringstream ss;
    ss << "New text " << num<<std::endl;
    s = ss.str();
}

void Read(int num) {    
    std::shared_lock<std::shared_mutex> guard(s_mutex);
    std::cout << "Read work ---> "; //Эта строка выведется врассыпную (разобьется на части) с остальными
                                            //выводами функции, т.к. использован shared_lock разрешающий доступ остальных
    std::stringstream ss;
    ss << "Read N" << num<<": "<<s << std::endl;
    std::cout << ss.str();
}

void ReadManager() {
    std::thread r[10];
    for (int i = 0; i < 10; i++) {
        r[i] = std::thread(Read, i);
    }

    for (int i = 0; i < 10; i++) {
        r[i].join();
    }
}

void WriteManager() {
    std::thread w[5];
    for (int i = 0; i < 5; i++) {
        w[i] = std::thread(Write, i);
    }

    for (int i = 0; i < 5; i++) {
        w[i].join();
    }
}

int main()
{    
    std::thread t1(ReadManager);
    std::thread t2(WriteManager);

    t1.join();
    t2.join();
}
~~~
___
## <a name="e11">Deadlock</a>
Deadlock - Ситуация, когда один поток захватывает мьютекс, который нужен другому потоку. А другой поток захватывает мьютекс, который нужен этому потоку. Как результат, они взаимно блокируют друг друга

~~~C++
#include<mutex>
#include<thread>
#include<iostream>

std::mutex resourceX;
std::mutex resourceY;

void thread_A_func()
{
    while (true)
    {
        std::unique_lock<std::mutex> lockX(resourceX);  //Блокируем мьютех Х
        std::cout << "thread_A taked resourse X" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));   //Полезная работа

        std::unique_lock<std::mutex> lockY(resourceY); //Пытаемся блокировать мьютех У, но он уже заняти
                                                       //функцией В, которая в свою очередь ждет освобождения мьютекса Х

        std::cout << "thread_A working" << std::endl;
    }
}

void thread_B_func()
{
    while (true)
    {
        std::unique_lock<std::mutex> lockY(resourceY); //Блокируем мьютекс У
        std::cout << "thread_B taked resourse Y" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));   //Полезная работа
        std::unique_lock<std::mutex> lockX(resourceX);  //Пытаемся заблокировать мьютекс Х
                                                        //но он занят функцией А, которая ждет разблокировки
                                                        //мьютекса У

        std::cout << "thread_B working" << std::endl;
    }
}

int main(void) {
    std::thread t1(thread_A_func);
    std::thread t2(thread_B_func);

    t1.join();
    t2.join();
}
~~~
___
## <a name="e12">Deadlock в классе</a>

~~~C++
#include<mutex>
#include<thread>
#include<iostream>
#include<list>


class DataHolder
{
public:
    // ... 
    DataHolder() = default;

    //Принимаем другой список по rValue
    DataHolder& operator=(DataHolder&& other) noexcept
    {
        swap(*this, other);
        return *this;
    }

    // ... 
private:
    void swap(DataHolder& lhs, DataHolder& rhs) const
    {
        std::cout << "Start swap" << std::endl;
        //Блокируем оба списка, чтобы произвести копирование
        //Именно здесь потоки блокируют друг друга при запуске перемещения для обоих списков сразу
        //в разных потоках
        std::lock_guard<std::mutex> lhs_lock(lhs.m_useful_data_lock);
        std::this_thread::sleep_for(std::chrono::seconds(1));   //Некоторая полезная работа
        std::lock_guard<std::mutex> rhs_lock(rhs.m_useful_data_lock);

        std::swap(lhs.m_useful_data, rhs.m_useful_data);
        std::cout << "End swap" << std::endl;
    }

private:
    std::list<int> m_useful_data;   //Некоторый список элементов
    std::mutex m_useful_data_lock;  //Мьютекс для блокировки при копировании данных
};

DataHolder data_holder_1;
DataHolder data_holder_2;

//Переносим из второго в первый
void thread_A() {
    data_holder_1 = std::move(data_holder_2);
}

//Переносим из первого во второй
void thread_B() {
    data_holder_2 = std::move(data_holder_1);
}

int main(void) {
    //В процессе выполнения потоки блокируют друг друга
    std::thread t1(thread_A);
    std::thread t2(thread_B);

    t1.join();
    t2.join();

}

~~~
___
## <a name="e13">scoped_lock для решения проблемы Deadlock</a>
scoped_lock для решения проблемы Deadlock
scoped_lock захватывает мьютексы в определенной последовательности

C++17 
~~~C++
#include<mutex>
#include<thread>
#include<iostream>
#include<list>


class DataHolder
{
public:
    // ... 
    DataHolder() = default;
    
    //Принимаем другой список по rValue
    DataHolder& operator=(DataHolder&& other) noexcept
    {        
        swap(*this, other);
        return *this;
    }

    // ... 
private:
    void swap(DataHolder& lhs, DataHolder& rhs) const
    {
        std::cout << "Start swap" << std::endl;

        //Контролирует раздачу доступа и deadlock ситуации
        std::scoped_lock lock(lhs.m_useful_data_lock, rhs.m_useful_data_lock);

        std::swap(lhs.m_useful_data, rhs.m_useful_data);
    }
   

private:
    std::list<int> m_useful_data;   //Некоторый список элементов
    std::mutex m_useful_data_lock;  //Мьютекс для блокировки при копировании данных
};

DataHolder data_holder_1;
DataHolder data_holder_2;

//Переносим из второго в первый
void thread_A() {
    data_holder_1 = std::move(data_holder_2);
}

//Переносим из первого во второй
void thread_B() {
    data_holder_2 = std::move(data_holder_1);
}

int main(void) {
    //В процессе выполнения потоки блокируют друг друга
    std::thread t1(thread_A);
    std::thread t2(thread_B);

    t1.join();
    t2.join();

}

~~~
___
## <a name="e14">Livelock и try_to_lock</a>

Livelock то же что и Deadlock. Отличие в том, что при Deadlock оба заблокированных друг другом потока бесконечно ждут своей очереди на выполнение. А в Livelock бесконечно перезапускается цикл у обоих потоков, пытающийся захватить ресурс и терпящий неудачу  
Livelock- это программы, которые активно выполняют параллельные операции, но эти операции никак не влияют на продвижение состояния программы вперед

~~~C++
#include <iostream>
#include <thread>
#include <mutex>


std::mutex resourceX;
std::mutex resourceY;

void thread_A_func()
{
    while (true)
    {
        std::unique_lock<std::mutex> lockX(resourceX);          //Создаем гвард для мьютекса Х
        std::cout << "thread_A taked resourse X\n";  
        std::this_thread::sleep_for(std::chrono::seconds(1));   //Выполняем некоторую полезную работу
        std::unique_lock<std::mutex> lockY(resourceY, std::try_to_lock); //Пытаемся захватить мьютекс У
                                                                         //try_to_lock вернет true если ресурс свободен и удалось его захватить
                                                                         //false если нет. Тогда ресурс остается не захваченным

        if (lockY.owns_lock() == false) continue;   //Если мьютекс ресурса У захватить не удалось, переходим к следующей итерации цикла
        
        std::cout << "thread_A working" << std::endl; //Работа, которая должна быть выполнена после захвата обоих мьютексов
                                                      //Эта часть не выполняется, т.к. функция В так же не может получить доступ к обоим мьютексам
        break;
    }
}

void thread_B_func()
{
    while (true)
    {
        std::unique_lock<std::mutex> lockY(resourceY); //Создаем гвард для мьютекса У

        std::cout << "thread_B taked resourse Y\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));   //Выполняем некоторую полезную работу

        std::unique_lock<std::mutex> lockX(resourceX, std::try_to_lock);    //Так же пытаемся захватить мьютекс и не можем,т.к. он уже захвачен функцией А
        if (lockX.owns_lock() == false) continue;   //Условие не выполняется и бесконечно находимся в цикле

        std::cout << "thread_B working" << std::endl;

        break;
    }
}

int main(void) {
    std::thread t1(thread_A_func);
    std::thread t2(thread_B_func);

    t1.join();
    t2.join();

}
~~~
___
## <a name="e15">Starvation</a>
Starvation - это любая ситуация, когда параллельный процесс не может получить все ресурсы, необходимые для выполнения его работы  
В более широком смысле starvation обычно подразумевает наличие одного или нескольких параллельных процессов, которые несправедливо мешают одному или нескольким другим параллельным процессам выполнять работу настолько эффективно, насколько это возможно
___
## <a name="e1"></a>
call_once - для единоразового выполнения некоторого фрагмента в потоке

~~~C++
std::once_flag resource_flag;

void foo()
{
    //Если при первом запуске нужно инициализировать указатель некоторым объектом
    //Но сделать это нужно только один раз
    std::call_once(resource_flag, []()
        {
            resource_ptr.reset(new some_resource);
        });

    resource_ptr->do_something();
}
~~~
____
## <a name="e16">condition_variables</a>

~~~C++
#include<mutex>
#include<thread>
#include<iostream>
#include<list>


std::mutex mutex;
std::condition_variable cv; //Условная переменная

int x = 0;
bool ready = false;

//Генератор данных
void producer() {
    std::unique_lock<std::mutex> lock(mutex);   //Создаем гвард для мьютекса 
                                                //(unique позволяет менее затратно работать с условными переменными)

    for (; x < 100;)
    {
        cv.wait(lock, [=] { return !ready; });  //wait - освобождает мьютекс и приостанавливает поток
                                                //пока не будет получен сигнал от условной переменной из другого потока
                                                //lock - гвард, с которым работаем
                                                //[=] - лямбда - принимаем все по значению
                                                //return !ready - ждем пока ready станет равным false (это значение генерирует consumer)
        
        ++x;                //Генерируем данные
        ready = true;       //Говорим о готовности данных для считывания
        cv.notify_one();    //Сигнал условной переменной о том, что условие (ready) стало истинным
                            //Фактически передается в cv.wait consumer
    }
}

//Потребитель данных
void consumer()
{
    std::unique_lock<std::mutex> lock(mutex);   //Создаем гвард для мьютекса 

    for (; x < 100;)
    {
        cv.wait(lock, [=] { return ready; });   //Ждем пока ready станет true

        std::cout << x << std::endl;

        ready = false;      //Говорим о том, что значение считано
        cv.notify_one();     //Сигнал условной переменной о том, что условие (ready) стало ложным
                            //Фактически передается в cv.wait producer
    }
}


int main(void) {
    //В процессе выполнения потоки блокируют друг друга
    std::thread t1(producer);
    std::thread t2(consumer);

    t1.join();
    t2.join();

}
~~~
___
## <a name="e17">semaphore</a>

~~~C++
#include<mutex>
#include<thread>
#include<iostream>
#include <semaphore>

std::binary_semaphore smphSignalMainToThread(0), smphSignalThreadToMain(0);

void ThreadProc()
{
    smphSignalMainToThread.acquire();   //Уменьшение счетчика на 1
                                        //Когда счетчик равен нулю, поток блокируется и ждет увеличения счетчика
                                        //Т.к. состарта счетчик равен нулю, то процесс отсюда продолжится, когда в основном потоке его значение увеличится

    std::cout << "[thread] Got the signal\n"; 
    std::this_thread::sleep_for(std::chrono::seconds(3)); //Полезная работа
    std::cout << "[thread] Send the signal\n";
    
    smphSignalThreadToMain.release();
}

int main()
{
    std::thread thrWorker(ThreadProc);  //Создаем поток
    std::cout << "[main] Send the signal\n";

    //Увеличиваем счетчик во втором потоке, чтобы он мог продолжить работу
    smphSignalMainToThread.release();

    //Уменьшаем счетчик, делая его равным нулю. Продолжение будет, когда второй поток увеличит этот счетчик
    smphSignalThreadToMain.acquire();

    std::cout << "[main] Got the signal\n";
    thrWorker.join();
}

~~~
___
## <a name="e18">future</a>
future - позволяет ждат получения некоторого значения из потока. После его получения, ожидание прекращается и ожидающий поток продолжает работу
async - ???
~~~C++
#include<thread>
#include<iostream>
#include<future>

int do_some_work()
{
    return 50;
}

int main()
{
    std::future<int> thread_result = std::async(do_some_work);  //Выполнение останавливается в ожидании результата работы do_some_work
    std::cout << thread_result.get() << std::endl;
}

~~~
___
## <a name="e19">promice</a>
promice - Один из способов возврата значения из потока
~~~C++
#include<thread>
#include<iostream>
#include<future>
#include <numeric>


std::vector<int> numbers = { 1, 2, 3, 4, 5, 6 };

void accumulate(std::vector<int>::iterator first,
    std::vector<int>::iterator last,
    std::promise<int> accumulate_promise)
{
    //Рассчитываем сумму чисел
    int sum = std::accumulate(first, last, 0);

    //Записываем в accumulate_promise результат
    accumulate_promise.set_value(sum);

    //некая полезная работа
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

int main()
{
    std::promise<int> accumulate_promise;   //Создаем объект типа promice
    std::future<int> accumulate_future = accumulate_promise.get_future(); //Создаем объект типа future и инициализируем объектом типа promice

    //Создаем поток для функции accumulate, передаем в него вектор и accumulate_promise
    std::thread work_thread(accumulate, numbers.begin(), numbers.end(), std::move(accumulate_promise));

    //Через объект accumulate_future получаем рассчитанное в функции значение
    std::cout << "result=" << accumulate_future.get() << '\n';
    work_thread.join();
}
~~~
___
## <a name="e20">std::packaged_task</a>
std::packaged_task содержит и является поставщиком значения для future
~~~C++
#include<thread>
#include<iostream>
#include<future>


int f(int x, int y) { return std::pow(x, y); }  //Базовая функция, которая должна быть выполнена


void task_thread()
{
    std::packaged_task<int(int, int)> task(f);  //Пакуем базовую задачу, указывая ее сигнатуру и передавая в конструктор ее название
    std::future<int> result = task.get_future();    //Создаем объект типа future и считываем значение задачи

    std::thread task_td(std::move(task), 2, 10);    //Запускаем выполнение задачи с данными для расчетов
    task_td.join();

    std::cout << "task_thread:\t" << result.get() << '\n';  //Выводим результат
}

int main()
{
    task_thread();
}
~~~

packaged_task с lambda
~~~C++
#include<thread>
#include<iostream>
#include<future>
#include <numeric>


void task_lambda()
{
    std::packaged_task<int(int, int)> task([](int a, int b)
        {
            return std::pow(a, b);
        });

    std::future<int> result = task.get_future();

    task(2, 9);

    std::cout << "task_lambda:\t" << result.get() << '\n';
}

int main()
{
    task_lambda();
}
~~~

packaged_task с bind
~~~C++
#include<thread>
#include<iostream>
#include<future>
#include <numeric>


int f(int x, int y) { return std::pow(x, y); }  //Базовая функция, которая должна быть выполнена

void task_bind()
{
    std::packaged_task<int()> task(std::bind(f, 2, 11));
    std::future<int> result = task.get_future();

    task();

    std::cout << "task_bind:\t" << result.get() << '\n';
}

int main()
{
    task_bind();
}
~~~
