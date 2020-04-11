#ifndef MINER_CLASS
#define MINER_CLASS

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>

#include <sha256.h>
#include <json.hpp>
#include <ecc.h>
#include <client_http.hpp>
#include <base.hpp>
#include <base64.h>

using namespace BaseDataSetup;

class Miner {
    bool should_mine_transactions;
    std::vector<std::string> PEER_NODE_URLS;
    std::vector<Transaction> NODE_PENDING_TRANSACTIONS;
    std::string networkName;
    std::string MINER_ADDRESS;
    BlockChain bc_obj;

    public:
        //        Init the variable for mining..   Also add the first block and create a blockchain!
        Miner() : should_mine_transactions(false), bc_obj(BlockChain(create_genesis_block())), networkName("network"),
        MINER_ADDRESS("03f644177f760b34088ff9330832444d26aba8999599d8905f45543544489289cd") {}
        //PrivateKey: 2501f54eb5111affe05833126f447be9742f2bea3570586a74b9aaa351bdcd54

        ~Miner(){
            bc_obj.Chain.clear();
            NODE_PENDING_TRANSACTIONS.clear();
            PEER_NODE_URLS.clear();
        }

        Block create_genesis_block();
        std::vector<BlockChain> find_new_chains();
        BlockChain* consensus();
        ProofOfWork prove_the_work(long last_proof);
        int mine();
        [[nodiscard]] static bool validateSignature(nlohmann::json jsonObject) noexcept;
        void appendTransactionToQueue(Transaction transactionToBeProcessed) {
            NODE_PENDING_TRANSACTIONS.push_back(transactionToBeProcessed);
        }
        void setMiningStatus(bool statusToSet) {this->should_mine_transactions = statusToSet;}
        bool shouldMiningOccur() {return this->should_mine_transactions;}

        std::string getGenesisBlock(){
            if(bc_obj.Chain.size() > 0){
                return bc_obj.Chain[0].hash;
            }else {
                return NULL;
            }
        }

        //
        //  HELPER FUNCTIONS BELOW HERE...
        //
        std::string pendingTransactionsToJSON() {
            nlohmann::json j;
            j["transactions"] = nlohmann::json::array();
            for(size_t i = 0, Length = NODE_PENDING_TRANSACTIONS.size(); i < Length; i++) {
                j["transactions"].push_back(nlohmann::json::object({
                    { "from", NODE_PENDING_TRANSACTIONS[i].FROM },
                    { "to", NODE_PENDING_TRANSACTIONS[i].TO },
                    { "amount", NODE_PENDING_TRANSACTIONS[i].amount } 
                }));
            };
            return j.dump();
        }

        std::string blockChainToJSON() {
            nlohmann::json j;
            j["blocks"] = nlohmann::json::array();
            for(size_t i = 0, Length = bc_obj.Chain.size(); i < Length; i++) {
                Block currentParsingBlock = bc_obj.Chain[i];
                j["blocks"].push_back(nlohmann::json::object({
                    { "data", currentParsingBlock.data_converted_to_string },
                    { "index", currentParsingBlock.index },
                    { "timestamp", currentParsingBlock.timestamp },
                    { "hash", currentParsingBlock.hash}
                }));
            };
            return j.dump();
        }

        BlockChain convertJSON_BlockChain_To_Obj(nlohmann::json jsonObj) {
            BlockChain bc_local_ref = BlockChain();
            try {
                if(jsonObj.at("blocks").size() > 0){
                    bc_local_ref = BlockChain(blockJSONToObj(jsonObj.at("blocks").front()));
                    for(size_t i = 0, Length = jsonObj.at("blocks").size(); i < Length; i++) {
                      bc_local_ref.Chain.push_back(blockJSONToObj(jsonObj.at("blocks")[i]));
                    }
                }
                return bc_local_ref;
            }
            catch(const std::exception &e) {
                return BlockChain();
            }
        }

        Block blockJSONToObj(nlohmann::json jsonObj) {
            int lclIndex = jsonObj.at("index").get<int>();
            long long timestamp = jsonObj.at("timestamp").get<long long>();
            std::string previous_hash = jsonObj.at("hash").get<std::string>();
            BlockData data = BlockData();
            data.proof_of_work = jsonObj.at("data").at("proof_of_work").get<int>();
            for(size_t i = 0, Length = jsonObj.at("transactions").size(); i < Length; i++) {
                data.transactions.push_back(transactionJSONToObj(jsonObj.at("transactions")[i]));
            }
            return Block(lclIndex, timestamp, data, previous_hash);
        }

        Transaction transactionJSONToObj(nlohmann::json jsonObj) {
            Transaction txtionToReturn = Transaction();
            txtionToReturn.FROM = jsonObj.at("from").get<std::string>();
            txtionToReturn.TO = jsonObj.at("to").get<std::string>();
            txtionToReturn.amount = jsonObj.at("amount").get<int>();
            return txtionToReturn;
        }
}; 
#endif

