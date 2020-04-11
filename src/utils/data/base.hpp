#ifndef BASE_DATA_CLASS
#define BASE_DATA_CLASS

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sha256.h> // TODO: FIX THIS INCLUDE
#include <json.hpp> // TODO: FIX THIS INCLUDE

namespace BaseDataSetup {
    static char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    struct Transaction {
        std::string FROM;
        std::string TO;
        int amount;
    };

    struct BlockData {
        int proof_of_work;
        std::vector<BaseDataSetup::Transaction> transactions;
    };

    struct Block {
        private: 
            std::string convertDataToJSONString(BaseDataSetup::BlockData data){
                nlohmann::json j;
                j["proof_of_work"] = data.proof_of_work;
                j["transactions"] = nlohmann::json::array();
                for(size_t i = 0, Length = data.transactions.size(); i < Length; i++) {
                    j["transactions"].push_back(nlohmann::json::object({
                        { "from", data.transactions[i].FROM },
                        { "to", data.transactions[i].TO },
                        { "amount", data.transactions[i].amount } 
                    }));
                }
                data_converted_to_string = j.dump();
                return data_converted_to_string;
            }
                
            std::string hash_block(){
                //
                //  TODO: NEED TO UTF ENCODE THE PREVIOUS_HASH
                //
                std::string stringToSha = std::to_string(index) + std::to_string(timestamp)+ convertDataToJSONString(data) + previous_hash;
                return sha256(stringToSha);// yolo, leaving it as a string for now
            }

        public:
            int index;
            long long timestamp;
            BaseDataSetup::BlockData data;
            std::string previous_hash;
            std::string hash;
            std::string data_converted_to_string;

            Block(int index, long long timestamp, BaseDataSetup::BlockData data, std::string previous_hash){
                this->index = index;
                this->timestamp = timestamp;
                this->data = data;
                this->previous_hash = previous_hash;
                this->hash = hash_block();
            }
    };

    struct BlockChain {
        std::vector<Block> Chain;
        BlockChain(Block init_block){
            Chain.push_back(init_block);
        }
        BlockChain(){}
        ~BlockChain(){
            Chain.clear();
        }
    };

    struct ProofOfWork {
        int proof;
        BlockChain bc_obj;
    };

    struct SignedTxtion {
        std::string signature;
        std::string message;
        bool status;
    };
            
    struct TxtionDetails {
        std::string addrFrom;
        std::string addrTo;
        std::string_view privateKey;
        std::string amount; // Should really be a std::uint64_t and dealing with a base denomination like Bitcoin deals with Sats...
    };

    [[nodiscard]] static int valueFromHexChar (const char *c) noexcept { 
        int returnVal = -1;
        for(int i = 0; i < 16; ++i) {
            if(BaseDataSetup::hexmap[i] == *c) {
                returnVal = i;
                break;
            }
        }
        return returnVal;
    };

    [[nodiscard]] static std::shared_ptr<std::vector<uint8_t>> stringToRawData(std::string_view val) {
        auto returnPointer = std::shared_ptr<std::vector<uint8_t>>(new std::vector<uint8_t>());
            for (size_t i = 0, Length = val.size(); i < Length - 1; i = i + 2) {
                char newSub[2];
                newSub[0] = val.substr(i,2)[0];
                newSub[1] = val.substr(i,2)[1];

                int hexVal1 = BaseDataSetup::valueFromHexChar(&newSub[0]);
                int hexVal2 = BaseDataSetup::valueFromHexChar(&newSub[1]);
                if((hexVal1 != -1) && (hexVal2 != -1)) {
                    uint8_t dataChunk = 0x00;
                    dataChunk = ((dataChunk & ~0xFF) | hexVal1) << 4;
                    dataChunk = dataChunk | (hexVal2 & 0x0F);
                    returnPointer->push_back(dataChunk);
                }
            }
        return returnPointer;
    }

    [[nodiscard]] static std::string hexStr(unsigned char *data, size_t len) {
        std::string s(len * 2, ' ');
        for (size_t i = 0; i < len; ++i) {
            s[2 * i]     = BaseDataSetup::hexmap[(data[i] & 0xF0) >> 4];
            s[2 * i + 1] = BaseDataSetup::hexmap[data[i] & 0x0F];
        }
        return s;
    }
        
    [[nodiscard]] static int timeStamp() {
        std::chrono::milliseconds time_stamp_now = std::chrono::duration_cast<std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch()); //Just get the time in ms..
        return static_cast<int>(llround(time_stamp_now.count()));
    }
};
#endif