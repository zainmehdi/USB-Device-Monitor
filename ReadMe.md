
# A Generic Device Monitor for USB Peripheral Devices

This package uses libudev to monitor the status of any registered USB device on a linux machine. 

## Requirements

- libudev  
- libusb

## Registering Devices
In order to register a device you need to enter device details to a config file in the following format

```
VID,PID,Name

VID = Vendor ID of th device
PID = Product ID of the device
Name = String indicating the name of the device (User defined)

* Use lsusb to find above mentioned details
```


## Usage

```
Observer pattern has been used in this project. An observer will have to be created to recieve notifications.
Please have a look at exampleObserver.cpp , exampleObserver.h and observer.h .

Device monitor can be initialized in just 4 lines of code in your main()

// Declaring monitor object
monitor mon;

// Specify config file
mon.readConfig("/home/zain/device-monitor/include/devices.cfg");

// Declare your observer/observers
exampleObserver observer1(mon);

// Start monitor thread
mon.start();

```
