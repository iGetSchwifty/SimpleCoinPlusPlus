#warning_list = -fsanitize=undefined -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wsign-conversion -Wsign-promo -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused
#
#	-fsanitize=address  this is where something is going wrong (an underflow?) so want to figure it out but its getting late and need to go to bed soon.. I know there is still some memory things I need to do in order to get this project to where I want it to be
#
# Possible Warnings that Jason Turner recommends
#
#-Wold-style-cast
#-Wcast-align
#-Wunused
#-Woverloaded-virtual
#-Wconversion
#-Wpedantic
#-Wsign-conversion
#-Wmisleading-indentation
#-Wduplicated-cond
#-Wduplicated-branches
#-Wlogical-op
#-Wnull-dereference
#-Wuseless-cast
#-Wdouble-promotion
#-Wformat=2
#-Wlifetime

# $(warning_list)


all: app

app: main.o sha256.o miner.o wallet.o server.o base64.o ecc.o
	g++ -o app main.o miner.o wallet.o sha256.o server.o base64.o ecc.o -lboost_system

main.o: main.cpp
	g++ -std=c++17 -c main.cpp

miner.o: ./simpleCoin++/miner/miner.cpp ./simpleCoin++/libs/json/json.hpp
	g++ -std=c++17 -c ./simpleCoin++/miner/miner.cpp

wallet.o: ./simpleCoin++/wallet/wallet.cpp ./simpleCoin++/libs/json/json.hpp
	g++ -std=c++17 -c ./simpleCoin++/wallet/wallet.cpp

sha256.o: ./simpleCoin++/libs/sha256/sha256.cpp ./simpleCoin++/libs/sha256/sha256.h
	g++ -std=c++17 -c ./simpleCoin++/libs/sha256/sha256.cpp

server.o: ./simpleCoin++/SimpleWebServer/Server.cpp ./simpleCoin++/SimpleWebServer/Server.hpp
	g++ -std=c++17 -c ./simpleCoin++/SimpleWebServer/Server.cpp

base64.o: ./simpleCoin++/libs/base64/base64.cpp ./simpleCoin++/libs/base64/base64.h
	g++ -std=c++17 -c ./simpleCoin++/libs/base64/base64.cpp

ecc.o: ./simpleCoin++/libs/easy-ecc/ecc.c ./simpleCoin++/libs/easy-ecc/ecc.h
	gcc -c ./simpleCoin++/libs/easy-ecc/ecc.c

#	Remove Test Stuff
#	Once you figure out the lifecyle stuff since you can create a valid transaction within the test and not the app
#	However, they seem to be running the same logic and code.
#	Has something to do with the way the threads are set up or something so looking into it but at least I know Im not crazy now...
#
test.o: test.cpp
	g++ -std=c++17 -c test.cpp $(warning_list)

test: test.o
	g++ -o test test.o ecc.o base64.o wallet.o -lboost_system $(warning_list)

cleanTest:
	rm test.o test

clean:
	rm *.o app