CC=g++
CFLAGS=-Wall - g

server: server.cpp
	g++ -Wall -g -c server.cpp
	g++ -Wall -g -c cli_checks_utils.cpp
	g++ -Wall -g -c connect_utils.cpp
	g++ -Wall -g -c epoll_manipulation_utils.cpp
	g++ -Wall -g -c alex_simple_protocol.cpp
	g++ -Wall -g -c communication_utils.cpp
	g++ server.o communication_utils.o cli_checks_utils.o connect_utils.o epoll_manipulation_utils.o alex_simple_protocol.o -o server

subscriber: subscriber.cpp
	g++ -Wall -g -c subscriber.cpp
	g++ -Wall -g -c cli_checks_utils.cpp
	g++ -Wall -g -c connect_utils.cpp
	g++ -Wall -g -c epoll_manipulation_utils.cpp
	g++ -Wall -g -c alex_simple_protocol.cpp
	g++ -Wall -g -c communication_utils.cpp
	g++ subscriber.o communication_utils.o cli_checks_utils.o connect_utils.o epoll_manipulation_utils.o alex_simple_protocol.o -o subscriber

run_server:
	./server 4040

run_subscriber:
	./subscriber 1 127.0.0.1 4040

clean:
	rm server.o \
	   cli_checks_utils.o \
	   subscriber.o \
	   server \
	   subscriber