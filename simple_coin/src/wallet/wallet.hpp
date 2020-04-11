#ifndef WALLET_CLASS
#define WALLET_CLASS

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <math.h>
#include <algorithm>
#include <stdexcept>
#include <base.hpp>
#include <ecc.h>
#include <base64.h>
#include <client_http.hpp>

class Wallet {
    bool shouldListen;
    SimpleWeb::Client<SimpleWeb::HTTP> client;
    int walletActionListener();

    public:
        Wallet(): client("localhost:3000"), shouldListen(true) {};

        ~Wallet(){
            shouldListen = false;
        };

        void run();

        bool send_transaction(BaseDataSetup::TxtionDetails details);
        void generate_ECDSA_keys();
        BaseDataSetup::SignedTxtion sign_ECDSA_msg(std::string_view privateKey);
};

#endif
