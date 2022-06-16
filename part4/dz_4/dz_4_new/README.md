# Ошибки в реализации

1. Забыли return в конце
~~~C++
ForwardList<T>& ForwardList<T>::operator=(ForwardList<T>&& other) 
~~~
2. Доступ через точку, а не стрелку
~~~C++
ForwardList<T>::ForwardList(ForwardList&& other)
~~~
3. Список инициализации принимается по значению. Не инициализируются поля. Проверка (`head == nullptr`) - неопределённое поведение

# Обзор исправлений
В процессе...
