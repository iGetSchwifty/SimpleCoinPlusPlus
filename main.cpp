#include "simpleCoin++/SimpleWebServer/Server.hpp"
#include "simpleCoin++/wallet/wallet.hpp"

//
//  Update project to use CMake
//

int main()//(int argc, char *argv[])
{
  std::unique_ptr<Server> app(new Server());
  app->StartNode();

  std::unique_ptr<Wallet> wallet(new Wallet());
  
  app->mineCoins();

  wallet->attachThread();
  app->listenForTransactions();

  return 0;
}