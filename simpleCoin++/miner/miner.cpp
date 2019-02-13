#include "./miner.hpp"
#include "../libs/base64/base64.h"
#include "../wallet/wallet.hpp"

using json = nlohmann::json;
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;
using namespace std;
using namespace chrono;

Miner::Block Miner::create_genesis_block(){
    milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch()); //Just get the time in ms..
    BlockData dataToInit;
    dataToInit.proof_of_work = 9;
    dataToInit.transactions = vector<Transaction>(); //EMPTY TRANSACTIONS
    return Miner::Block(0, ms.count(), dataToInit, "0");
}

vector<Miner::BlockChain> Miner::find_new_chains(){
    vector<Miner::BlockChain> blockChainsToReturn = vector<Miner::BlockChain>();
    // Synchronous request
    try {
        for(string &node_url : PEER_NODE_URLS){
            HttpClient client(node_url);
            auto r1 = client.request("GET", "/blocks");
            auto jsonBlockChainData = json::parse(r1->content.string()); 
            //
            //  TODO: A FUNCTION HERE THAT WOULD VALIDATE OUR BLOCKCHAIN AGAINST THEIR BLOCKCHAIN...
            //
            blockChainsToReturn.push_back(convertJSON_BlockChain_To_Obj(jsonBlockChainData));
        };
    }
    catch(const SimpleWeb::system_error &e) {
        cerr << "Client request error: " << e.what() << endl;
    }
    return blockChainsToReturn;
}

// should return nullptr if we should keep searching for proof
// else it will contain the longest blockchain we need to Give up searching for proof, update chain and start over
Miner::BlockChain* Miner::consensus(){
    Miner::BlockChain* blockChainToReturn = nullptr;
    vector<Miner::BlockChain> otherBlockchains = find_new_chains();
    for (auto &other_bc : otherBlockchains) {  
        if (other_bc.Chain.size() < bc_obj.Chain.size())
        {
            blockChainToReturn = &other_bc;
        }
    }
    return blockChainToReturn;
}

Miner::ProofOfWork Miner::prove_the_work(long last_proof){
    ProofOfWork powToReturn = ProofOfWork();
    long incrementor = last_proof + 1;
    bool doShouldExit = false;
    milliseconds start_time = duration_cast< milliseconds >(system_clock::now().time_since_epoch()); //Just get the time in ms..
    do {
        milliseconds get_time = duration_cast< milliseconds >(system_clock::now().time_since_epoch()); //Just get the time in ms..
        if( ((int) get_time.count() - (int) start_time.count()) % 60 == 0 ){// Check if any node found the solution
            BlockChain* blockChainConsensus = consensus();
            if(blockChainConsensus != nullptr){
                powToReturn.proof = -444;  // Honestly just returning this because i like the number
                powToReturn.bc_obj = *blockChainConsensus;
                doShouldExit = true;
            }
        } else if ( ((int) get_time.count() - (int) start_time.count()) % 100 == 0 ) {
            incrementor++;
        }
    }while ( !((incrementor % 444 == 0) && (incrementor % last_proof == 0)) && (doShouldExit == false) );

    if(doShouldExit == false){
        //  IF WE GET HERE, WE FOUND IT
        powToReturn.bc_obj = this->bc_obj;
        powToReturn.proof = incrementor;
    }
    return powToReturn;
}

//
//  Analyze this function and determine how it will affect memory over time...
//  I mean ideally we would want to get the blockChain out of memory and into a text file or something
//
int Miner::mine() {
    ProofOfWork pow = prove_the_work(this->bc_obj.Chain.back().data.proof_of_work);

    if(pow.proof == -444){
        //  Update our chain..
        this->bc_obj.Chain = pow.bc_obj.Chain;
    }else{
        // Once we find a valid proof of work, we know we can mine a block so 
        // we reward the miner by adding a transaction
        // First we load all pending transactions sent to the node server
        vector<Transaction> newTransactionList = NODE_PENDING_TRANSACTIONS;
        NODE_PENDING_TRANSACTIONS.clear();

        // Then we add the mining reward
        Transaction miningReward = Transaction();
        miningReward.amount = 1;
        miningReward.FROM = networkName;
        miningReward.TO = MINER_ADDRESS;

        newTransactionList.push_back(miningReward);
        // Now we can gather the data needed to create the new block
        Block* lastBlock = &this->bc_obj.Chain.back();
        milliseconds time_stamp_now = duration_cast< milliseconds >(system_clock::now().time_since_epoch()); //Just get the time in ms..
        BlockData newData = BlockData();
        newData.proof_of_work = pow.proof;
        newData.transactions = newTransactionList;

        // Empty transaction list
        newTransactionList.clear();

        // Now create the new block
        Block newBlock = Block(lastBlock->index + 1, time_stamp_now.count(), newData, lastBlock->hash);
        this->bc_obj.Chain.push_back(newBlock);

        // Let the client know this node mined a block
        //cout << "You mined a block - " << "Index: " << newBlock.index << ", Timestamp: " << newBlock.timestamp
        //     << ", Data: " << newBlock.data_converted_to_string << ", Hash: " << newBlock.previous_hash << endl << endl;
        //cout.flush();
    }
    return 0;
}

bool Miner::validateSignature(json jsonObject) {
    bool returnVal = false;
    //  Verify if the signature is correct. This is used to prove if
    //  it's a valid request trying to do a transaction at the
    //  address. Called when a user tries to submit a new transaction.
    string messageString = jsonObject["message"];
    string data = jsonObject.at("from").get<string>();
    string signatureData = base64_decode(jsonObject.at("signature").get<string>());

    uint8_t p_publicKey[ECC_BYTES + 1];
    Wallet::stringToRawData(data, p_publicKey);

    cout << "PubKey" << endl << p_publicKey << endl << "ENDOFPUB" << endl;
    if(ecdsa_verify(
        p_publicKey,
        reinterpret_cast<const uint8_t*>(&messageString[0]),
        (const uint8_t*)signatureData.c_str()
    )){
        //  valid
        returnVal = true;
        cout << "Valid"; cout.flush();
    }
    else{
        cout << "NOT VALID"; cout.flush();
    }

    return returnVal;
}
