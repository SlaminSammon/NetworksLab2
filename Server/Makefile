serverRun: server.o Appointment.o User.o functions.o
	g++ -std=c++11 server.o Appointment.o User.o functions.o -o serverRun

server.o: server.cpp
	g++ -std=c++11 -c server.cpp

Appointment.o: Appointment.cpp Appointment.h
	g++ -std=c++11 -c Appointment.cpp

User.o: User.cpp User.h
	g++ -std=c++11 -c User.cpp

functions.o: functions.cpp functions.h
	g++ -std=c++11 -c functions.cpp

clean:
	rm *.o serverRun