#include "monitor.h"


#define SUBSYSTEM "usb"

monitor::monitor()
{
    std::cout<<"Device Monitor Starting .... \n";

}

monitor::~monitor(){
   std::cout<<"Monitor Shutting Down ....\n";
}

void monitor::run(){

    // Initialize Udev
    udev=udev_new();

    if(!udev){
       std::cout<<"udev_new() failed\n";
       return;
    }

    // Check already connected devices and enumerate them
    enumerate_devices(udev);

    // Start monitoring registered devices
    monitor_devices(udev);

    // Unref udev before exiting the program
    udev_unref(udev);

}

void monitor::process_nodes(udev_device *dev){

    const char* action = udev_device_get_action(dev);
    if (! action)
        action = "exists";


    const char* vendor = udev_device_get_sysattr_value(dev, "idVendor");
    if (! vendor)
        vendor = "0000";

    const char* product = udev_device_get_sysattr_value(dev, "idProduct");
    if (! product)
        product = "0000";


    // We need to get device node here. Once the device is detected we need to monitor this node.
    // Whenever a device is connected / disconnected udev will notify us of the node. By keeping
    // a track of this node we can easily monitor its status
    node= udev_device_get_devnode(dev);


    if(device_list.find(product)!=device_list.end() && strcmp(action,"add")==0){

        name_ = std::get<1>(device_list.find(product)->second).c_str();
        vid_ = vendor;
        pid_= product;
        action_="Add";

        nodes.insert(std::make_pair(node,name_));
        std::cout<<"Sensor Added:"<<name_<<" PID:"<<pid_<<std::endl;
        std::cout<<"-----------------------------------------------\n";
        notifyObservers();

    }

    if(nodes.find(node)!=nodes.end() && strcmp(action,"remove")==0){

        name_ = nodes.find(node)->second.c_str();
        vid_ = vendor;
        pid_= product;
        action_="Remove";

        std::cout<<"Sensor Removed:"<<name_<<" PID:"<<pid_<<std::endl;
        std::cout<<"-----------------------------------------------\n";

        nodes.erase(node);
        notifyObservers();

    }

    // Printing device information    
    if(enable_debugging){
            std::cout<<
            udev_device_get_subsystem(dev)<<
            udev_device_get_devtype(dev)<<
            action<<
            "VID:"<<
            vendor<<
            "PID:"<<
            product<<
            udev_device_get_devnode(dev);
    }

}


void monitor::process_devices(udev_device *dev){


    if (dev) {
        if (udev_device_get_devnode(dev)){
            process_nodes(dev);
        }

        udev_device_unref(dev);

    }


}


// This function will enumerate devices that are already connected when
// program starts and store node ids in process_devices_start function
void monitor::enumerate_devices(struct udev *udev){
    struct udev_enumerate* enumerate = udev_enumerate_new(udev);

    udev_enumerate_add_match_subsystem(enumerate, SUBSYSTEM);
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry* devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry* entry;

    udev_list_entry_foreach(entry, devices) {
        const char* path = udev_list_entry_get_name(entry);
        struct udev_device* dev = udev_device_new_from_syspath(udev, path);

        if (dev) {
            if (udev_device_get_devnode(dev)){

                const char* vendor = udev_device_get_sysattr_value(dev, "idVendor");
                const char* product = udev_device_get_sysattr_value(dev, "idProduct");

                if(device_list.find(product)!=device_list.end()){
                    nodes.insert(std::make_pair(udev_device_get_devnode(dev),std::get<1>(device_list.find(product)->second)));

                    // Notifying Observers of the devices that are already connected
                    name_ = std::get<1>(device_list.find(product)->second);
                    vid_ = vendor;
                    pid_= product;
                    action_="Exists";
                    notifyObservers();

                }

            }
        }

        //process_devices_start(dev);                    //nodes.push_back(std::get<1>(device_list.find(product)->second).c_str());

    }

    

    udev_enumerate_unref(enumerate);

}



