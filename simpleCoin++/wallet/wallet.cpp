#include "wallet.hpp"
#include <stdio.h>
#include <stdlib.h>
#include "../libs/base64/base64.h"
#include "../libs/easy-ecc/ecc.h"

using json = nlohmann::json;
using namespace std;

const char Wallet::hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

bool Wallet::send_transaction(TxtionDetails details) {
    bool returnVal = false;
    SignedTxtion signedTxtion = sign_ECDSA_msg(details.privateKey);
    if(signedTxtion.status == false) {
        return returnVal;
    }
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

SignedTxtion Wallet::sign_ECDSA_msg(string_view privateKey) {
    SignedTxtion signedTxtionToReturn {};
    string str_roundedTimeStamp = to_string(timeStamp());

    uint8_t p_signature[ECC_BYTES*2] {0};
    
    auto p_privateKey = stringToRawData(privateKey);

    if(ecdsa_sign(
        p_privateKey->data(),
        reinterpret_cast<const uint8_t*>(&str_roundedTimeStamp[0]),
        p_signature
    )){
        signedTxtionToReturn.status = true;
        signedTxtionToReturn.signature = base64_encode(p_signature, 64);
        signedTxtionToReturn.message = str_roundedTimeStamp;

        cout << "Signed Transaction Successfully\n" << endl;
    }else {
        signedTxtionToReturn.status = false;
        cout << "Error signing transaction" << endl;
    }
    return signedTxtionToReturn;
};

void Wallet::generate_ECDSA_keys() {
    string privateEncodedKey;
    string publicKey;
    uint8_t p_publicKey[ECC_BYTES+1]{0};
	uint8_t p_privateKey[ECC_BYTES]{0};

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

        cout << "Wallet address / Public key: " << publicKey << endl;
        cout << "*** PRIVATE KEY DONT LOSE ***: " << privateEncodedKey << endl;
    }else {
        cout << "Error creating keys.." << endl;
    }
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
            if(send_transaction(txtionDetails)) {
                cout << "Transaction Successfully Sent!\n";
            } else {
                cout << "Transaction Failed To Send\n";
            }
        }
    }
    return 0;
};