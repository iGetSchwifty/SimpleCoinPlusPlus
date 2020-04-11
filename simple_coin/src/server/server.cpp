#include "server.hpp"

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

void Server::run() {
    int result = 0;
    (*miner)->setMiningStatus(true);
    do {
        result = (*miner)->mine();
    } while((*miner)->shouldMiningOccur() || result != 0);
}

void Server::startNode() {
    server.config.port = 3000;
    
    server.resource["^/$"]["GET"] = [&](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        std::stringstream stream;
        stream << "<h1>{'init_block_hash':" << (*miner)->getGenesisBlock() << " }</h1>";
        response->write(stream);
    };

    //
    //  GET BLOCKS
    //
    server.resource["^/blocks$"]["GET"] = [&](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        std::stringstream stream;
        stream << (*miner)->blockChainToJSON();
        response->write(stream);
    };

    //
    //  GET txion
    //
    server.resource["^/txion$"]["GET"] = [&](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        std::stringstream stream;
        stream << (*miner)->pendingTransactionsToJSON();
        response->write(stream);
    };

    //
    //  POST txtion
    //
    server.resource["^/txion$"]["POST"] = [&](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        try {
            // Retrieve string:
            auto content = request->content.string();
            auto jsonObject = nlohmann::json::parse(content.c_str());
            // {"from": addr_from, "to": addr_to, "amount": amount, "signature": signature, "message": message}
            if((*miner)->validateSignature(jsonObject)){
                Transaction txion {};
                std::string toReturnVal = "New transaction - FROM: ";
                txion.FROM = jsonObject.at("from").get<std::string>();
                txion.TO = jsonObject.at("to").get<std::string>();
                txion.amount = std::stoi(jsonObject.at("amount").get<std::string>());
                (*miner)->appendTransactionToQueue(txion);
                response->write(toReturnVal + txion.FROM + " TO: " + txion.TO + " AMOUNT: " + std::to_string(txion.amount));
            } else {
                content = "ERROR: Check Transaction";
                *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
            }
        }
        catch(const std::exception &e) {
            *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
        }
  };

    server.on_error = [](std::shared_ptr<HttpServer::Request> /*request*/, const SimpleWeb::error_code & /*ec*/) {
    // Handle errors here
    // Note that connection timeouts will also call this handle with ec set to SimpleWeb::errc::operation_canceled
    };

    server.start();
}