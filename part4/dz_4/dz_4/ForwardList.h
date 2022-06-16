#pragma once
#include <memory>
#include<initializer_list>
#include <iostream>

//List
template <typename T>
class ForwardList {
private:
	struct Node;										//For using in Iterator class	

	Node* head;											//pointer to the beginning of list
	size_t count;										//amount of elements in the list	

public:
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

	//class for making constant iterator
	class Iterator_const :public Iterator {
	public:		
		Iterator_const(Node* node);						//use constructor of base Iterator		
		const T& operator*() const;						//get constant reference for current node
	};
	
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

	Iterator begin() const;											//Iterator to the head of the list
	Iterator end() const;											//Iterator to the tail of the list
	Iterator_const cbegin() const;									//const iterator to the head of the list
	Iterator_const cend() const;									//const iterator to the end of the list
	
	void insert_after(const Iterator& p, const T& data);			//insert new element selected by iterator
	void erase_after(const Iterator& p);							//erase element after iterator

private:
	struct Node
	{
		T m_data;											//data
		Node* m_next;										//next element
		Node(T data) :m_data(data), m_next(nullptr) {}
		Node() :m_next(nullptr) {}
	};
};

//-------------------- FORWARDLIST ----------------------------------

//default constructor
template<typename T>
inline ForwardList<T>::ForwardList():head(nullptr),count(0){}

//destructor
template<typename T>
inline ForwardList<T>::~ForwardList()
{
	while (head) {
		pop_front();
	}
}

//constructor with initializer list
template<typename T>
inline ForwardList<T>::ForwardList(std::initializer_list<T> list)
{
	Node* cur = head;
	for (auto it = list.end() - 1; it >= list.begin(); it--) {
		push_front(*it);
	}
	count = list.size();
}

//Move constructor
template<typename T>
inline ForwardList<T>::ForwardList(ForwardList&& other)
{
	head = other->head;
	count = other->count;

	other->head = nullptr;
	other->count = 0;
}

//Move operator =
template<typename T>
inline ForwardList<T>& ForwardList<T>::operator=(ForwardList<T>&& other) noexcept
{
	if (&other == this) {
		return *this;
	}
	while (count) {
		this->pop_front();
	}

	count = other.count;
	head = other.head;
	other.count = 0;
	other.head = nullptr;
}

//Add element in front by lvalue
template<typename T>
inline void ForwardList<T>::push_front(const T& data)
{
	Node* tmp = new Node(data);

	//First element in list
	if (head==nullptr) {
		head = tmp;
	}
	else {
		tmp->m_next = head;
		head = tmp;
	}

	count++;
}

//Add element in front by rvalue
template<typename T>
inline void ForwardList<T>::push_front(T&& data)
{

	Node* tmp = new Node(std::move(data));

	//First element in list
	if (head == nullptr) {
		head = tmp;
	}
	else {
		tmp->m_next = head;
		head = tmp;
	}

	count++;
}

//delete element at the begin of list
template<typename T>
inline void ForwardList<T>::pop_front()
{
	if (count == 0)
		return;
	if (count == 1) {
		delete head;
		head = nullptr;
		count = 0;
		return;
	}

	Node* tmp = head;
	head = head->m_next;
	delete tmp;
	count--;
}

//get first element from list
template<typename T>
inline T ForwardList<T>::front() const{
	return head->m_data;
}

//return amount of element at list
template<typename T>
inline size_t ForwardList<T>::size() const
{
	return count;
}

//check if list empty
template<typename T>
inline bool ForwardList<T>::empty() const {
	return count == 0;
}

//clear list
template<typename T>
inline void ForwardList<T>::clear() {
	ForwardList::~ForwardList();
}

//insert new element selected by iterator
template<typename T>
inline void ForwardList<T>::insert_after(const Iterator& p, const T& data) {
	Iterator it(head);
	Iterator prev(head);	//for previous elem
	while (it != p) {
		prev = it;
		it++;
		if (it == nullptr) throw std::exception("Iterator out of list");
	}
	Node* tmp = new Node(data);
	count++;

	prev.m_node->m_next = tmp;
	tmp->m_next = it.m_node;
}

//erase element after iterator
template<typename T>
inline void ForwardList<T>::erase_after(const Iterator& p) {
	Iterator it(head);
	Iterator next(head);	//for next elem
	while (it != p) {		
		it++;
		if (it == nullptr) throw std::exception("Iterator out of list");
	}
	next = it; 
	next++;	//element for deleting
	if (next == nullptr) throw std::exception("Iterator on the last element. Nothing to erase");
		
	count--;

	it.m_node->m_next = next.m_node->m_next;
	delete next.m_node;
	
}


//------------------------ ITERATOR --------------------------------

template<typename T>
inline ForwardList<T>::Iterator::Iterator(Node* node):m_node(node) {}

//Iterator to the head of the list
template<typename T>
typename inline ForwardList<T>::Iterator ForwardList<T>::begin() const
{
	return Iterator(head);
}

//Iterator to the tail of the list
template<typename T>
typename inline ForwardList<T>::Iterator ForwardList<T>::end() const
{
	return Iterator(nullptr);
}

//operator ==
template<typename T>
inline bool ForwardList<T>::Iterator::operator==(const Iterator& other) const
{
	if (this == &other) {
		return true;
	}
	return m_node == other.m_node;
}

//operator !=
template<typename T>
inline bool ForwardList<T>::Iterator::operator!=(const Iterator& other) const
{
	return !operator==(other);
}

//get reference for current node
template<typename T>
inline T& ForwardList<T>::Iterator::operator*() const
{
	if (m_node) {
		return m_node->m_data;
	}
	else {
		throw std::exception("Iterator is outside of list");
	}
}

//increment iterator postfix form
template<typename T>
inline void ForwardList<T>::Iterator::operator++()
{
	if (m_node) {
		m_node = m_node->m_next;
	}
}

//increment iterator prefix form
template<typename T>
inline void ForwardList<T>::Iterator::operator++(int)
{
	if (m_node) {
		m_node = m_node->m_next;
	}
}


//-------------------- ITERATOR_CONST --------------------------------

//Iterator_const constructor
template<typename T>
inline ForwardList<T>::Iterator_const::Iterator_const(Node* node) : Iterator(node) {}

//const iterator to the head of the list
template<typename T>
typename inline ForwardList<T>::Iterator_const ForwardList<T>::cbegin() const
{
	return Iterator_const(head);
}

//get constant reference for current node
template<typename T>
inline const T& ForwardList<T>::Iterator_const::operator*() const
{
	if (ForwardList::Iterator::m_node) {
		return ForwardList::Iterator::m_node->m_data;
	}
	else {
		throw std::exception("Iterator is outside of list");
	}
}


