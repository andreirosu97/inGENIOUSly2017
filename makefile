all: car_client.o car_server.o car_controller.o car_connection.o car_motor.o car_rf.o main.o
	g++ -std=c++11 -lwiringPi -pthread car_client.o car_server.o car_controller.o car_connection.o car_motor.o main.o car_rf.o rc522.o -o programel
car_client.o: car_client.cpp car_client.h
	g++ -std=c++11 -I . -c car_client.cpp
car_server.o: car_server.cpp car_server.h
	g++ -std=c++11 -I . -c car_server.cpp
car_controller.o: car_controller.cpp car_controller.h
	g++ -std=c++11 -I . -c car_controller.cpp
car_connection.o: car_connection.cpp car_connection.h
	g++ -std=c++11 -I . -c car_connection.cpp
car_motor.o: car_motor.cpp car_motor.h
	g++ -std=c++11 -I . -c car_motor.cpp
main.o: main.cpp
	g++ -std=c++11 -I . -c main.cpp
rc522.o: rc522.cpp
	g++ -std=c++11 -I . -c rc522.cpp
car_rf.o: car_rf.cpp
	g++ -std=c++11 -I . -c car_rf.cpp
clean:
	rm -f *.o
