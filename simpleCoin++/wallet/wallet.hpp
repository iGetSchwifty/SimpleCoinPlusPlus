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

#include "../libs/sha256/sha256.h"
#include "../libs/json/json.hpp"
#include "../libs/easy-ecc/ecc.h"
#include "../SimpleWebServer/Helpers/client_http.hpp"

class Wallet {
    private:
        struct SignedTxtion {
            std::string signature;
            std::string message;
            int status;
        };
        struct TxtionDetails {
            std::string addrFrom;
            std::string addrTo;
            std::string privateKey;
            std::string amount;
        };
        SimpleWeb::Client<SimpleWeb::HTTP> client;

        std::thread* thread_ptr;
        bool shouldListen;
        const static char hexmap[];

        int walletActionListener();
        void walletWrapper();

        static const int valueFromHexChar (const unsigned char *c) { 
            int returnVal = -1;
            for(int i = 0; i < 16; ++i) {
                if(hexmap[i] == *c) {
                    returnVal = i;
                    break;
                }
            }
            return returnVal;
        }

    public:
        Wallet(): client("localhost:3000"), shouldListen(true) {};
        ~Wallet(){
            shouldListen = false;
            thread_ptr->detach();
        };

        void attachThread() {thread_ptr->join();};
        bool send_transaction(TxtionDetails details);
        void generate_ECDSA_keys();
        SignedTxtion sign_ECDSA_msg(std::string privateKey);
        void listen_to_actions();

        std::string hexStr(unsigned char *data, int len) {
            std::string s(len * 2, ' ');
            for (int i = 0; i < len; ++i) {
                s[2 * i]     = hexmap[(data[i] & 0xF0) >> 4];
                s[2 * i + 1] = hexmap[data[i] & 0x0F];
            }
            return s;
        }

        static const void stringToRawData(std::string val, uint8_t arrayToPopulate[]) {
            for (int i = 0, Length = val.size(); i < Length - 1; i = i + 2) {
                unsigned char newSub[2];
                newSub[0] = val.substr(i,2)[0];
                newSub[1] = val.substr(i,2)[1];

                int hexVal1 = valueFromHexChar(&newSub[0]);
                int hexVal2 = valueFromHexChar(&newSub[1]);
                if((hexVal1 != -1) && (hexVal2 != -1)) {
                    uint8_t dataChunk = ((dataChunk & ~0xFF) | hexVal1) << 4;
                    dataChunk = dataChunk | (hexVal2 & 0x0F);
                    arrayToPopulate[i/2] = dataChunk;
                }
            }
        }
};

#endif
