#ifndef MONITOR_H
#define MONITOR_H

#include <subject.h>
#include <observer.h>
#include <libudev.h>
#include <thread>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <map>
#include <set>
#include <mutex>
#include <fstream>



/**
 * @brief The monitor class
 * @details This class uses libudev to monitor status of connected USB devices. It inherits
 * subject superclass and overrides its methods
 *
 * @see http://www.voidcn.com/article/p-nubqsspe-bdd.html
 * @see https://gavv.github.io/articles/libudev-usb/
 *
 */
class monitor : public Subject
{

public:

    /**
    * @brief Class constructor
    */
    explicit monitor();
    ~monitor();

    /**
    * @brief Run method for Thread.
    */
    void run();

    /**
     * @brief This function will keep a track of connected devices and register/unregister
     * respective nodes
     *
     * Whenever a new device is added it will get the device node and store it.
     * Similarly when a device is disconnected we will acquire node for the
     * disconnected device and emit an appropriate signal.
     * @param dev
     * @see process_devices(struct udev_device* dev)
     */
    void process_nodes(struct udev_device* dev);


    /**
     * @brief This function emcapsulates the functionality of
     * process_nodes(struct udev_device* dev)
     * @param dev
     * @see print_devices(struct udev_device* dev)
     */
    void process_devices(struct udev_device* dev);


    /**
     * @brief This function is just needed to check if any device was already connected
     * when the system started so it could be enumartaed and node could be acquired.
     * @param udev
     * @see process_devices_start(struct udev_device* dev)
     */
    void enumerate_devices(struct udev* udev);

    /**
     * This is the main function that will do all the monitoring operation
     * and call relevant functions. Please have a look at definition for further
     * details.
     * @param udev
     */
    void monitor_devices(struct udev* udev);


    struct udev* udev;

    /**
     * @brief For acquiring node for connected/disconnected device
     * @see process_devices(struct udev_device* dev)
     */
    const char* node=nullptr;

    /**
     * @brief Flag that indicated that thread needs to be terminated.
     * It is activated from the destructor of mainprocess class.
     * @see mainprocess::~mainprocess()
     */
    bool endmonitorflag=false;

    /**
     * @brief devices A map containing VID,PID and names for devices to be monitored
     * Tuple is definced as <VID,Name>
     * Key is the PID of the device to be monitored
     */
    std::map<std::string,std::tuple<std::string, std::string>> device_list;

    /**
     * @brief nodes Stores nodes for all the devices to be monitored
     * Key is the PID and value is the name of the device registered
     */
    std::map<std::string,std::string> nodes;

    /**
     * @brief add_device Adds a new device to be monitored
     * @param vid
     * @param pid
     * @param name
     */
    void add_device(std::string vid, std::string pid, std::string name);

    /**
     * @brief remove_device Removes a device from list
     * @param vid
     * @param pid
     * @param name
     */
    void remove_device(std::string pid);

    /**
     * @brief list_devices Prints devices that are currently to be monitored
     */
    void list_devices();

    // Overriding superclass's methods
    void registerObservers(Observer *o) override ;
    void removeObserver(Observer *o) override ;
    void notifyObservers() const override ;

    /**
     * @brief observers_ Stores list of observers
     */
    std::vector<Observer*> observers_;

    /**
     * @brief Start Starts running monitor in its own thread
     */
    void start();

    /**
     * @brief Stops monitor thread
     */
    void stopMonitoring();

    /**
     * @brief Reads config file and parses response in data structure
     */
    void readConfig(std::string filename);
    
    /**
     * @brief Enables or disables debugging
     */
    bool enable_debugging=false;

private:
    std::string vid_;
    std::string pid_;
    std::string name_;
    std::string action_;
    bool endMonitoring =false;
    std::mutex mutex_;

    std::thread t_;




};

#endif // MONITOR_H
