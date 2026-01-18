#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>


class CryptoManager {
    public:

    std::string plaintextPublicKey;

    using EVP_PKEY_ptr = std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)>;
    using EVP_PKEY_CTX_ptr = std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)>;
    using BIO_ptr = std::unique_ptr<BIO, decltype(&BIO_free_all)>;

    static EVP_PKEY_ptr loadPublicKey(const std::string& pem);
    static EVP_PKEY_ptr loadPrivateKey(const std::string& pem);

    EVP_PKEY_ptr loadPrivateKeyFromFile(const std::string& name, CryptoManager& manager);
    EVP_PKEY_ptr loadPublicKeyFromFile(const std::string& name, CryptoManager& manager);

    static std::vector<unsigned char> encrypt(EVP_PKEY* pubKey, const std::string& plainText);
    static std::string decrypt(EVP_PKEY* privKey, const std::vector<unsigned char>& cipherText);

    std::vector<unsigned char> readBinaryFile(const std::string& filename);
    bool doesThisFileExist(const std::string& name);
};