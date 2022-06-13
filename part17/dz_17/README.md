# Сondition
Task 1.  
Implement serial number generator for exe from archive. 

Task2.  
Find serial number, which will have MD5 hash equal to this value `96c4dda0c4a0b34262b1d91d47056f9e`

# Submission format
Source code, which prints password.

# Input data
Archive `rev.task.7z`, attached to the assignment (archive password: rev).

# Solution overview
*Task 1*   
Устанавливаем [IDA Pro](https://www.hex-rays.com/ida-pro/), распаковываем приложенный архив и перетаскиваем в него распакованную программу.   
![pic_1](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic1.png)   

Нас интересует вызов диалогового окна с вводом серийного ключа и его проверкой. Поэтому устанавливаем курсор в строку вызова окна DialogBoxParamA и нажимаем пробел   
![pic_2](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic2.png)   

Сразу после фрагмента с вызовом диалогового окна находится функция, описывающая этот процесс. Выделяем в ней любой адрес и снова жмем пробел, чтобы открыть структуру   
![pic_3](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic3.png)   

Находим фрагмент, где явно выводится результат проверки серийного номера   
![pic_4](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic4.png)

Видим, что в узле выше производится некоторая проверка, которая и приводит к выводу информации о правильности серийного ключа:  
![pic_5](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic5.png) 

Еще выше находится проверка равенства некоторого значения шести. Неуспешность данной проверки напрямую влияет на результат оценки серийного ключа.   
![pic_6](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic6.png) 

Устанавливаем в строке со сравнением breakpoint с помощью кнопки **F2** и запускаем отладку  
![pic_7](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic7.png)

Вводим какое-нибудь проверочное число, например `123` и после остановки отладки в точке breakpoint наводим мышку на сравниваемое с шестеркой значение.  
![pic_8](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic8.png) 

Как видим, в значении для сравнения находится тройка. Разумным будет предположить, что это количество цифр, введенных в качестве серийного ключа. Пробуем перезапустить программу введя число с шестью цифрами, например `123456`, видим, что данная проверка проходит успешно. Но все так же не проходит проверка в следующем узле. При более детальном изучении, видим, что этот узел вызывает некоторую функцию возвращающую bool значение. Если функция возвращает true, то серийный код верен.  
![pic_9](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic9.png) 

Снимаем предыдущий breakpoint и устанавливаем новый в строку с вызовом функции. Запускаем отладку и на вызове функции нажимаем **F7**, чтобы увидеть её. Далее, нажимая **F7** для пошагового выполнения отладки, видим, что часть узлов выполняются в цикле  
![pic_10](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic10.png)

Изучим более внимательно, что происходит внутри цикла. Первая операция, которая нас интересует это копирование (`mov`)  
![pic_11](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic11.png)

Как видим, в `al`, копируется `esi` с индексом `eax`. Т.е. в данном случае нулевой символ введенного нами серийного ключа был скопирован. В результате `al` содержит в себе `31`, что является шестнадцатиричным представлением нулевого символа введенного нами числа (т.е. "1"):  
![pic_12](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic12.png) 

Далее выполняется операция добавления (`add`) десяти к предыдущему значению. Как результат, имеем в `al` значение `41`  
![pic_13](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic13.png) 

Далее к полученному значению, добавляется текущий индекс элемента. В данном случае он равен 0, т.к. обрабатывается нулевой элемент переданного серийного кода:  
![pic_14](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic14.png) 

Далее выполняется некоторая операция добавления, которая не очевидна на текущей итерации, поскольку никак не изменяет полученный ранее результат  
![pic_15](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic15.png) 

И последняя операция, это инкрементирование индекса, фактически переход к следующему символу введенного серийного ключа.  
![pic_16](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic16.png) 

Пройдем еще одну итерацию, до операции с неочевидным результатом. Нетрудно посчитать, что для следующей итерации при обработке символа "2" справедлив следующий расчет, согласно рассмотренного алгоритма:
~~~
десятичное 2 = шестнадцатиричное 32
32 + 10 + 1(измененный индекс) = 43
~~~
И действительно, видим это значение одним из операндов рассматриваемой операции  
![pic_17](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic17.png) 

Тогда как вторым операндом оказывается результат предыдущей операции  
![pic_18](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic18.png) 

Таким образом можно прийти к выводу, что введенной серийный ключ преобразуется в некоторое значение по следующему алгоритму:

>sum<sub>i</sub> = digit<sub>i</sub> + 10 + i + sum<sub>i-1</sub>

Далее видим, что полученное значение сравнивается с числом `0x2F2`, что в десятичной системе соответствует числу `754`  
![pic_19](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic19.png) 

Следующим узлом производится проверка суммы 0 и 5 символа значению `0xDE`, что в десятичной системе соответствует `222`. К слову, отсутствие второго выхода из данного узла скорее всего говорит о том, что в случае неуспешности данной проверки, программа не выведет никакого результата.   
![pic_20](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic20.png) 

И наконец последняя проверка сравнивает сумму 2 и 3 символа значению `0xD4`, что в десятичной системе соответствует `212`  
![pic_21](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic21.png) 

Сложив вместе рассмотренные условия, получаем следующие ограничения:
~~~
total_sum = 754 (0x2F2)
digit_0 + digit_5 = 222 (0xDE)
digit_2 + digit_3 = 212 (0xD4)
20 < digit < 127 (размер таблицы ASCII, за вычетом спецобозначений)
~~~

Как видим, не хватает ограничения для последних двух символов
~~~
digit_1 + digit_4 = ?
~~~

Попробуем определить это ограничение используя имеющиеся данные (и помня, что вычисления проводятся над шестнадцатиричными числами)

~~~
sum_0 = digit_0 + 10 + 0 + 0
sum_1 = digit_1 + 10 + 1 + sum_0 = digit_1 + digit_0 + 21
sum_2 = digit_2 + 10 + 2 + sum_1 = digit_2 + digit_1 + digit_0 + 33
sum_3 = digit_3 + 10 + 3 + sum_2 = digit_3 + digit_2 + digit_1 + digit_0 + 46
sum_4 = digit_4 + 10 + 4 + sum_3 = digit_4 + digit_3 + digit_2 + digit_1 + digit_0 + 0x5A
sum_5 = digit_5 + 10 + 5 + sum_4 = digit_5 + digit_4 + digit_3 + digit_2 + digit_1 + digit_0 + 0x6F

digit_0 + digit_5 = 0xDE, тогда
sum_5 = digit_4 + digit_3 + digit_2 + digit_1 + 0x6F + 0xDE

digit_2 + digit_3 = 0xD4, тогда
sum_5 = digit_4 + digit_1 + 0x6F + 0xDE + 0xD4, или
digit_1 + digit_4 = sum_5 - 0x6F - 0xDE - 0xD4

sum_5 = total_sum = 0x2F2, тогда
digit_1 + digit_4 = 0x2F2 - 0x6F - 0xDE - 0xD4 = 0xD1 (или 209 в десятичной)
~~~

Таким образом, получено последнее ограничение digit_1 + digit_4 = 209 (0xD1) и можно приступать к написанию программы-генератора ключей.

Как видно из ограничений, мы должны сгенерировать три пары ключей. Создадим для этого функцию, возвращающую пару из двух символов, сумма которых передается как параметр:

~~~C++
std::pair<char, char> RandomPairDigit(size_t sum) {
	//some code
}
~~~

Рандом символов должен быть таким, чтобы их сумма была равна переданному параметру `sum` и ни один из символов не превышал в своем представлении размера **ASCII** таблицы (126, 127 символ это `DEL`, его так же исключаем). Таким образом определяем минимально допустимое значение для `digit_1`

~~~C++
size_t min = sum - 126;
~~~

Рандомим значение для `digit_1` в диапазоне от `min` до `126`

~~~C++
size_t digit_1 = rand() % (126 - min + 1) + min;
~~~

Очевидно, что `digit_2` вычисляется, как разница между суммой двух символов и полученного ранее `digit_1`

~~~C++
size_t digit_2 = sum - digit_1;
~~~

Полученные значения преобразуем в пару символов и возвращаем из функции
~~~C++
return std::pair<char, char>(static_cast<char>(digit_1), static_cast<char>(digit_2));
~~~

Соберем вместе полученные рандомом символы в строку
~~~C++
void PrintCode() {
	std::string code = "XXXXXX";

	//Sum of digit_0 and digit_5 is 0DE or 222 in Decimal
	auto pair_0_5 = RandomPairDigit(222);
	code[0] = pair_0_5.first;
	code[5] = pair_0_5.second;

	//Sum of digit_2 and digit_3 is 0D4 or 212 in Decimal
	auto pair_2_3 = RandomPairDigit(212);
	code[2] = pair_2_3.first;
	code[3] = pair_2_3.second;

	//digit_1 + digit_4 = 2F2 - 0DE - 0D4 - 6F = D1
	//digit_1 + digit_4 = 754 - 222 - 212 - 111 = 209
	auto pair_1_4 = RandomPairDigit(209);
	code[1] = pair_1_4.first;
	code[4] = pair_1_4.second;

	std::cout << "Password: " << code << std::endl;
}
~~~

И вызовем функцию столько раз, сколько ключей нам необходимо:

~~~C++
int main()
{
	size_t password_amount = 10;

	for (size_t i = 0; i < password_amount; i++)
		PrintCode();
}
~~~
![pic_22](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic22.png) 

Как показывает проверка, каждый из ключей подходят  
![pic_23](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic23.png)   
![pic_24](https://github.com/sotnikea/Apriorit/raw/main/part17/dz_17/img/pic24.png) 

*Task 2*  
Чтобы выполнить данное задание, необходимо преобразовывать каждый ключ в [MD5](https://ru.wikipedia.org/wiki/MD5) хэш и сравнить его с заданным хэшем. Для преобразования строки с серийным ключом был найден и использован данный исходный код: http://www.zedwood.com/article/cpp-md5-function  
Правильность работы найденного фрагмента кода проверена на контрольной строке с данным онлайн-конвертером: https://www.md5hashgenerator.com/  

Найденный фрагмент кода внедрен в написанный ранее генератор ключей. Поиск ключей ограничен моментом, когда преобразованный в MD5 ключ не окажется равным указанному хэшу:

~~~C++
std::string GetCode() {
    std::string code = "XXXXXX";

    //Sum of digit_0 and digit_5 is 0DE or 222 in Decimal
    auto pair_0_5 = RandomPairDigit(222);
    code[0] = pair_0_5.first;
    code[5] = pair_0_5.second;

    //Sum of digit_2 and digit_3 is 0D4 or 212 in Decimal
    auto pair_2_3 = RandomPairDigit(212);
    code[2] = pair_2_3.first;
    code[3] = pair_2_3.second;

    //digit_1 + digit_4 = 2F2 - 0DE - 0D4 - 6F = D1
    //digit_1 + digit_4 = 754 - 222 - 212 - 111 = 209
    auto pair_1_4 = RandomPairDigit(209);
    code[1] = pair_1_4.first;
    code[4] = pair_1_4.second;

    return code;
}

int main()
{
    while (true) {
        std::string cur_code = GetCode();
        if (md5(cur_code) == "96c4dda0c4a0b34262b1d91d47056f9e") {
            std::cout << "Find right key: " << cur_code << std::endl;
            break;
        }
    }
}
~~~

Результатом запуска получен следующий ключ, удовлетворяющий указанному хешу:
~~~
Find right key: sedplk
~~~

Готовый код обоих заданий приложен.

# Links
IDA Pro - https://www.hex-rays.com/ida-pro/  
ASCII Table -  https://www.asciitable.com/  
MD5 - https://ru.wikipedia.org/wiki/MD5  
MD5 Hash Generator - https://www.md5hashgenerator.com/  
C++ md5 function - http://www.zedwood.com/article/cpp-md5-function  
Hex Calculator - https://www.calculator.net/hex-calculator.html?number1=7D&c2op=%2B&number2=54&calctype=op&x=59&y=20  