// This is the main function that will perform monitoring.
void monitor::monitor_devices(struct udev *udev){


    struct udev_monitor* mon = udev_monitor_new_from_netlink(udev, "udev");

    udev_monitor_filter_add_match_subsystem_devtype(mon, SUBSYSTEM, NULL);
    udev_monitor_enable_receiving(mon);

    int fd = udev_monitor_get_fd(mon);

    while (1) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        // Creating a timer for "select" to timeout,otherwise it keeps
        // on blocking and thread termination cant be done.
        struct timeval tv;
        tv.tv_sec=2;
        tv.tv_usec=0;

        // Select is used for network monitoring in linux systems.
        // google can serve better information in this domain.
        // We are using a blocking call here. Select will block until
        // a new device event (add/remove) is received

        ////////////////////////////////// Blocking call part //////////////////////////////////////
        //                                                                                        //
        //        int ret = select(fd+1, &fds, NULL, NULL,NULL);  // This was the blocking call   //
        //         if (ret <= 0)                                                                  //
        //            break;                                                                      //
        //        else{                                                                           //
        //            if (FD_ISSET(fd, &fds)) {                                                   //
        //                struct udev_device* dev = udev_monitor_receive_device(mon);             //
        //                process_devices(dev);                                                   //
        //            }                                                                           //
        //        }                                                                               //
        ////////////////////////////////////////////////////////////////////////////////////////////



        ///////////////////////// Non BLocking Interface ///////////////////////////////////

        int ret = select(fd+1, &fds, NULL, NULL,&tv);    // This is non blocking call, it will timeout after every two seconds

        if(ret>0){
            if (FD_ISSET(fd, &fds)) {
                struct udev_device* dev = udev_monitor_receive_device(mon);
                process_devices(dev);
            }
        }


        if(endMonitoring){
            return;
        }


    }

}

void monitor::add_device(std::string vid, std::string pid, std::string name){
    auto device = std::make_tuple(vid,name);
    device_list.insert(std::make_pair(pid,device));
}


void monitor::list_devices(){
    if(!device_list.empty()){
        for(auto &i:device_list){
            std::cout<<"PID:"<<i.first.c_str()<<" VID:"<<std::get<0>(i.second).c_str()<<" Name:"<<std::get<1>(i.second).c_str()<<std::endl;

        }
    }

}

void monitor::remove_device(std::string pid){

        device_list.erase (pid);
}


void monitor::registerObservers(Observer *o){
    observers_.push_back(o);
}

void monitor::removeObserver(Observer *o){
    observers_.erase(std::find(observers_.begin(),observers_.end(),o));
}

void monitor::notifyObservers() const{
    for(auto o:observers_){
        o->update(vid_,pid_,name_,action_);
    }
}

void monitor::start(){

    t_=std::thread(&monitor::run,this);
    t_.join();
}

void monitor::stopMonitoring(){

    std::lock_guard<std::mutex> lock (mutex_);
    endMonitoring=true;
}

void monitor::readConfig(std::string filename){

    // std::ifstream is RAII, i.e. no need to call close
    std::ifstream cFile (filename);
    std::string delimiter = ",";
    if (cFile.is_open())
    {
        std::string line;
        while(getline(cFile, line)){
            line.erase(std::remove_if(line.begin(), line.end(), isspace),
                                 line.end());
            if(line[0] == '#' || line.empty())
                continue;

            auto delimiterPos = line.find(delimiter);
            auto VID = line.substr(0, delimiterPos);
            line.erase(0,delimiterPos+delimiter.length());

            delimiterPos = line.find(",");
            auto PID = line.substr(0, delimiterPos);
            line.erase(0,delimiterPos+delimiter.length());

            delimiterPos = line.find(",");
            auto Name = line.substr(0, delimiterPos);

            //std::cout <<"Name: "<<Name << " " <<"VID:"<< VID <<" "<<"PID:"<<PID<< '\n';
            add_device(VID,PID,Name);
        }
        
    }
    else {
        std::cerr << "Couldn't open config file for reading.\n";
    }
}
