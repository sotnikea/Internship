#ifndef CODEREVIEWTASK_MYVECTOR_HPP     
#define CODEREVIEWTASK_MYVECTOR_HPP    

#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <iterator>
#include <memory>

template <typename T>
class MyVector
{
	using vector_data = std::vector<std::pair<T, std::string>>;	//save pair of T data and string name

public:
	MyVector():m_data(std::make_shared<vector_data>()){}	//constructor	
	MyVector& operator =(const MyVector& other) = delete;	//denied operator =
	MyVector& operator =(MyVector&& other) = delete;		//denied operator =

	//Because of shared_ptr control copying
	MyVector(const MyVector& other) = default;	//copy constructor
	MyVector(MyVector&& other) = default;		//move constructor
	~MyVector(void) = default;					//destructor	
	
	const std::pair<const T&, const std::string&> operator[](size_t index) const;	//const overload [size_t]	
	std::pair<T, std::string>& operator[](size_t index);							//overload [size_t]
	const T& operator[](const std::string& name) const;								//const overload [string]		
	T& operator[](const std::string& name);											//overload [string]

	void push_back(const T& obj, const std::string& name) noexcept;					//add new element in vector
	std::pair<const T&, const std::string&> at(size_t index) const;					//get pair by index
	typename std::vector<std::pair<T, std::string>>::iterator begin() noexcept;		//Return iterator to fisrt element of data vector	
	typename std::vector<std::pair<T, std::string>>::const_iterator cbegin() const ;//Return const_iterator to first element of data vector	
	typename std::vector<std::pair<T, std::string>>::iterator end() noexcept;		//Return iterator to the end of data vector	
	typename std::vector<std::pair<T, std::string>>::const_iterator cend() const;	//Return const_iterator to the end of data vector	
	bool empty() const;																//return if data vector is empty	
	size_t size() const;															//return size of data vector	
	void reserve(size_t extra);														//reserve extra space in data vector	
	size_t capacity() const;														//return information about exist space in vectors	
	void clear();																	//clear data

private:
	void copy_datas();	//copy data vector, in case of operation write

private:	
	std::shared_ptr<vector_data> m_data{};
};


//add new element in vector
template <typename T>
inline void MyVector<T>::push_back(const T& obj, const std::string& name) noexcept
{
	copy_datas();
	this->m_data->emplace_back(obj, name);
}

//const overload [size_t]
template <typename T>
const std::pair<const T&, const std::string&> MyVector<T>::operator[](size_t index) const
{
	if (index >= this->m_data->size() || index < 0)
	{
		throw std::out_of_range("Index is out of range");
	}
	return this->m_data->operator[](index);
}

//overload [size_t]
template <typename T>
std::pair<T, std::string>& MyVector<T>::operator[](size_t index)
{
	if (index >= this->m_data->size() || index < 0)
	{
		throw std::out_of_range("Index is out of range");
	}
	copy_datas();	//becouse changed data
	return this->m_data->operator[](index);
}

//const overload [string]
template <typename T>
const T& MyVector<T>::operator[](const std::string& name) const
{
	//going throw vector of data pair and find taked string name
	auto iter = std::find_if(m_data->begin(), m_data->end(),
		[name](auto it) {
			return (it.second == name);
		}
	);

	//if name doesn't exist throw exception
	if (iter == m_data->end())
	{
		throw new std::invalid_argument(name + " is not found in the MyVector");
	}
	int index = iter - m_data->begin();

	return this->m_data->operator[](index).first;
}

//overload [string]
template <typename T>
T& MyVector<T>::operator[](const std::string& name)
{
	//going throw vector of data pair and find taked string name
	auto iter = std::find_if(m_data->begin(), m_data->end(),
		[name](auto it) {
			return (it.second == name);
		}
	);

	//if name doesn't exist throw exception
	if (iter == m_data->end())
	{
		throw new std::invalid_argument(name + " is not found in the MyVector");
	}

	int index = iter - m_data->begin();

	copy_datas();	//return to ref to value, so make copy

	return this->m_data->operator[](index).first;
}

//get pair by index
template <typename T>
std::pair<const T&, const std::string&> MyVector<T>::at(size_t index) const
{
	return this->m_data->operator[](index);
}

//Return iterator to fisrt element of data vector
template <typename T>
typename std::vector<std::pair<T, std::string>>::iterator MyVector<T>::begin() noexcept
{
	copy_datas();
	return this->m_data->begin();
}

//Return const_iterator to first element of data vector
template <typename T>
typename std::vector<std::pair<T, std::string>>::const_iterator MyVector<T>::cbegin() const {
	return this->m_data->begin();
}

//Return iterator to the end of data vector
template <typename T>
typename std::vector<std::pair<T, std::string>>::iterator MyVector<T>::end() noexcept
{
	copy_datas();
	return this->m_data->end();
}

//Return const_iterator to the end of data vector
template <typename T>
typename std::vector<std::pair<T, std::string>>::const_iterator MyVector<T>::cend() const {
	return this->m_data->end();
}

//return if data vector is empty
template <typename T>
bool MyVector<T>::empty() const {
	return m_data->empty();
}

//return size of data vector
template <typename T>
size_t MyVector<T>::size() const {
	return m_data->size();
}

//reserve extra space in data vector
template <typename T>
void MyVector<T>::reserve(size_t extra)
{
	//reserve operation allocate extra space in data vector
	//and in fact change it structure
	//so copy datas before operation
	copy_datas();
	m_data->reserve(extra);
}

//return information about exist space in vectors
template <typename T>
size_t MyVector<T>::capacity() const {
	return m_data->capacity();
}

//clear data
template <typename T>
void MyVector<T>::clear()
{
	copy_datas();
	m_data->clear();
}

//copy data vector, in case of operation write
template <typename T>
void MyVector<T>::copy_datas()
{
	//if there is just one pointer for used memory
	//object can be modified without copy it data
	if (this->m_data.use_count() == 1) {
		return;
	}
	//Just copy current data
	this->m_data = std::make_shared<vector_data>(*this->m_data);
}

#endif //CODEREVIEWTASK_MYVECTOR_HPP