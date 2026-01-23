#include "encryption.h"

// COMPILE WITH g++ encryption.cpp -o crypto_tool -lcrypto

CryptoManager::EVP_PKEY_ptr CryptoManager::loadPublicKey(const std::string& pem) {
    BIO_ptr bio(BIO_new_mem_buf(pem.data(), static_cast<int>(pem.size())), BIO_free_all);
    return EVP_PKEY_ptr(PEM_read_bio_PUBKEY(bio.get(), nullptr, nullptr, nullptr), EVP_PKEY_free);
}

CryptoManager::EVP_PKEY_ptr CryptoManager::loadPrivateKey(const std::string& pem) {
    BIO_ptr bio(BIO_new_mem_buf(pem.data(), static_cast<int>(pem.size())), BIO_free_all);
    return EVP_PKEY_ptr(PEM_read_bio_PrivateKey(bio.get(), nullptr, nullptr, nullptr), EVP_PKEY_free);
}

std::vector<unsigned char> CryptoManager::encrypt(EVP_PKEY* pubKey, const std::string& plainText) {
    EVP_PKEY_CTX_ptr ctx(EVP_PKEY_CTX_new(pubKey, nullptr), EVP_PKEY_CTX_free);

    if(EVP_PKEY_encrypt_init(ctx.get()) <= 0) return {};

    EVP_PKEY_CTX_set_rsa_padding(ctx.get(), RSA_PKCS1_OAEP_PADDING);

    size_t outLen;

    EVP_PKEY_encrypt(ctx.get(), nullptr, &outLen, (const unsigned char*)plainText.data(), plainText.size());

    std::vector<unsigned char> cipherText(outLen);

    if (EVP_PKEY_encrypt(ctx.get(), cipherText.data(), &outLen, (const unsigned char*)plainText.data(), plainText.size()) <= 0) {
        return {};
    }
    return cipherText;

}

std::string CryptoManager::decrypt(EVP_PKEY* privKey, const std::vector<unsigned char>& cipherText) {
    EVP_PKEY_CTX_ptr ctx(EVP_PKEY_CTX_new(privKey, nullptr), EVP_PKEY_CTX_free);

    if(EVP_PKEY_decrypt_init(ctx.get()) <= 0) return "";

    EVP_PKEY_CTX_set_rsa_padding(ctx.get(), RSA_PKCS1_OAEP_PADDING);

    size_t outLen;
    EVP_PKEY_decrypt(ctx.get(), nullptr, &outLen, cipherText.data(), cipherText.size());

    std::string plainText(outLen, '\0');
    if(EVP_PKEY_decrypt(ctx.get(), (unsigned char*)plainText.data(), &outLen, cipherText.data(), cipherText.size()) <= 0) {
        return "Decryption failed";
    }

    plainText.resize(outLen);
    return plainText;
}

std::vector<unsigned char> CryptoManager::readBinaryFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file) return {};

    file.unsetf(std::ios::skipws);
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);


    std::vector<unsigned char> vec;
    vec.reserve(fileSize);
    vec.insert(vec.begin(), 
            std::istreambuf_iterator<char>(file), 
            std::istreambuf_iterator<char>());

    return vec;
}

bool CryptoManager::doesThisFileExist(const std::string& name) {
    if(FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

CryptoManager::EVP_PKEY_ptr CryptoManager::loadPrivateKeyFromFile(const std::string& name, CryptoManager& manager) {
    // opening private key
    if(!doesThisFileExist(name)) {
        std::cerr << "Place the PRIVATE key in a file called private.pem in the same folder as this executable. Do not give it to anyone or distribute it. If anyone else may know it, generate a new public/private key pair.\n";
        std::exit(-1);
    }

    std::ifstream priv(name);
    std::string privKey = "";
    
    std::string plc;
    while(getline(priv, plc)) {
        privKey += plc+ '\n';
    }
    priv.close();
    return manager.loadPrivateKey(privKey);
}

CryptoManager::EVP_PKEY_ptr CryptoManager::loadPublicKeyFromFile(const std::string& name, CryptoManager& manager) {

    std::string plc;
    if(!doesThisFileExist(name)) {
        std::cerr << "Place the PUBLIC key in a file called public.pem in the same folder as this executable. \n";
        std::exit(-1);
    }

    std::ifstream pub(name);
    std::string pubKey = "";

    while(getline(pub, plc)) {
        pubKey += plc + '\n';
    }
    pub.close();

    plaintextPublicKey = pubKey;

    return manager.loadPublicKey(pubKey);
}
/*
int main() {
    
    CryptoManager m;
    CryptoManager::EVP_PKEY_ptr privptr = loadPrivateKeyFromFile("priv.pem", m);
    CryptoManager::EVP_PKEY_ptr pubptr  = loadPublicKeyFromFile ("public.pem", m);

    // take user input
    char inp;
    std::cout << "[e]ncrypt or [d]ecrypt? ";
    std::cin >> inp;
    inp = tolower(inp);

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::vector<unsigned char> directOut;
    std::string res;
    if(inp == 'e') {
        std::ofstream outp("result.bin", std::ios::binary);

        std::string p;
        std::cout << "Enter message to encrypt: ";
        std::getline(std::cin, p);

        directOut = m.encrypt(pubptr.get(), p);
        p = "";

        // res = std::string(directOut.begin(), directOut.end());
        // std::cout << res << " , " << directOut.size() << std::endl;
        // outp << res << std::endl;

        outp.write(reinterpret_cast<const char*> (directOut.data()), directOut.size());
        outp.close();

        std::cout << "Wrote " << directOut.size() << " bytes to result.bin \n";

    } else {
        std::cout << "Reading from `result.bin` ...\n";
        // std::cin >> res;

        directOut = m.readBinaryFile("result.bin");
        if(directOut.empty()) {
            std::cerr << "File is empty or missing. \n";
            return 1;
        }
        res = m.decrypt(privptr.get(), directOut);
        std::cout << "\nDecrypted message : " << res << "\n\n";

    }

    return 0;

}
*/