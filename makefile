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
	g++ -c ./simpleCoin++/libs/sha256/sha256.cpp

server.o: ./simpleCoin++/SimpleWebServer/Server.cpp ./simpleCoin++/SimpleWebServer/Server.hpp
	g++ -std=c++17 -c ./simpleCoin++/SimpleWebServer/Server.cpp 

base64.o: ./simpleCoin++/libs/base64/base64.cpp ./simpleCoin++/libs/base64/base64.h
	g++ -c ./simpleCoin++/libs/base64/base64.cpp

ecc.o: ./simpleCoin++/libs/easy-ecc/ecc.c ./simpleCoin++/libs/easy-ecc/ecc.h
	gcc -c ./simpleCoin++/libs/easy-ecc/ecc.c

clean:
	rm *.o app