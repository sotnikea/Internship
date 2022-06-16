# Условие
*Задание 1*

Дан готовый метод, нужно покрыть его юнит тестами
~~~C++
size_t GetProductsAmount(std::vector<int> prices, int amountOfMoney)
{
    std::sort(prices.begin(), prices.end());

    int sum = 0;
    for (size_t i = 0; i < prices.size(); ++i)
    {
        sum += prices[i];
        if (sum > amountOfMoney)
        {
            return i;
        }
    }

    return prices.size();
}
~~~
*Задание 2*  
Нужно реализовать метод `IsLeapYear` по условию ниже и покрыть его тестами (рекомендуется попробовать применить **TDD**)

Given a year, report if it is a leap year.  
The tricky thing here is that a leap year in the Gregorian calendar occurs:  
on every year that is evenly divisible by 4  
  except every year that is evenly divisible by 100  
    unless the year is also evenly divisible by 400  
For example, 1997 is not a leap year, but 1996 is. 1900 is not a leap year, but 2000 is.  

Чтобы не проверять отрицательные числа сделайте входной параметр `unsigned`:
~~~C++
bool IsLeapYear(unsigned int year);
~~~

# Требования
- Задания выполнять используя google test
- В ответе достаточно архива с файлами исходного кода, весь проект кидать не нужно
- Не забудьте проверить граничные случаи и все пути выполнения

# Формат сдачи
Готовые файлы с решением или ссылка на гит

# Обзор решения
*Задание 1*  

Для начала развернем новый проект. Создадим в нем 3 файла:
- GetProductsAmount.h
- GetProductsAmount.cpp
- main.cpp

В GetProductsAmount.h размещаем прототип функции, которую необходимо протестировать, а так же подключаем все необходимые для нее библиотеки
~~~C++
#pragma once
#include <iostream>
#include <vector>
#include<algorithm>

size_t GetProductsAmount(std::vector<int> prices, int amountOfMoney);
~~~

В GetProductsAmount.cpp подключаем GetProductsAmount.h и размещаем в нем реализацию рассматриваемой функции
~~~C++
#include "GetProductsAmount.h"

size_t GetProductsAmount(std::vector<int> prices, int amountOfMoney)
{    
    //Сортируем стоимость продуктов по возрастанию
    std::sort(prices.begin(), prices.end());

    //Переменная для общей стоимости товаров
    int sum = 0;


    for (size_t i = 0; i < prices.size(); ++i)
    {
        sum += prices[i];           //Суммируем стоимость товаров
        if (sum > amountOfMoney)    //Если стоимость выше имеющихся денег
        {
            return i;               //Возвращаем количество продуктов, которые можем купить
        }
    }

    
    return prices.size();       //Если цикл закончился, можем купить все продукты, возвращаем размер вектора
    return 0;
}
~~~

В main.cpp подключаем GetProductsAmount.h и объявляем функцию main. Наполнение этой функции для нас не важно:
~~~C++
#include "GetProductsAmount.h"


int main(void) {

    return 0;
}
~~~

