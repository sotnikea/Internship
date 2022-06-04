# 11. Cryptography + Blockchain
## Оглавление
1. [General blockchain info](#r1)
2. [Ethereum history](#r2)
3. [General smart contract info](#r3)
4. [API analogy](#r4)
5. [Usage mechanics](#r5)
6. [Transactions](#r6)
7. [Advantages](#r7)
8. [Storage](#r8)
9. [No concurrency](#r9)
10. [Limitations](#r10)
11. [Costly](#r11)
12. [Immutable](#r12)
13. [Примеры смарт контрактов](#r13)


## <a name="r1">General blockchain info</a>
**Blockchain** можно сравнить с *односвязным списком*. Вместо указателя на предыдущий элемент, элементы блокчейна содержат хеш-сумму предыдущего элемента. Элементы внутри блокчейна нельзя добавить, удалить или изменить без изменения других элементов структуры.  

Это наделяет блокчейн очень полезным свойством: элементы внутри блокчейна тяжело заменить, удалить или добавить. Чтобы это сделать, нужно перестроить весь блокчейн заново, начиная от измененного элемента.  

Но как правило, когда говорят о блокчейне, имеют в виду распределенную систему использующую блокчейн для хранения данных. Чаще всего такая система представляет из себя *пиринговую сеть*.   

Каждый узел сети имеет свою копию блокчейна, один из узлов создает новый блок согласно определенному набору правил и распространяет его между пирами. Если полученный блок соответствует определенному набору правил - узел добавляет его в свой блокчейн. Этот набор правил называется **консенсусом**.   

Узлам нет необходимости доверять “чужим” данным, все данные должны быть подтверждены подписью и хеш-суммами и соответствовать консенсусу. Так сеть становится действительно децентрализованной.

## <a name="r2">Ethereum history</a>
Ethereum was initially described in a white paper by Vitalik Buterin, a programmer involved with Bitcoin Magazine, in late 2013 with a goal of building decentralized applications. Formal development of the Ethereum software project began in early 2014 through a Swiss company, Ethereum Switzerland GmbH

## <a name="r3">General smart contract info</a>
Пока что я описывал блокчейн как хранилище данных, но в большинстве случаев хранилище бесполезно без некоторой бизнес-логики, способной с ним работать. К примеру, если у нас в блокчейн записывается пульс пациента, для выплаты страховки хотелось бы узнать, случился ли у пациента сердечный приступ. Конечно, можно выкачать всю цепочку данных и анализировать её отдельно, но еще лучше было бы автоматически выплатить страховку при несчастном случае. Для этого нам нужен смарт-контракт.  

Смарт-контракты взаимодействуют с блокчейном, автоматически устанавливая некоторые ограничения и условия на пользовательские данные. В общем случае, смарт-контракты похожи на правила консенсуса, но в то время как задача консенсуса - привести большое количество несвязанных пиров к общему состоянию системы, задача смарт-контракта - выяснить, к какому состоянию мы пришли и что все-таки с ним делать (выплачивать страховку или нет).

## <a name="r4">API analogy</a>
Смарт-контракты чаще всего представляют собой некоторое API для работы с пользовательскими данными. Таким образом, смарт-контракт можно сравнить с бек-энд какого-нибудь сайта. Возвращаясь к примеру со страховкой, такой смарт-контракт предоставлял бы две основных функции: одну - для получения данных о пульсе и вторую - для получения выплаты по страховке. При чем, каждый вызов функции - транзакция в сети. Каждый вызов навечно записывается в блокчейн и изменить его параметры или результаты нельзя.   

Имплементации сильно отличаются от одной сети к другой.
Самая простая имплементация смарт-контракта - это внедрить его прямо в код клиента сети. Так, например, существуют фреймворки для создания “dedicated” сетей типа *Graphene*. Более продвинутые имплементации позволяют писать свой исполняемый код, который вызывается клиентом сети. Это самый распространенный вариант. Такие смарт-контракты могут быть написаны на привычных языках программирования например `C++` в `EOS` или `Go` в `Hyperledger`, или на специальных языках, как `solidity` в `ethereum`.   

Еще один вариант исполнения смарт-контрактов - ограниченный набор специализированных инструкций, которые можно комбинировать для создания типичных контрактов. Например Zilliqa.

## <a name="r5">Usage mechanics</a>
Из-за этого, понятное дело, смарт-контракты не нацелены на прямое употребление пользователями. Чаще всего, смарт-контракт сопровождается некоторым клиентским приложением или веб-страницей. Кроме того, в зависимости от предназначения, смарт-контракт может иметь несколько вариантов таких “фронт-эндов”. Например, криптовалюта или монета должна быть доступна из различных приложений-кошельков. Тоесть при разработке нужно учитывать интерфейс нашего контракта.

## <a name="r6">Transactions</a>
Каждый вызов функции смарт-контракта является транзакцией. Это означает что при любой ошибке или при возникновении исключения внутри функции - состояние смарт контракта не изменится. Ошибочный вызов не будет иметь никакого эффекта. Результаты вызова сохранятся только если функция завершится успешно.
Например, рассмотрим смарт-контракт с одной функцией:
~~~Solidity
contract Dummy {
  uint myValue = 10;
  function setValue(uint newValue) public {
    require(newValue > myValue);
    myValue = newValue;
    if (myValue > 1000) {
      throw;
    }
  }
}
~~~
Рассмотрим несколько последовательных вызовов функции setValue

| N п/п | Вызов | Статус |       Результат       | Комментарии |
| ----- | ----- | ------ |     -------------     | ----------- |
| 1 | setValue(50) | success |       myValue = 50       | Все проверки прошли, значение myValue изменилось. |
| 2 | setValue(10) | revert |       myValue = 50       | Снова, newValue должно быть больше myValue. Ничего не произошло. |
| 3 | setValue(5000) | revert |       myValue = 50       | Первое условие проходит. Значению myValue присваивается 5000. Но затем срабатывает условие myValue > 1000. И все изменения значений отменяются. |

## <a name="r7">Advantages</a>
Одно из самых больших преимуществ смарт-контрактов - то как они вписываются в общую схему распределенной сети без доверия. Совершенно не знакомые друг с другом пользователи могут безопасно взаимодействовать посредством смарт-контракта. В том-же случае со страховой компанией, это поможет избежать мошенничества, тоесть человек не сможет подделать медицинскую карту или заключение врача. С другой стороны, это поможет избежать обмана людей, которые действительно пострадали. Но все это только при условии, что смарт-контракт правильно написан. 
Остальные преимущества в основном зависят от имплементации, но они тоже довольно часто встречаются

## <a name="r8">Storage</a>
Теоретически, смарт-контракты могут хранить неограниченное количество данных. На практике, это количество ограничивается тем, что за каждую операцию смарт-контракта необходимо внести оплату - комиссию. Тоесть нельзя сразу залить кучу терабайт данных в блокчейн, но можно не беспокоиться о масштабируемости смарт-контракта. Масштабируемость больше зависит от архитектуры конкретной сети. Правильно написанный смарт-контракт будет работать так-же эффективно с 10 пользователями как и с 1000. 

## <a name="r9">No concurrency</a>
В случае линейного блокчейн, понятно что транзакции идут в четкой последовательности. Это значит что какая-либо многопоточность в принципе не возможна. Более того, из-за транзакционности вызовов становится легко гарантировать состояние. Это одновременно и хорошо и плохо. Если два пользователя отправили одновременно по одной транзакции - мы можем гарантировать, что выполнится сначала одна, а потом - вторая. Но их порядок неопределен. 

## <a name="r10">Limitations</a>
Большое число разных сетей - следовательно большое число относительно новых инструментов разработки. Для хорошо развитых сетей с активным комьюнити чаще всего уже есть различные специализированные IDE или расширения для популярных сред разработки, но даже с ними бывают проблемы. При разработке смарт-контрактов нужно быть готовым часто менять окружение. Например, очень полезным в этом плане стал докер. Вместо того чтобы каждый раз настраивать заново ноды каждой сети, можно просто запустить контейнер, который в считанные секунды поднимет тестовую сеть.

## <a name="r11">Costly</a>
Использование смарт-контрактов часто подразумевает под собой уплату некоторых комиссий. В зависимости от сети, комиссия может быть организована по разному, но общая суть одна и та-же. Смарт-контракт не может выполнять никаких долгосрочных и дорогостоящих операций. В идеале, все операции должны выполняться за постоянное время. 

## <a name="r12">Immutable</a>
Из-за направленности на независимость смарт-контрактов часто, в сетях запрещено изменение кода смарт-контракта. Тоесть, любые ошибки и баги останутся там навсегда. Поэтому очень большое внимание должно уделяться безопасности смарт-контракта. Есть ряд хороших практик и паттернов которых стоит придерживаться, проводятся регулярные аудиты безопасности, но даже это не спасает от уязвимостей которые могут стоит миллионы долларов. Написание смарт-контракта больше сравнимо с написанием ПО для самолета или медицинского оборудования, где мало шансов на ошибку. 

## <a name="r13">Примеры смарт контрактов</a>
### Покупка в магазание и перевод денег

~~~Solidity
//SPDX-License-Identifier: MIT
pragma solidity 0.8.11;

contract MyShop{
    //Адрес владельца контракта
    address public owner;  //public после adress делает переменную открытой для ридера

    //Информация о покупателе в виде ключ=>значение (адрес покупателя=>сумма оплаты)
    mapping(address=>uint) public payments;

    //Конструктор - вызывается в момент размещения контракта в блокчейне
    constructor(){
        //msg - глобальный объект, сообщение полученное по транзакции
        //msg.sender - адрес отправителя контракта
        //ТАК КАК КОНСТРУКТОР ВЫЗЫВАЕТСЯ СОСТАВИТЕЛЕМ КОНТРАКТА
        owner = msg.sender;        
    }

    //Функция, с помощью которой покупатель что-то оплачивает
    //Публичная функция, чтобы ее можно было вызывать извне
    //payable - пометка о том, что функция может принимать деньги
    function payForItem() public payable{
        //Зачиселение денег автоматическое, на счет отправителя контракта
        //Описывать процесс в функции не нужно
        //ФУНКЦИЯ ВЫЗЫВАЕТСЯ  ПОКУПАТЕЛЕМ, поэтому msg.sender возвращаем инфу о нем
        payments[msg.sender] = msg.value;
    }

    //Функция, с помощью которой забираем все зачисленные на счет контракта деньги
    function withdrawAll() public{
        //Обрабатывать деньги могут только объекты с модификатором payable
        //Создаем временную переменную и кастуем в нее адрес владельца
        address payable _to = payable(owner);

        //Сохраняем адресс этого контракта, чтобы списать с него деньги на счет владельца
        address _thisContract = address(this);

        //Переводим деньги между адресами
        //balance - все деньги на счету указанного адреса
        _to.transfer(_thisContract.balance);
    }

}
~~~