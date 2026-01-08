#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>

// compile with g++ main.cpp -o crypto_tool -lcrypto



using EVP_PKEY_ptr = std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)>;
using EVP_PKEY_CTX_ptr = std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)>;
using BIO_ptr = std::unique_ptr<BIO, decltype(&BIO_free_all)>;

class CryptoManager {
public:
    static EVP_PKEY_ptr loadPublicKey(const std::string& pem) {
        BIO_ptr bio(BIO_new_mem_buf(pem.data(), static_cast<int>(pem.size())), BIO_free_all);
        return EVP_PKEY_ptr(PEM_read_bio_PUBKEY(bio.get(), nullptr, nullptr, nullptr), EVP_PKEY_free);
    }

    static EVP_PKEY_ptr loadPrivateKey(const std::string& pem) {
        BIO_ptr bio(BIO_new_mem_buf(pem.data(), static_cast<int>(pem.size())), BIO_free_all);
        return EVP_PKEY_ptr(PEM_read_bio_PrivateKey(bio.get(), nullptr, nullptr, nullptr), EVP_PKEY_free);
    }

    static std::vector<unsigned char> encrypt(EVP_PKEY* pubKey, const std::string& plainText) {
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

    static std::string decrypt(EVP_PKEY* privKey, const std::vector<unsigned char>& cipherText) {
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

    std::vector<unsigned char> readBinaryFile(const std::string& filename) {

        std::ifstream file(filename, std::ios::binary);
        if(!file) return {};
        file.unsetf(std::ios::skipws);

        file.seekg(0, std::ios::end);
        std::streampos fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<unsigned char> output;
        output.reserve(fileSize);
        output.insert(output.begin(),
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>());

        return output;
    }
};

int main() {
    char inp;
    std::cout << "[e]ncrypt or [d]ecrypt? ";
    std::cin >> inp;
    inp = tolower(inp);

    CryptoManager m;
    
    std::string plc;
    std::ifstream priv("priv.pem");
    std::string privKey = "";

    while(getline(priv, plc)) {
        privKey += plc+ '\n';
    }
    priv.close();

    EVP_PKEY_ptr privptr = m.loadPrivateKey(privKey);

    std::ifstream pub("public.pem");
    std::string pubKey = "";

    while(getline(pub, plc)) {
        pubKey += plc + '\n';
    }
    pub.close();

    EVP_PKEY_ptr pubptr = m.loadPublicKey(pubKey);

    std::ofstream outp("result");
    std::vector<unsigned char> directOut;
    std::string res;
    if(inp == 'e') {
        std::string p;
        std::cin >> p;
        directOut = m.encrypt(pubptr.get(), p);
        res = std::string(directOut.begin(), directOut.end());
        std::cout << res << " , " << directOut.size() << std::endl;
        outp << res << std::endl;

        outp.close();
    } else {
        std::cout << "give input in file named 'input'. press enter when ready. ";
        std::cin >> res;

        directOut = m.readBinaryFile("input");

        res = m.decrypt(privptr.get(), directOut);

        std::cout << res << std::endl;

    }

    return 0;

}