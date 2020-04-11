#ifndef BASE_DATA_CLASS
#define BASE_DATA_CLASS

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sha256.h> // TODO: FIX THIS INCLUDE
#include <json.hpp> // TODO: FIX THIS INCLUDE

namespace BaseDataSetup {

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
};
#endif