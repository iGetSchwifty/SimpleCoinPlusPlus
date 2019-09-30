#include "Server.hpp"

using namespace std;
using json = nlohmann::json;
using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

void Server::mineCoins() {
    int result = 0;
    do {
        result = (*miner)->mine();
    }while((*miner)->shouldMiningOccur() || result != 0);
}

void Server::StartNode() {
    //Miner* server_miner_inst = miner;
    server.config.port = 3000;
    (*miner)->setMiningStatus(true);

    // GET-example for the path /
    // Responds with request-information
    server.resource["^/$"]["GET"] = [&](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
        stringstream stream;
        stream << "<h1>{'init_block_hash':" << (*miner)->getGenesisBlock() << " }</h1>";
        response->write(stream);
    };

    //
    //  GET BLOCKS
    //
    server.resource["^/blocks$"]["GET"] = [&](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
        stringstream stream;
        stream << (*miner)->blockChainToJSON();
        response->write(stream);
    };

    //
    //  GET txion
    //
    server.resource["^/txion$"]["GET"] = [&](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
        stringstream stream;
        stream << (*miner)->pendingTransactionsToJSON();
        response->write(stream);
    };

    //
    //  POST txtion
    //
    server.resource["^/txion$"]["POST"] = [&](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
        try {
            // Retrieve string:
            auto content = request->content.string();
            auto jsonObject = json::parse(content.c_str());
            // {"from": addr_from, "to": addr_to, "amount": amount, "signature": signature, "message": message}
            if((*miner)->validateSignature(jsonObject)){
                Miner::Transaction txion = Miner::Transaction();
                string toReturnVal = "New transaction - FROM: ";
                txion.FROM = jsonObject.at("from").get<string>();
                txion.TO = jsonObject.at("to").get<string>();
                txion.amount = stoi(jsonObject.at("amount").get<string>());
                (*miner)->appendTransactionToQueue(txion);
                response->write(toReturnVal + txion.FROM + " TO: " + txion.TO + " AMOUNT: " + to_string(txion.amount));
            } else {
                content = "ERROR: Check Transaction";
                *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
            }
        }
        catch(const exception &e) {
            *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
        }
  };

    server.on_error = [](shared_ptr<HttpServer::Request> /*request*/, const SimpleWeb::error_code & /*ec*/) {
    // Handle errors here
    // Note that connection timeouts will also call this handle with ec set to SimpleWeb::errc::operation_canceled
    };

    thread_ptr = make_unique<thread*> (new thread([&]() {
        // Start server
        server.start();
    }));
}