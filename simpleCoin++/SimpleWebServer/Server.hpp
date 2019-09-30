#ifndef SIMPLE_SERVER_CLASS
#define SIMPLE_SERVER_CLASS

#include "Helpers/server_http.hpp"
#include "../miner/miner.hpp"
#include <thread>
#include <algorithm>
#include <fstream>
#include <vector>
#include <boost/filesystem.hpp>

class Server {
    private:
        SimpleWeb::Server<SimpleWeb::HTTP> server;
        std::unique_ptr<std::thread*> thread_ptr;
        std::unique_ptr<Miner*> miner;

    public:
        Server(): thread_ptr(nullptr) {
            miner = std::make_unique<Miner*>(new Miner());
        };
        ~Server() {
            stopMining();
            shutDown();
        };

        void StartNode();
        void listenForTransactions(){(*thread_ptr)->join();};
        void shutDown() {(*thread_ptr)->detach();};
        void mineCoins();
        void stopMining() {(*miner)->setMiningStatus(false);};
};

#endif