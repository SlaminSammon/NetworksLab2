Inside this repositiory one will find two programs which form a scheduling server client. 
The programs were developed on Ubuntu 14.04, but should run on any build of Linux.
The compiler used was g++11.
The server must be started first and can be started by going to the server directory and typing:
$ make 
then type 
$ ./serverRun

The client can then be started by going to the client directory and typing 
$ make 
then type
$ ./clientStart {ipaddress of the machine that is running the server}
