all: car_client.o car_server.o car_controller.o car_connection.o main.o
	g++ -std=c++11 -pthread car_client.o car_server.o car_controller.o car_connection.o main.o -o programel
car_client.o: car_client.cpp car_client.h
	g++ -std=c++11 -pthread -I . -c car_client.cpp
car_server.o: car_server.cpp car_server.h
	g++ -std=c++11 -I . -c car_server.cpp
car_controller.o: car_controller.cpp car_controller.h
	g++ -std=c++11 -I . -c car_controller.cpp
car_connection.o: car_connection.cpp car_connection.h
	g++ -std=c++11 -I . -c car_connection.cpp
main.o: main.cpp
	g++ -std=c++11 -I . -c main.cpp
clean:
	rm -f *.o