Теперь перейдем к подключению google test. Нажимаем правой кнопкой мыши по заголовку проекта в обозревателе решений  
![image_1](https://github.com/sotnikea/Internship_Apriorit/raw/main/part5/dz_5/img/pic1.png)   

Выбираем в списке проектов Google Test  
![image_2](https://github.com/sotnikea/Internship_Apriorit/raw/main/part5/dz_5/img/pic2.png)

Задаем проекту название и нажимаем кнопку "Создать"  
![image_3](https://github.com/sotnikea/Internship_Apriorit/raw/main/part5/dz_5/img/pic3.png)

В выборе конфигурации ничего не меняем  
![image_4](https://github.com/sotnikea/Internship_Apriorit/raw/main/part5/dz_5/img/pic4.png)

Открываем свойства добавленного проекта Google Test   
![image_5](https://github.com/sotnikea/Internship_Apriorit/raw/main/part5/dz_5/img/pic5.png)

И указываем, что не будем использовать precompiled headers  
![image_6](https://github.com/sotnikea/Internship_Apriorit/raw/main/part5/dz_5/img/pic6.png)

В строке Preprocessor Definition нажимаем кнопку Edit  
![image_7](https://github.com/sotnikea/Internship_Apriorit/raw/main/part5/dz_5/img/pic7.png)

И указываем значение: _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING  
![image_8](https://github.com/sotnikea/Internship_Apriorit/raw/main/part5/dz_5/img/pic8.png)

В файле test.cpp созданного google test проекта подключаем файл с функцией, для тестирования  
![image_9](https://github.com/sotnikea/Internship_Apriorit/raw/main/part5/dz_5/img/pic9.png)

На этом этапе попытка билда проекта уже должна быть успешной.  
Перейдем к написанию тестов. Воспользуемся паттерном [AAA](https://medium.com/@pjbgf/title-testing-code-ocd-and-the-aaa-pattern-df453975ab80).   
В первом тесте проверим ситуацию, когда вектор цен в рассматриваемой функции пуст. Ожидаемое количество товаров, которое мы сможем купить в этом случае естественно должно быть равно 0:

~~~C++
TEST(GetProductsAmountTest, VectorOfPriceIsEmpty) {
	//Arrange
	std::vector<int> prices{};
	int amountOfMoney = 0;

	//Act
	size_t res = GetProductsAmount(prices, amountOfMoney);

	//Assert
	ASSERT_EQ(res, 0);
}
~~~

Теперь рассмотрим ситуацию, когда денег достаточно на покупку всех товаров:  
~~~C++
TEST(GetProductsAmountTest, MoneyEnoughForEverithing) {
	//Arrange
	std::vector<int> prices{ 10,1,20,4 };
	int amountOfMoney = 40;

	//Act
	size_t res = GetProductsAmount(prices, amountOfMoney);

	//Assert
	ASSERT_EQ(res, 4);
}
~~~

Теперь рассмотрим ситуацию, когда денег достаточно только на часть товаров
~~~C++
TEST(GetProductsAmountTest, MoneyNOTEnoughForEverithing) {
	//Arrange
	std::vector<int> prices{ 10,5,20,3 };
	int amountOfMoney = 1;

	//Act
	size_t res = GetProductsAmount(prices, amountOfMoney);

	//Assert
	ASSERT_EQ(res, 0);
}
~~~

Так же стоит рассмотреть вариант, когда несколько продуктов имеют одинаковую стоимость
~~~C++
TEST(GetProductsAmountTest, ProductWithSamePrice) {
	//Arrange
	std::vector<int> prices{ 1,1,1,1 };
	int amountOfMoney = 3;

	//Act
	size_t res = GetProductsAmount(prices, amountOfMoney);

	//Assert
	ASSERT_EQ(res, 3);
}
~~~

Ситуация, когда денег достаточно только на один продукт
~~~C++
TEST(GetProductsAmountTest, MoneyEnoughForOneProduct) {
	//Arrange
	std::vector<int> prices{ 10,5,20,3 };
	int amountOfMoney = 4;

	//Act
	size_t res = GetProductsAmount(prices, amountOfMoney);

	//Assert
	ASSERT_EQ(res, 1);
}
~~~

Ситуация, когда денег хватает на несколько продуктов:
~~~C++
TEST(GetProductsAmountTest, MoneyEnoughForFewProducts) {
	//Arrange
	std::vector<int> prices{ 6,2,7,3,4,5,1 };
	int amountOfMoney1 = 4;
	int amountOfMoney2 = 5;
	int amountOfMoney3 = 6;
	int amountOfMoney4 = 9;
	int amountOfMoney5 = 10;

	//Act
	size_t res1 = GetProductsAmount(prices, amountOfMoney1);
	size_t res2 = GetProductsAmount(prices, amountOfMoney2);
	size_t res3 = GetProductsAmount(prices, amountOfMoney3);
	size_t res4 = GetProductsAmount(prices, amountOfMoney4);
	size_t res5 = GetProductsAmount(prices, amountOfMoney5);

	//Assert
	ASSERT_EQ(res1, 2);
	ASSERT_EQ(res2, 2);
	ASSERT_EQ(res3, 3);
	ASSERT_EQ(res4, 3);
	ASSERT_EQ(res5, 4);

}
~~~

И ситуацию, когда программа функция должна правильно выбрать два товара по более дешевой цене вместо одного более дорогого:
~~~C++
TEST(GetProductsAmountTest, TwoProductsWithLowerPriceOrOneMoreExpensive) {
	//Arrange
	std::vector<int> prices{ 2,1,1 };
	int amountOfMoney = 2;

	//Act
	size_t res = GetProductsAmount(prices, amountOfMoney);

	//Assert
	ASSERT_EQ(res, 2);
}
~~~

Запуск тестов показывает, что все они проходят успешно:  
![image_10](https://github.com/sotnikea/Internship_Apriorit/raw/main/part5/dz_5/img/pic10.png)

*Задание 2*
TDD или разработка через тестирование, предполагает фактически написание теста, который должна выполнять функция, но функционал для этого теста еще не реализован. В последствии функционал реализуется и формируется новый тест. Попробуем выполнить это условие.  

Начнем с условия: "on every year that is evenly divisible by 4". Напишем для этого тест:
~~~C++
TEST(IsLeapYearTest, YearThatIsEvenlyDivisibleBy4) {
	//Correct
	ASSERT_TRUE(IsLeapYear(4));
	ASSERT_TRUE(IsLeapYear(1008));
	ASSERT_TRUE(IsLeapYear(2004));

	//Wrong
	ASSERT_FALSE(IsLeapYear(1));
	ASSERT_FALSE(IsLeapYear(1002));
	ASSERT_FALSE(IsLeapYear(2003));
}
~~~
Естественно тест при запуске не выполняется, поэтому реализовываем функционал для данного условия:
~~~C++
bool IsLeapYear(unsigned int year) {
	return year % 4 == 0;	
}
~~~

Теперь с тестом все отлично. Переходим к следующему условию: "except every year that is evenly divisible by 100". Тест для него будет выглядеть следующим образом:
~~~C++
TEST(IsLeapYearTest, ExceptEveryYearThatIsEvenlyDivisibleBy100) {
	//Correct
	ASSERT_TRUE(IsLeapYear(104));
	ASSERT_TRUE(IsLeapYear(1108));
	ASSERT_TRUE(IsLeapYear(2012));

	//Wrong
	ASSERT_FALSE(IsLeapYear(100));
	ASSERT_FALSE(IsLeapYear(1000));
	ASSERT_FALSE(IsLeapYear(2200));
}
~~~
Теперь дополняем функционал функции, чтобы этот тест выполнялся:

~~~C++
bool IsLeapYear(unsigned int year) {
	return year % 4 == 0 && year % 100 != 0;	
}
~~~
При запуске видим, что оба теста проходят успешно. Значит новый функционал никак не изменил работу более ранней части функции. Переходим к последнему условию: "unless the year is also evenly divisible by 400". Тест будет иметь следующий вид
~~~C++
TEST(IsLeapYearTest, UnlessTheYearIsAlsoEvenlyDivisibleBy400) {
	//Correct
	ASSERT_TRUE(IsLeapYear(400));
	ASSERT_TRUE(IsLeapYear(1600));
	ASSERT_TRUE(IsLeapYear(2000));

	//Wrong
	ASSERT_FALSE(IsLeapYear(100));
	ASSERT_FALSE(IsLeapYear(1100));
	ASSERT_FALSE(IsLeapYear(2300));	
}
~~~
А функция получит следующее дополнение:
~~~C++
bool IsLeapYear(unsigned int year) {
	return year % 4 == 0 && year % 100 != 0 || year % 400 == 0;	
}
~~~

При запуске все тесты проходят успешно. Значит задание выполнено верно.

# Оценка результата
100 из 100  
Реализация не требует правок

# Ссылки

Настройка Google test - https://www.youtube.com/watch?v=uxyfXU63Qpw   
AAA pattern - https://medium.com/@pjbgf/title-testing-code-ocd-and-the-aaa-pattern-df453975ab80   
TDD - https://ru.wikipedia.org/wiki/%D0%A0%D0%B0%D0%B7%D1%80%D0%B0%D0%B1%D0%BE%D1%82%D0%BA%D0%B0_%D1%87%D0%B5%D1%80%D0%B5%D0%B7_%D1%82%D0%B5%D1%81%D1%82%D0%B8%D1%80%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D0%B5   









