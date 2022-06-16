# Условие
Написать свою реализацию односвязного списка (как эталон того, что нужно реализовать, можно посмотреть на `std::forward_list`)

# Требования
У списка должны быть определены следующие операции:
- Базовые операции работы со списком: конструирование, вставка элементов в начало списка, итерирование по списку, размер списка, удаление элементов.
- Операции перемещения (Перемещающий конструктор, перемещающее присваивание)
- Метод вставки элементов в начало должен иметь перегрузку для rvalue ссылки, и корректно перемещать объекты.
- Конструирование списка с помощью конструктора, принимающего `std::initializer_list`.
- Список должен быть шаблонным, и работать не только и примитивными значениями, но и с объектами (и корректно их освобождать)
- В идеале нужно стремиться к интерфейсу класса как у `std::forward_list`, но реализовывать абсолютно все методы не требуется -- кастомный аллокатор, и `emplace_*` методы не обязательны (Но минусом не будет). `sort`, `merge`, `unique`  делать не нужно.
- BONUS POINTS: Реализовать метод `split_when`, позволяющий разделить список на два, который определяет позицию, начиная с какого элемента нужно делать разделение, основываясь на `bool` значении, возвращаемого из передаваемой ф-ции-предиката. Сам предикат можно передавать с помощью `std::function`, пример:
~~~C++
// Predicate type
template<typename T>
using SplitPredicate = std::function<bool(const T& value)>;

// Proposed function signature
template<typename T>
class LinkedList
{
    // ... Other methods ...
    LinkedList<T> split_when(SplitPredicate<T> condition);
    // ... Other methods ...
};

// Expected usage
LinkedList<int> original = { 1, 42, 3 };
auto tail = original.split_when(
    [](const int& value)){ return value == 42; }
);
// original now is { 1 }
// tail now is { 42, 3 }
~~~

# Формат сдачи
Файл с реализацией или ссылка на работу в гит

# Примечания
-	Утечки памяти/неправильная работа с памятью тоже оцениваются; если для работы с памятью будут корректно использоваться умные указатели (`std::unique_ptr`) - будет плюсом.
-	Обязательно проверяйте вашу работу на собираемость/работоспособность перед отправкой.
-	Для разработки рекомендуется использовать Visual Studio 2019. Но если очень хочется и выхода совсем нет - можно использовать что угодно :)

# Обзор решения
Реализован шаблонный класс
~~~C++
template <typename T>
class ForwardList
~~~

Класс содержит в себе структуру, описывающую сохраняемые данные, ссылку на следующий элемента, а также конструкторы, необходимые для инициализации этих данных:

~~~C++
struct Node
	{
		T m_data;											//data
		Node* m_next;										//next element
		Node(T data) :m_data(data), m_next(nullptr) {}
		Node() :m_next(nullptr) {}
	};
~~~

В классе реализованы следующие методы:
~~~C++
ForwardList();													//default constructor
~ForwardList();													//destructor
ForwardList(std::initializer_list<T> list);						//constructor with initializer list
ForwardList(ForwardList&& other);								//Move constructor
ForwardList<T>& operator =(ForwardList<T>&& other) noexcept;	//Move operator =
void push_front(const T& data);									//push lvalue
void push_front(T&& data);										//push rvalue
void pop_front();												//delete in front	
size_t size() const;											//amount of elements
bool empty() const;												//check if list empty
void clear();													//clear list
T front() const;												//get first element from list
~~~

Для реализации итерирования по созданному списку, внутри класса, а так же для логических операций над его нодами, возможности инкрементирования и разыменования значения, реализован вложенный класс `Iterator`
~~~C++
//For iteration throw the list
class Iterator {
public:		
    Iterator(Node* node);							//iterator constructor		
    bool operator ==(const Iterator& other) const;	//operator ==		
    bool operator !=(const Iterator& other) const;	//operator !=		
    T& operator*() const;							//get reference for current node		
    void operator++();								//increment iterator postfix form		
    void operator++(int);							//increment iterator prefix form

public:
    Node* m_node;
};
~~~

Для реализации методов, возвращающих константные итераторы на начало и конец списка, реализован вложенный класс `Iterator_const`, наследующий функционал класса `Iterator`
~~~C++
//class for making constant iterator
class Iterator_const :public Iterator {
public:		
    Iterator_const(Node* node);			//use constructor of base Iterator		
    const T& operator*() const;			//get constant reference for current node
};
~~~

Таким образом получены методы возвращающие итераторы на элементы списка:
~~~C++
Iterator begin() const;					//Iterator to the head of the list
Iterator end() const;					//Iterator to the tail of the list
Iterator_const cbegin() const;			//const iterator to the head of the list
Iterator_const cend() const;			//const iterator to the end of the list
~~~

А так же методы вставки элементов на указанные позиции и удаление элементов по указанной позиции
~~~C++
void insert_after(const Iterator& p, const T& data);	//insert new element selected by iterator
void erase_after(const Iterator& p);                    //erase element after iterator
~~~

В файле main.cpp приложен набор тестов, для проверки работы реализованных методов.

# Оценка результата
90 из 100   
[Ошибки в реализации и внесенные исправления](dz_4_new/)
