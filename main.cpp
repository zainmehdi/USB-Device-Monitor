#include "monitor.h"
#include "exampleObserver.h"
#include <signal.h>



int main(int argc, char *argv[]){


    // Declaring monitor object
    monitor mon;

    // Registering devices
    //mon.add_device("03e7","2150","T265");
    //mon.add_device("05ca","0368","RicoH Theta");

    mon.readConfig("/home/zain/device-monitor/include/devices.cfg");

    exampleObserver observer1(mon);

    // Starting monitor thread
    mon.start();


}
