#include "wallet.hpp"
#include <stdio.h>
#include <stdlib.h>
#include "../libs/base64/base64.h"

using json = nlohmann::json;
using namespace std;
using namespace chrono;

const char Wallet::hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

bool Wallet::send_transaction(Wallet::TxtionDetails details) {
    bool returnVal = false;

    SignedTxtion signedTxtion = sign_ECDSA_msg(details.privateKey);
    // Synchronous request examples since im throwing the wallet in its own thread ideally.
    json dataToSend;
    dataToSend["from"] = details.addrFrom;
    dataToSend["to"] = details.addrTo;
    dataToSend["amount"] = details.amount;
    dataToSend["signature"] = signedTxtion.signature;
    dataToSend["message"] = signedTxtion.message;

    try {
        auto req = client.request("POST", "/txion", dataToSend.dump());
        cout << endl << req->content.rdbuf() << endl;
        returnVal = true;
    }
    catch(const SimpleWeb::system_error &e) {
        cout << endl << "Client request error: " << e.what() << endl;
        cout.flush();
    }

    return returnVal;
};

Wallet::SignedTxtion Wallet::sign_ECDSA_msg(string privateKey) {
    SignedTxtion signedTxtionToReturn = SignedTxtion();
    milliseconds time_stamp_now = duration_cast< milliseconds >(system_clock::now().time_since_epoch()); //Just get the time in ms..
    int roundedTimeStamp = (int) llround(time_stamp_now.count());
    string str_roundedTimeStamp = to_string(roundedTimeStamp);

    uint8_t p_signature[ECC_BYTES*2];
    uint8_t p_privateKey[ECC_BYTES];

    stringToRawData(privateKey, p_privateKey);

    cout << "PrivateKeySign:" << endl << p_privateKey << endl << "ENDOFPrivateKeySign" << endl;

    if(ecdsa_sign(
        p_privateKey,
        reinterpret_cast<const uint8_t*>(&str_roundedTimeStamp[0]),
        p_signature
    )){
        signedTxtionToReturn.status = 1;
        signedTxtionToReturn.signature = base64_encode(p_signature, 64);
        signedTxtionToReturn.message = str_roundedTimeStamp;
    }else {
        signedTxtionToReturn.status = 0;
        cout << "Error signing transaction" << endl;
    }
    return signedTxtionToReturn;
};

void Wallet::generate_ECDSA_keys() {
    int privateKey[ECC_BYTES];
    string privateEncodedKey;
    string publicKey;
    uint8_t p_publicKey[ECC_BYTES+1];
	uint8_t p_privateKey[ECC_BYTES];

    if(ecc_make_key(
        p_publicKey,
        p_privateKey
    )){
        for (auto &s : p_publicKey) {
            string hexOutput = hexStr(&s, sizeof(s));
            publicKey.append(hexOutput);
        }

        for (auto &s : p_privateKey) {
            string hex2 = hexStr(&s, sizeof(s));
            privateEncodedKey.append(hex2);
        }

        cout << "PrivateKey:" << endl << p_privateKey << endl << "ENDOFPrivateKey" << endl;
        cout << "PubKey:" << endl << p_publicKey << endl << "ENDOFPUB" << endl;

        cout << "Wallet address / Public key: " << publicKey << endl;
        cout << "*** PRIVATE KEY DONT LOSE ***: " << privateEncodedKey << endl;
    }else {
        cout << "Error creating keys.." << endl;
    }
};

void Wallet::listen_to_actions() {
    thread_ptr = new thread([&]() {
        this->walletWrapper();
    });
};

void Wallet::walletWrapper() {
    int res = 0;
    do {
        res = walletActionListener();
    } while (res == 0 && shouldListen);
};

int Wallet::walletActionListener() {
    string Input;
    cin >> Input;

    if(Input == "1") {
        generate_ECDSA_keys();
    }else if (Input == "2") {
        string addr_from;
        string private_key;
        string addr_to;
        string amount;
        cout << "From: introduce your wallet address (public key)" << endl;
        cin >> addr_from;
        cout << "Introduce your private key" << endl;
        cin >> private_key;
        cout << "To: introduce destination wallet address" << endl;
        cin >> addr_to;
        cout << "Amount: number stating how much do you want to send" << endl;
        cin >> amount;

        cout << "Is everything correct?" << endl;
        cout << "From: " << addr_from << endl
             << "Private Key: " << private_key << endl
             << "To: " << addr_to << endl
             << "Amount: " << amount << endl;

        cout << endl << "y/n" << endl;
        cin >> Input;

        if(Input == "y" || Input == "Y") {
            TxtionDetails txtionDetails = TxtionDetails();
            txtionDetails.addrFrom = addr_from;
            txtionDetails.addrTo = addr_to;
            txtionDetails.amount = amount;
            txtionDetails.privateKey = private_key;
            send_transaction(txtionDetails);
        }
    }
    return 0;
};