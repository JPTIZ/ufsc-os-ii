#include <aes.h>

using namespace EPOS;

OStream cout;

void show(const unsigned char input[],
          const unsigned char output[],
          size_t length = 16) {
    for (int i = 0; i < length; ++i) {
        cout << input[i] << ",";
    }
    cout << "\n    became:\n";
    for (int i = 0; i < length; ++i) {
        cout << output[i] << ",";
     }
     cout << "\n--\n";
}

int main() {
    // Data
    const unsigned char key[16] = {0xaa, 0xaa, 0xaa, 0xaa,
                                   0xaa, 0xaa, 0xaa, 0xaa,
                                   0xaa, 0xaa, 0xaa, 0xaa,
                                   0xaa, 0xaa, 0xaa, 0xaa,};
    const unsigned char input[16] = "Hello, you litt";//le 32-bit world!";
    unsigned char output[16];
    unsigned char back[16];

    // Encrypt
    AES_Common::AES<16> enigma = AES_Common::AES<16>(AES_Common::CBC);

    enigma.encrypt(input, key, output);

    show(input, output);

    enigma.decrypt(output, key, back);

    show(output, back);

    // Welp
    while (true) {}
}