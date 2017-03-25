all: car_client.o car_server.o car_controller.o
	g++ -std=c++11 car_client.o car_server.o car_controller.o -o programel
car_client.o: car_client.cpp car_client.h
	g++ -std=c++11 -I . -c car_client.cpp
car_server.o: car_server.cpp car_server.h
	g++ -std=c++11 -I . -c car_server.cpp
car_controller.o: car_controller.cpp car_controller.h
	g++ -std=c++11 -I . -c car_controller.cpp
clean:
	rm -rf *.o
