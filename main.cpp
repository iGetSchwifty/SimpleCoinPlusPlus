#include "simpleCoin++/SimpleWebServer/Server.hpp"
#include "simpleCoin++/wallet/wallet.hpp"

int main(int argc, char *argv[])
{
  std::unique_ptr<Server> app(new Server());
  app->StartNode();

  std::unique_ptr<Wallet> wallet(new Wallet());
  wallet->listen_to_actions();
  
  app->mineCoins();

  wallet->attachThread();
  app->listenForTransactions();

  return 0;
}