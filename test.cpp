#include <iostream>
#include <chrono>
#include <math.h>
#include "./simpleCoin++/libs/easy-ecc/ecc.h"
#include "./simpleCoin++/libs/base64/base64.h"
#include "./simpleCoin++/wallet/wallet.hpp"

using namespace std;

int main() {
    string privateEncodedKey;
    string publicKey;
    uint8_t p_publicKey[ECC_BYTES+1];
	uint8_t p_privateKey[ECC_BYTES];

    uint8_t p_signature[ECC_BYTES*2];
    string str_roundedTimeStamp = to_string(Wallet::timeStamp());

    if(ecc_make_key(
        p_publicKey,
        p_privateKey
    )){
        for (auto &s : p_publicKey) {
            string hexOutput = Wallet::hexStr(&s, sizeof(s));
            publicKey.append(hexOutput);
        }

        for (auto &s : p_privateKey) {
            string hex2 = Wallet::hexStr(&s, sizeof(s));
            privateEncodedKey.append(hex2);
        }

        cout << "PrivateKey:" << endl << p_privateKey << endl << "ENDOFPrivateKey" << endl;
        cout << "PubKey:" << endl << p_publicKey << endl << "ENDOFPUB" << endl;

        cout << "Wallet address / Public key: " << publicKey << endl;
        cout << "*** PRIVATE KEY DONT LOSE ***: " << privateEncodedKey << endl;

        string private_key;
        string addr_from;
        cout << "From: introduce your wallet address (public key)" << endl;
        cin >> addr_from;
        cout << "Introduce your private key" << endl;
        cin >> private_key;

        auto decoded_privateKey = Wallet::stringToRawData(private_key);
        auto decoded_publicKey = Wallet::stringToRawData(addr_from);

        if(ecdsa_sign(
            decoded_privateKey->data(),
            reinterpret_cast<const uint8_t*>(&str_roundedTimeStamp[0]),
            p_signature
        )){
            string signatureData = base64_decode(base64_encode(p_signature, 64));
            if(ecdsa_verify(
                decoded_publicKey->data(),
                reinterpret_cast<const uint8_t*>(&str_roundedTimeStamp[0]),
                reinterpret_cast<const uint8_t*>(signatureData.c_str())
            )){
                //  valid
                cout << "Valid\n";
            }
            else{
                cout << "NOT VALID";
            }

        }else {
            cout << "Error signing transaction" << endl;
        }

    }else {
        cout << "Error creating keys.." << endl;
    }
    return 0;
}