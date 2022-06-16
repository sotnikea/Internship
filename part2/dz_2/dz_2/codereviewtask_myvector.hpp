#ifndef CODEREVIEWTASK_MYVECTOR_HPP     
#define CODEREVIEWTASK_MYVECTOR_HPP     

#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <iterator>

template <typename T>                       
class MyVector {
public:
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

public:
	MyVector();	//Default constructor
	MyVector(const MyVector& other);	//Copy constructor for operation like MyVector my2(my)
	~MyVector();	//Destructor
	void push_back(const T& obj, const std::string& name);	//Add new element in vector
	typename std::vector<T>::iterator begin();	//Return iterator to fisrt element of data vector
	typename std::vector<T>::const_iterator cbegin();	//Return const_iterator to first element of data vector
	typename std::vector<T>::iterator end();	//Return iterator to the end of data vector
	typename std::vector<T>::const_iterator cend();	//Return const_iterator to the end of data vector
	bool empty();	//return if data vector is empty
	size_t size();	//return size of data vector
	void reserve(size_t extra); //reserve extra space in data vector
	size_t capacity();	//return information about exist space in vectors
	void clear();	//clear data
	std::pair<const T, std::string> at(int index);	//get pair by index

	//create object of proxy class for managing read/write operation of operator[]
	MyVectorProxy operator[](size_t index);	
	const MyVectorProxy operator[](size_t index) const;
	MyVectorProxy operator[](std::string name);
	const MyVectorProxy operator[](std::string name) const;

	friend class MyVectorProxy;	//needed to have permission change private value in MyVectorValue
private:
	void copy_datas();	//manage reference to common object and copy data if it needed

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
};

//----------- MyVectorValue ---------------------

//Constructor for data structure
template<typename T>
inline MyVector<T>::MyVectorValue::MyVectorValue() :
	m_data(new std::vector<T>()),
	m_names(new std::vector<std::string>()),
	refCount(1) {}

//Constructor with parametrs for data structure
template<typename T>
inline MyVector<T>::MyVectorValue::MyVectorValue(const MyVectorValue* other) :
	m_data(new std::vector<T>(*(other->m_data))),
	m_names(new std::vector<std::string>(*(other->m_names))),
	refCount(1) {}

//Destructor for data structure
template<typename T>
inline MyVector<T>::MyVectorValue::~MyVectorValue()
{
	delete m_data;
	delete m_names;
}

//Default constructor of data structure just create new instance for MyVectorValue
template<typename T>
inline MyVector<T>::MyVector() : value(new MyVectorValue()) {}

//----------------- MyVector --------------------

//Copy constructor of MyVector
template<typename T>
inline MyVector<T>::MyVector(const MyVector& other):
	value(other.value)	//Copy instance with all that pointers
{
	++value->refCount;	//+one more object using the same data in memory
}

//Destructor of MyVector
template<typename T>
inline MyVector<T>::~MyVector()
{
	//If amount of objects that using this memory became zero
	//destroy instance of structure
	//Destructor of structure relise memory for both vectors
	if (--value->refCount == 0) {	
		delete value;
	}
}

//Add new element in vector
template<typename T>
inline void MyVector<T>::push_back(const T& obj, const std::string& name)
{
	copy_datas();
	value->m_data->push_back(obj);
	value->m_names->push_back(name);
}

//Return iterator to fisrt element of data vector
template<typename T>
inline typename std::vector<T>::iterator MyVector<T>::begin()
{
	copy_datas();
	return value->m_data->begin();
}

//Return const_iterator to first element of data vector
template<typename T>
inline typename std::vector<T>::const_iterator MyVector<T>::cbegin()
{
	return value->m_data->begin();
}

//Return iterator to the end of data vector
template<typename T>
inline typename std::vector<T>::iterator MyVector<T>::end()
{
	//non-constant vector give posibbility to change data, so copy it
	copy_datas();
	return value->m_data->end();
}

//Return const_iterator to the end of data vector
template<typename T>
inline typename std::vector<T>::const_iterator MyVector<T>::cend()
{
	return value->m_data->end();
}

//return if data vector is empty
template<typename T>
inline bool MyVector<T>::empty()
{
	return value->m_data->empty();
}

//return size of data vector
template<typename T>
inline size_t MyVector<T>::size()
{
	return value->m_data->size();
}

//reserve extra space in data vector
template<typename T>
inline void MyVector<T>::reserve(size_t extra)
{
	//reserve operation allocate extra space in data vector
	//and in fact change it structure
	//so copy datas befor operation
	copy_datas();
	value->m_data->reserve(extra);
	//vector fills by pair, so reserve extra space also in names vector
	value->m_names->reserve(extra);
}

