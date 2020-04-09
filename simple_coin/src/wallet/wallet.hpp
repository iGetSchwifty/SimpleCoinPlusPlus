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
#include <util/data/base>

class Wallet {
    bool shouldListen;
    static char hexmap[];

    int walletActionListener();
    void walletWrapper();

    [[nodiscard]] int valueFromHexChar (const char *c) noexcept { 
        int returnVal = -1;
        for(int i = 0; i < 16; ++i) {
            if(hexmap[i] == *c) {
                returnVal = i;
                break;
            }
        }
        return returnVal;
    };

    public:
        Wallet(): shouldListen(true) {};

        ~Wallet(){
            shouldListen = false;
        };

        void attachThread() {
            walletWrapper();
        };

        //bool send_transaction(BaseDataSetup::TxtionDetails details);
        void generate_ECDSA_keys();
        //BaseDataSetup::SignedTxtion sign_ECDSA_msg(std::string_view privateKey);

        std::string hexStr(unsigned char *data, size_t len) {
            std::string s(len * 2, ' ');
            for (size_t i = 0; i < len; ++i) {
                s[2 * i]     = hexmap[(data[i] & 0xF0) >> 4];
                s[2 * i + 1] = hexmap[data[i] & 0x0F];
            }
            return s;
        }

        std::shared_ptr<std::vector<uint8_t>> stringToRawData(std::string_view val) {
            auto returnPointer = std::shared_ptr<std::vector<uint8_t>>(new std::vector<uint8_t>());
            for (size_t i = 0, Length = val.size(); i < Length - 1; i = i + 2) {
                char newSub[2];
                newSub[0] = val.substr(i,2)[0];
                newSub[1] = val.substr(i,2)[1];

                int hexVal1 = valueFromHexChar(&newSub[0]);
                int hexVal2 = valueFromHexChar(&newSub[1]);
                if((hexVal1 != -1) && (hexVal2 != -1)) {
                    uint8_t dataChunk = 0x00;
                    dataChunk = ((dataChunk & ~0xFF) | hexVal1) << 4;
                    dataChunk = dataChunk | (hexVal2 & 0x0F);
                    returnPointer->push_back(dataChunk);
                }
            }
            return returnPointer;
        }
        
        int timeStamp() {
            std::chrono::milliseconds time_stamp_now = std::chrono::duration_cast<std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch()); //Just get the time in ms..
            return static_cast<int>(llround(time_stamp_now.count()));
        }
};

#endif
