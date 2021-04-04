#include <exampleObserver.h>

exampleObserver::exampleObserver(Subject& monitor)
    :vid_{""},pid_{""},name_{""},action_{""},monitor_{monitor}
{
   monitor_.registerObservers(this);
}

exampleObserver::~exampleObserver(){
    monitor_.removeObserver(this);
}

void exampleObserver::update(string vid, string pid, string name,string action){
    vid_=vid;
    pid_=pid;
    name_=name;
    action_=action;

    std::cout<<"Observer 1 Notifications \n";
    std::cout<<"VID:"<<vid_<<" PID:"<<pid_<<" Name:"<<name_<<" Action:"<<action<<std::endl;
    std::cout<<"-----------------------------------------------\n";

    if(name=="T265" && (action=="Exists" || action == "Add")){
        std:cout<<"T265 is connected\n";
    }
    else if (name == "T265" && action == "Remove"){
        std::cout<<"T265 Disconnected\n";
    }
}
