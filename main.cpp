//#include "simpleCoin++/SimpleWebServer/Server.hpp"
#include "simpleCoin++/wallet/wallet.hpp"

//
//  Update project to use CMake
//

int main() {
  Wallet wallet {};
  wallet.attachThread();
  return 0;
}