warning_list = -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wsign-conversion -Wsign-promo -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused



all: app

app: main.o sha256.o miner.o wallet.o server.o base64.o ecc.o
	g++ -o app main.o miner.o wallet.o sha256.o server.o base64.o ecc.o -lboost_system $(warning_list)

main.o: main.cpp
	g++ -std=c++17 -c main.cpp $(warning_list)

miner.o: ./simpleCoin++/miner/miner.cpp ./simpleCoin++/libs/json/json.hpp
	g++ -std=c++17 -c ./simpleCoin++/miner/miner.cpp $(warning_list)

wallet.o: ./simpleCoin++/wallet/wallet.cpp ./simpleCoin++/libs/json/json.hpp
	g++ -std=c++17 -c ./simpleCoin++/wallet/wallet.cpp $(warning_list)

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
	g++ -std=c++17 -c test.cpp

test: test.o
	g++ -o test test.o ecc.o base64.o

cleanTest:
	rm test.o test

clean:
	rm *.o app