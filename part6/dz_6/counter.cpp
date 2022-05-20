#include "counter.h"

Counter::Counter(int startValue):m_value(startValue){}

void Counter::Increment(){
    m_value++;
    emit ToCounterSet(true);
}

void Counter::Decrement(){
    m_value--;
    emit ToCounterSet(false);
}

int Counter::GetValue() const{
    return m_value;
}

void Counter::FromCounterGet(bool operation){
    if(operation)
        m_value++;
    else
        m_value--;
}
