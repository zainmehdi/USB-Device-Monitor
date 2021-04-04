
#include <observer.h>
#include <subject.h>
#include <iostream>

using namespace std;

class exampleObserver : public Observer{

public:
    explicit exampleObserver(Subject &monitor);
    ~exampleObserver();

    void update(string vid, string pid, string name,string action);

    // Just for example purposes. You are free to design your observer in any way
    // you please. Just make sure it follows the observer template
    std::string T265_status = "Disconnected", RicoH_status="Connected";

private:
    string vid_;
    string pid_;
    string name_;
    string action_;
    Subject& monitor_;

};
