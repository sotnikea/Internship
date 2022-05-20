#ifndef COUNTER_H
#define COUNTER_H
#include <QObject>
#include <iostream>

class Counter : public QObject
{
    Q_OBJECT
    int m_value;
public:
    Counter(int startValue);    //Constructor
    void Increment();           //+1 on counter
    void Decrement();           //-1 on counter
    int GetValue() const;             //Get current value from counter
signals:
    void ToCounterSet(bool operation);

public slots:
    void FromCounterGet(bool operation);

};

#endif // COUNTER_H
