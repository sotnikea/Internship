# Condition
Your task is to find as many mistakes and drawbacks in this code (according to the theme material) as you can. Annotate these mistakes with comments.

# Restrictions
- MyVector stores a collection of objects with their names. For each object `T`, `MyVector` stores T's name as `std::string`.  
- Several objects can have similar name.
- `operator[](const std::string& name)` should return the first object with the given name.

# Need to implement
Once you have found all the mistakes, rewrite the code so it would not change its original purpose and it would contain no mistakes. Try to make the code more efficient without premature optimization.  
You can change MyVector interface completely, but there are several rules:
1. You should correctly and fully implement **copy-on-write** idiom.
2. `std::pair<const T&, const std::string&> operator[](int index) const` must take constant time at worst.
3. `const T& operator[](const std::string& name) const` should be present.
4. `both operator[]` should have non-const version.
5. Your implementation should provide all the member types of `std::vector`.
6. Your implementation should provide the following functions:
    - `begin(), cbegin(), end(), cend()`
    - `empty(), size()`
    - `reserve(), clear()`

# Code for review and refinement
~~~C++
#ifndef CODEREVIEWTASK_MYVECTOR_HPP
#define CODEREVIEWTASK_MYVECTOR_HPP

#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

template <typename T>
class MyVector : public std::vector<T>
{
public:
    MyVector()
    {
        m_ref_ptr = new size_t(1);
        m_names = new std::vector<std::string>();
    }

    MyVector(const MyVector& other)
        : std::vector<T>(other),
          m_ref_ptr(other.m_ref_ptr),
          m_names(other.m_names)
    {
        (*m_ref_ptr)++;
    }

    ~MyVector()
    {
        if (--*m_ref_ptr == 0)
        {
            delete m_ref_ptr;
            delete m_names;
        }
    }

    void push_back(const T& obj, const std::string& name)
    {
        copy_names();

        std::vector<T>::push_back(obj);
        m_names->push_back(name);
    }

    std::pair<const T&, const std::string&> operator[](int index) const
    {
        if (index >= std::vector<T>::size())
        {
            throw new std::out_of_range("Index is out of range");
        }

        return std::pair<const T&, const std::string&>(std::vector<T>::operator[](index), (*m_names)[index]);
    }

    const T& operator[](const std::string& name) const
    {
        std::vector<std::string>::const_iterator iter = std::find(m_names->begin(), m_names->end(), name);
        if (iter == m_names->end())
        {
            throw new std::invalid_argument(name + " is not found in the MyVector");
        }

        return std::vector<T>::operator[](iter - m_names->begin());
    }

private:
    void copy_names()
    {
        if (*m_ref_ptr == 1)
        {
            return;
        }

        size_t* temp_ref_ptr = new size_t(1);
        std::vector<std::string>* temp_names = new std::vector<std::string>(*m_names);

        (*m_ref_ptr)--;
        m_ref_ptr = temp_ref_ptr;
        m_names = temp_names;
    }

private:
    // Use copy-on-write idiom for efficiency (not a premature optimization)
    std::vector<std::string>* m_names;
    size_t* m_ref_ptr;
};


#endif //CODEREVIEWTASK_MYVECTOR_HPP
~~~

# Delivery method
- Base code with comments describing the errors found
- Modified project

# Solution overview
В предложенном решении обнаружены следующие ошибки  
1. Плохая идея наследоваться от стандартной библиотеки векторов потому что деструктор в ней не виртуальный 
~~~C++
class MyVector : public std::vector<T>
~~~
Здесь будет утечка
~~~C++
std::vector<int>* my = new MyVector<int>(); 
~~~

2. Рекомендуется инициализировать переменные в том же порядке в каком они объявлены. Стоило инициализировать `m_names` до `m_ref_ptr`

~~~C++
private:
    std::vector<std::string>* m_names;  
    size_t* m_ref_ptr;       
public:
    MyVector(const MyVector& other):          
        std::vector<T>(other),       
        m_ref_ptr(other.m_ref_ptr),
        m_names(other.m_names)          
    {
        (*m_ref_ptr)++;                 
    }
~~~

3. При работе с `STL` контейнерами для их длины и индекса стоит использовать тип `size_t`

~~~C++
std::pair<const T&, const std::string&> operator[](int index) const;
~~~

4. Не стоит бросать исключения по указателю. Стоит делать это по значению, а принимать по константной ссылке. New сам может сгенерировать ошибку скрывая основную проблему. Кроме того не определен необходимый `catch` для `throw new`

