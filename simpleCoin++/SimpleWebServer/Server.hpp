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
        std::thread* thread_ptr;
        Miner* miner;

    public:
        Server(): thread_ptr(nullptr) {
            miner = new Miner();
        };
        ~Server() {
            stopMining();
            delete miner;
            if(thread_ptr != nullptr){
                ShutDown();
                delete thread_ptr;
            }
        };

        void StartNode();
        void listenForTransactions(){thread_ptr->join();};
        void ShutDown() {thread_ptr->detach();};
        void mineCoins();
        void stopMining() {miner->setMiningStatus(false);};
};

#endif