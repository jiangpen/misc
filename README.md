# misc
Misc working related stuff 
bluetoothconsole_git: it is a Visual Studio 2015 C++ program to demostrate the Windows winsock/Bluetooth API to use RFCOMM to communicate with a Bluetooth device.
/////////////////////////////////////////////////////////////
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