~~~C++
throw new std::out_of_range("Index is out of range");
~~~
~~~C++
throw new std::invalid_argument(name + " is not found in the MyVector");
~~~

Исправленная реализация предполагает исключение наследования от `std::vector` и самостоятельную реализацию необходимого функционала.  
Обрабатываемые классом данные, помещены в структуру  
~~~C++
private:
	struct MyVectorValue {
		std::vector<T>* m_data;	//data for save
		std::vector<std::string>* m_names;	//name of saving data
		size_t refCount;	//amount of references
		MyVectorValue();	//Default constructor
		MyVectorValue(const MyVectorValue* other);	//Constructor with parametrs
		~MyVectorValue();	//Destructor
	};
	MyVectorValue* value;
~~~
Реализованы:  
Конструктор по умолчанию, конструктор копирования и декструктор, освобождающий память, только если ссылок на неё более не осталось
~~~C++
MyVector();
MyVector(const MyVector& other);
~MyVector();
~~~

Метод, реализующий добавление нового элемента в вектор
~~~C++
void push_back(const T& obj, const std::string& name);
~~~
Методы, возвращающие итераторы и константные итераторы на начало и конец вектора
~~~C++
typename std::vector<T>::iterator begin();
typename std::vector<T>::const_iterator cbegin();
typename std::vector<T>::iterator end();
typename std::vector<T>::const_iterator cend();
~~~
Методы, возвращающие текущий размер вектора и количество зарезервированного в нем места
~~~C++
size_t size();
size_t capacity();
~~~
Метод очищающий вектор, и метод резервирующий дополнительно место в нем
~~~C++
void reserve(size_t extra);
void clear();
~~~
Метод возвращающий информацию о наличии или отсутствии в векторе данных. А так же метод, возвращающий хранящиеся в нем данные по индексу
~~~C++
bool empty();
std::pair<const T, std::string> at(int index);
~~~

В основе реализации лежит идиома copy-on-write, предполагающая выделение новой памяти под данные только в случае внесения в них изменений. При чтении на данные лишь создается новая ссылка. Данный принцип реализован в методе `copy_datas`.
~~~C++
void copy_datas()
{
	//if there is just one pointer for used memory
	//object can be modified without copy it data
	if (value->refCount == 1) {
		return;
	}

	--value->refCount;	//Old memory lost one pointer
	MyVectorValue* temp = new MyVectorValue(value);	//Get new memory for this object using copy old data in constructor
	value = temp; //Save new memory in base pointer
}
~~~

Так же в проекте реализована перегрузка оператора [] для чтения и записи данных. При реализации обнаружена проблема. По логике программы чтение данных с помощью [] не должно выделять новую память и копировать данные вектора. Тогда как запись с помощью оператора [] наоборот должно это делать.  
Тем не менее по умолчанию перегрузка оператора [] не позволяет сделать разграничение для событий чтения и записи.  
Чтобы решить данную проблему был реализован proxy класс:
~~~C++
//Proxy class for managing read and write operator with operator[]
	class MyVectorProxy {
	public:
		MyVectorProxy(MyVector& vec, size_t index); //constructor
		MyVectorProxy& operator=(const MyVectorProxy& rhs); //copy constructor

		//lvalue for write operation by data's name
		MyVectorProxy& operator=(std::pair<const T&, const std::string&> p);
		//lvalue for write operation by data's index
		MyVectorProxy& operator=(const T& val);
		//rvalue for read operation by data's name
		operator std::pair<T, std::string>() const;
		//rvalue for read operation by data's index
		operator T() const;

	private:
		MyVector& theMyVector;
		size_t dataIndex;
	};
~~~
И на основании созданного proxy класса реализованы перегрузки оператора [] для доступа к данным по индексу и по имени
~~~C++
//create object of proxy class for managing read/write operation of operator[]
	MyVectorProxy operator[](size_t index);	
	const MyVectorProxy operator[](size_t index) const;
	MyVectorProxy operator[](std::string name);
	const MyVectorProxy operator[](std::string name) const;
~~~
В файле main.cpp присутствует набор тестов, позволяющий проверить работу каждого метода. Так же файл содержит функционал для проверки возможных утечек памяти.

# Result evaluation
50 из 100   
[Ошибки в реализации и внесенные исправления](dz_2_new/)

# Links
Скотт Майерс. Наиболее эффективное использование С++. 35 Новых рекомендаций по улучшению ваших программ и проектов
