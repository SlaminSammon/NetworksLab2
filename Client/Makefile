clientStart: client.o clientFunctions.o
	g++ -std=c++11 client.o clientFunctions.o -o clientStart

client.o: client.cpp
	g++ -std=c++11 -c client.cpp

clientFunctions.o: clientFunctions.cpp clientFunctions.h
	g++ -std=c++11 -c clientFunctions.cpp

clean:
	rm *.o clientStart