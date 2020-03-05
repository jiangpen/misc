# misc
Misc working related stuff 

* bluetoothconsole_git: it is a Visual Studio 2015 C++ program to demostrate the Windows winsock/Bluetooth API to use RFCOMM to communicate with a Bluetooth device.


* C++ lambda
void do_something(std::function<void(int )> callback, int x)
{

  callback(x);
}
int num_callbacks=0;
    auto callback=[&](int x){
      printf("%d,%d\n", num_callbacks++,x);
    };
do_something(callback,1);
do_something(callback,2);
do_something(callback,3);

* ubuntu assign USBtty to certain fixed name with given USB port.
1) plug in ttyUSB device for example /dev/ttyUSB2 and use command "udevadm info -a -n /dev/ttyUSB2| grep '{devpath}'"
   It will show something like 
    ATTRS{devpath}=="9"
    ATTRS{devpath}=="0"
Or if it connected to a HUB, may show
     ATTRS{devpath}=="1.4.4.4.1"
    ATTRS{devpath}=="1.4.4.4"
    ATTRS{devpath}=="1.4.4"
    ATTRS{devpath}=="1.4"
    ATTRS{devpath}=="1"
    ATTRS{devpath}=="0"
2) create a rule file for example 99-usbserial.rules in /etc/udev/rules.d.
SUBSYSTEMS=="usb",  ATTRS{devpath}=="9", MODE="0666", SYMLINK+="ttyUSB109"
3) unplug and plug in the USB, you can see /dev/ttyUSB109. 
4) Any ttyUSB device connected to this given USB port will show as /dev/ttyUSB109. 
5) label the port and USB cablle,so you know which device is which.
