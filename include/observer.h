#pragma once

#include <iostream>
// Any class that subcribes to Monitor should implement the update method
// to be able to receive notifications. Monitor class will report updates
// in the form of (VID,PID,Name) of the device with the status change (plugged
// or unplugged)


class Observer
{
private:
    /* data */
public:
    virtual void update(std::string vid, std::string pid, std::string name, std::string action) =0;
    virtual ~Observer() = default;
};

