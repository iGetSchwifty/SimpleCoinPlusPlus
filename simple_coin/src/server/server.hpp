#ifndef SIMPLE_SERVER_CLASS
#define SIMPLE_SERVER_CLASS

#include <algorithm>
#include <fstream>
#include <vector>
#include <server_http.hpp>
#include <miner.hpp>

class Server {
    SimpleWeb::Server<SimpleWeb::HTTP> server;
    std::shared_ptr<Miner*> miner;

    public:
        Server(): miner(std::make_shared<Miner*>(new Miner())) {};
        ~Server() {
            stopMining();
        };

        void StartNode();
        void run();
        void stopMining() {(*miner)->setMiningStatus(false);};
};

#endif