//return information about exist space in vectors
template<typename T>
inline size_t MyVector<T>::capacity()
{	
	return value->m_data->capacity();
}

//clear data
template<typename T>
inline void MyVector<T>::clear()
{
	copy_datas();
	value->m_data->clear();
	value->m_names->clear();
}

//get pair by index
template<typename T>
inline std::pair<const T, std::string> MyVector<T>::at(int index)
{
	return std::pair<const T, std::string>(value->m_data->at(index), value->m_names->at(index));
}

//create object of proxy class for managing read/write operation of operator[]
template<typename T>
inline typename MyVector<T>::MyVectorProxy MyVector<T>::operator[](size_t index)
{
	//if index is out of data vector size
	if (index >= value->m_data->size() || index < 0)
	{
		throw std::out_of_range("Index is out of range");
	}
	return MyVectorProxy(*this, index);
}

//create const object of proxy class for managing read/write operation of operator[]
template<typename T>
inline typename const MyVector<T>::MyVectorProxy MyVector<T>::operator[](size_t index) const
{
	//if index is out of data vector size
	if (index >= value->m_data->size() || index < 0)
	{
		throw std::out_of_range("Index is out of range");
	}

	return MyVectorProxy(const_cast<MyVector&>(*this), index);
}

//create object of proxy class for managing read/write operation of operator[]
template<typename T>
inline typename MyVector<T>::MyVectorProxy MyVector<T>::operator[](std::string name)
{
	std::vector<std::string>::const_iterator iter = std::find(value->m_names->begin(), value->m_names->end(), name);

	//if name doesn't exist throw exception
	if (iter == value->m_names->end())
	{
		throw std::invalid_argument(name + " is not found in the MyVector");
	}

	//copy_datas();

	int index = iter - value->m_names->begin();
	return MyVectorProxy(*this, index);
}

//create const object of proxy class for managing read/write operation of operator[]
template<typename T>
inline typename const MyVector<T>::MyVectorProxy MyVector<T>::operator[](std::string name) const
{
	std::vector<std::string>::const_iterator iter = std::find(value->m_names->begin(), value->m_names->end(), name);

	//if name doesn't exist throw exception
	if (iter == value->m_names->end())
	{
		throw new std::invalid_argument(name + " is not found in the MyVector");
	}

	//copy_datas();

	int index = iter - value->m_names->begin();
	return MyVectorProxy(const_cast<MyVector&>(*this), index);
}

//manage reference to common object and copy data if it needed
template<typename T>
inline void MyVector<T>::copy_datas()
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
//------------------- MyVectorProxy --------------------------

//Proxy class constructor
template<typename T>
inline MyVector<T>::MyVectorProxy::MyVectorProxy(MyVector& vec, size_t index) :
	theMyVector(vec), dataIndex(index) {}

//proxy class copy constructor
template<typename T>
inline typename MyVector<T>::MyVectorProxy& MyVector<T>::MyVectorProxy::operator=(const MyVectorProxy& rhs)
{
	if (theMyVector.value == rhs.theMyVector.value &&
		theMyVector.value->m_data->at(dataIndex) == rhs.theMyVector.value->m_data->at(rhs.dataIndex))
		return *this;
	theMyVector.copy_datas();
	theMyVector.value->m_data->at(dataIndex) = rhs.theMyVector.value->m_data->at(rhs.dataIndex);
	return *this;
}

//lvalue for write operation by data's name
template<typename T>
inline typename MyVector<T>::MyVectorProxy& MyVector<T>::MyVectorProxy::operator=(std::pair<const T&, const std::string&> p)
{
	theMyVector.copy_datas();

	theMyVector.value->m_data->at(dataIndex) = p.first;
	theMyVector.value->m_names->at(dataIndex) = p.second;

	return *this;
}

//lvalue for write operation by data's index
template<typename T>
inline typename MyVector<T>::MyVectorProxy& MyVector<T>::MyVectorProxy::operator=(const T& val)
{
	theMyVector.copy_datas();

	theMyVector.value->m_data->erase(theMyVector.value->m_data->begin() + dataIndex);
	theMyVector.value->m_data->insert(theMyVector.value->m_data->begin() + dataIndex, val);

	return *this;
}

//rvalue for read operation by data's index
template<typename T>
inline MyVector<T>::MyVectorProxy::operator T() const
{
	return theMyVector.value->m_data->at(dataIndex);
}

//rvalue for read operation by data's name
template<typename T>
inline MyVector<T>::MyVectorProxy::operator std::pair<T, std::string>() const
{
	return std::pair<T, std::string>(theMyVector.value->m_data->at(dataIndex), theMyVector.value->m_names->at(dataIndex));

}
#endif //CODEREVIEWTASK_MYVECTOR_HPP


