#include <smart_data.h>
#include <alarm.h>

#include "hash.h"
#include "crypto_hash.h"

using namespace EPOS;
using namespace crypto;

OStream cout;

void print_as_hex(const char* text, const unsigned char* what, const int len=SHA256_BLOCK_SIZE) {
    const char HEX_CHARS[] = {"0123456789abcdef"};
    cout << text << ": ";
    for (int i = 0; i < len; ++i) {
        char c = what[i];
        cout << HEX_CHARS[(c >> 4) & 0b1111] << HEX_CHARS[c & 0b1111];
    }
    cout << '\n';
}

void encrypt_hash(const BYTE *hash, const BYTE *key, BYTE *output)
{
    AES_Common::AES<16> enigma = AES_Common::AES<16>(AES_Common::ECB);
    BYTE input_aes1[16];
    BYTE input_aes2[16];
    for (int i = 0; i < 16; i++) {
        input_aes1[i] = hash[i];
        input_aes2[i] = hash[i+16];
    }

    BYTE output_aes1[16];
    BYTE output_aes2[16];
    enigma.encrypt(input_aes1, key, output_aes1);
    enigma.encrypt(input_aes2, key, output_aes2);

    for (int i = 0; i < 16; i++) {
        output[i] = output_aes1[i];
        output[i+16] = output_aes2[i];
    }

}

int main()
{
    Alarm::delay(5000000);
    cout << "Hello!" << endl;

    const unsigned char key[16] = {0xaa, 0xaa, 0xaa, 0xaa,
                                   0xaa, 0xaa, 0xaa, 0xaa,
                                   0xaa, 0xaa, 0xaa, 0xaa,
                                   0xaa, 0xaa, 0xaa, 0xaa,};

    Encrypted_Hash h(0, 15000000, Encrypted_Hash::ADVERTISED);

    BYTE text1[] = {"hey, I have a secret (not drugs)"};
    BYTE buf1[SHA256_BLOCK_SIZE];

    Sha256 hasher = Sha256();
    hasher.update(text1, 32);
    hasher.digest(buf1);

    print_as_hex("Key   ", key, 16);
    print_as_hex("Sha256", buf1);

    BYTE cryptographed_hash[SHA256_BLOCK_SIZE];
    encrypt_hash(buf1, key, cryptographed_hash);

    print_as_hex("AES128", cryptographed_hash);

    const int bus_size = 32; // I32 = 32, I64 = 64
    const int sends = 8 * SHA256_BLOCK_SIZE / bus_size;

    cout << "Beggining transmission.\n";
    for (int i = 0; i < sends; i++) {
        cout << "    " << (i+1) << ". Sending [";

        int value = 0;

        const int parts_size = bus_size / 8;
        for (int j = 0; j < parts_size; ++j) {
            int part = cryptographed_hash[i * parts_size + j];
            value |= part << (j*8);
            cout << part << ", ";
        }

        cout << "] (" << value << ")...";
        h = value;
        cout << "Done.\n";
        Alarm::delay(1000000);
    }
    cout << "End transmission.\n";

    while (true) {}
    return 0;
}
