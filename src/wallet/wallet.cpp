#include "wallet.hpp"

bool Wallet::send_transaction(BaseDataSetup::TxtionDetails details) {
    using namespace std;
    using namespace nlohmann;
    bool returnVal = false;
    BaseDataSetup::SignedTxtion signedTxtion = sign_ECDSA_msg(details.privateKey);
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

BaseDataSetup::SignedTxtion Wallet::sign_ECDSA_msg(std::string_view privateKey) {
    using namespace std;
    BaseDataSetup::SignedTxtion signedTxtionToReturn {};
    string str_roundedTimeStamp = to_string(BaseDataSetup::timeStamp());

    uint8_t p_signature[ECC_BYTES*2] {0};
    
    auto p_privateKey = BaseDataSetup::stringToRawData(privateKey);

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
    using namespace std;
    string privateEncodedKey;
    string publicKey;
    uint8_t p_publicKey[ECC_BYTES+1]{0};
	uint8_t p_privateKey[ECC_BYTES]{0};

    if(ecc_make_key(
        p_publicKey,
        p_privateKey
    )){
        for (auto &s : p_publicKey) {
            string hexOutput = BaseDataSetup::hexStr(&s, sizeof(s));
            publicKey.append(hexOutput);
        }

        for (auto &s : p_privateKey) {
            string hex2 = BaseDataSetup::hexStr(&s, sizeof(s));
            privateEncodedKey.append(hex2);
        }

        cout << "Wallet address / Public key: " << publicKey << endl;
        cout << "*** PRIVATE KEY DONT LOSE ***: " << privateEncodedKey << endl;
    }else {
        cout << "Error creating keys.." << endl;
    }
};

void Wallet::run() {
    int res = 0;
    do {
        res = walletActionListener();
    } while (res == 0 && shouldListen);
};

int Wallet::walletActionListener() {
    using namespace std;
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
            BaseDataSetup::TxtionDetails txtionDetails {};
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