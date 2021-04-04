#pragma once

// Forward declaration
class Observer;

// Subject superclass acting as an interface 
class Subject
{
private:
    /* data */
public:
    
    virtual void registerObservers(Observer *o) = 0;
    virtual void removeObserver(Observer *o) = 0;
    virtual void notifyObservers() const =0;

    virtual ~Subject()=default;
};

