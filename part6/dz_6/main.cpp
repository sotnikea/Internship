#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <iostream>

#include "counter.h"

void PrintCountersValue(const Counter& c1, const Counter& c2){
    std::cout <<"Value of 1st counter: "<< c1.GetValue() << std::endl;
    std::cout <<"Value of 2nd counter: "<< c2.GetValue() << std::endl;
}

int main(int c, char **v){
    QApplication app(c,v);

    Counter c1(50);         //First counter
    Counter c2(100);        //Second counter

    //Connect signals with slots of both counters
    QObject::connect(&c1, &Counter::ToCounterSet, &c2, &Counter::FromCounterGet);
    QObject::connect(&c2, &Counter::ToCounterSet, &c1, &Counter::FromCounterGet);

    std::cout <<"--- START VALUE OF COUNTERS ---" << std::endl;
    PrintCountersValue(c1,c2);

    std::cout <<"--- INCREMENT 1ST COUNTER ---" << std::endl;
    c1.Increment();
    PrintCountersValue(c1,c2);

    std::cout <<"--- INCREMENT 2ND COUNTER ---" << std::endl;
    c2.Increment();
    PrintCountersValue(c1,c2);

    std::cout <<"--- DECREMENT 1ST COUNTER ---" << std::endl;
    c1.Decrement();
    PrintCountersValue(c1,c2);

    std::cout <<"--- DECREMENT 2ND COUNTER ---" << std::endl;
    c2.Decrement();
    PrintCountersValue(c1,c2);

    return app.exec();
}
