#include <iostream>
#include <chrono>
#include <math.h>
#include "./simpleCoin++/libs/easy-ecc/ecc.h"
#include "./simpleCoin++/libs/base64/base64.h"

using namespace std;
using namespace chrono;

const char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

string hexStr(unsigned char *data, int len) {
    string s(len * 2, ' ');
    for (int i = 0; i < len; ++i) {
        s[2 * i]     = hexmap[(data[i] & 0xF0) >> 4];
        s[2 * i + 1] = hexmap[data[i] & 0x0F];
    }
    return s;
}

const int valueFromHexChar (const unsigned char *c) { 
    int returnVal = -1;
    for(int i = 0; i < 16; ++i) {
        if(hexmap[i] == *c) {
            returnVal = i;
            break;
        }
    }
    return returnVal;
}

const void stringToRawData(std::string val, uint8_t arrayToPopulate[]) {
    for (int i = 0, Length = val.size(); i < Length - 1; i = i + 2) {
        unsigned char newSub[2];
        newSub[0] = val.substr(i,2)[0];
        newSub[1] = val.substr(i,2)[1];

        int hexVal1 = valueFromHexChar(&newSub[0]);
        int hexVal2 = valueFromHexChar(&newSub[1]);
        if((hexVal1 != -1) && (hexVal2 != -1)) {
            uint8_t dataChunk = ((dataChunk & ~0xFF) | hexVal1) << 4;
            dataChunk = dataChunk | (hexVal2 & 0x0F);
            arrayToPopulate[i/2] = dataChunk;
        }
    }
}

int main(int argc, char *argv[]) {
    int privateKey[ECC_BYTES];
    string privateEncodedKey;
    string publicKey;
    uint8_t p_publicKey[ECC_BYTES+1];
	uint8_t p_privateKey[ECC_BYTES];

    uint8_t decoded_publicKey[ECC_BYTES + 1];
    uint8_t decoded_privateKey[ECC_BYTES];

    uint8_t p_signature[ECC_BYTES*2];
    milliseconds time_stamp_now = duration_cast< milliseconds >(system_clock::now().time_since_epoch()); //Just get the time in ms..
    int roundedTimeStamp = (int) llround(time_stamp_now.count());
    string str_roundedTimeStamp = to_string(roundedTimeStamp);

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

        string private_key;
        string addr_from;
        cout << "From: introduce your wallet address (public key)" << endl;
        cin >> addr_from;
        cout << "Introduce your private key" << endl;
        cin >> private_key;

        stringToRawData(private_key, decoded_privateKey);
        stringToRawData(addr_from, decoded_publicKey);

        if(ecdsa_sign(
            decoded_privateKey,
            reinterpret_cast<const uint8_t*>(&str_roundedTimeStamp[0]),
            p_signature
        )){
            string signatureData = base64_decode(base64_encode(p_signature, 64));
            if(ecdsa_verify(
                decoded_publicKey,
                reinterpret_cast<const uint8_t*>(&str_roundedTimeStamp[0]),
                (const uint8_t*)signatureData.c_str()
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