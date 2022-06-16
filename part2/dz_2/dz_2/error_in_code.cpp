#ifndef CODEREVIEWTASK_MYVECTOR_HPP     
#define CODEREVIEWTASK_MYVECTOR_HPP     

#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>                    




template <typename T>                       
class MyVector : public std::vector<T>      //Плохая идея наследоваться от стандартной библиотеки векторов потому что деструктор в ней не виртуальный
											//std::vector<int>* my = new MyVector<int>(); Здесь будет утечка
{
public:
    MyVector()
    {													
        m_ref_ptr = new size_t(1);                  
        m_names = new std::vector<std::string>();   
    }


    MyVector(const MyVector& other)     
        : std::vector<T>(other),       
        m_ref_ptr(other.m_ref_ptr),     	//Рекомендуется инициализировать переменные в том же порядке
											//в каком они объявлены. Стоило инициализировать m_names до m_ref_ptr
        m_names(other.m_names)          
    {
        (*m_ref_ptr)++;                 
    }

    ~MyVector()
    {
        if (-- * m_ref_ptr == 0)    
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


    std::pair<const T&, const std::string&> operator[](int index) const //size_t для индекса
    {
        if (index >= std::vector<T>::size())		//Стоит так же проверять вероятность отрицательного индекса
        {
            throw new std::out_of_range("Index is out of range");   //Не стоит бросать исключения по указателю. Стоит делать это по значению
																	//А принимать по константной ссылке
																	//new сам может сгенерировать ошибку скрывая основную проблему
																	//Кроме того не определен необходимы catch для throw new
        }

       
        return std::pair<const T&, const std::string&>(std::vector<T>::operator[](index), (*m_names)[index]);
    }

    const T& operator[](const std::string& name) const
    {
       
        std::vector<std::string>::const_iterator iter = std::find(m_names->begin(), m_names->end(), name);
        
        if (iter == m_names->end())
        {
            throw new std::invalid_argument(name + " is not found in the MyVector");	//Не стоит бросать исключения по указателю. Стоит делать это по значению
																						//А принимать по константной ссылке
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
    std::vector<std::string>* m_names;  
    size_t* m_ref_ptr;                 
};


#endif //CODEREVIEWTASK_MYVECTOR_HPP

