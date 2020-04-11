#ifndef SIMPLE_SERVER_CLASS
#define SIMPLE_SERVER_CLASS

#include <algorithm>
#include <fstream>
#include <thread>
#include <vector>
#include <server_http.hpp>
#include <miner.hpp>

class Server {
    SimpleWeb::Server<SimpleWeb::HTTP> server;
    std::shared_ptr<Miner*> miner;
    std::thread server_thread;

    void startNode();
    void stopMining() { (*miner)->setMiningStatus(false); };

    public:
        Server(): miner(std::make_shared<Miner*>(new Miner())),
                  server_thread([&](){ startNode(); }) {};

        ~Server() { stopMining(); };

        void run();
};

#endif