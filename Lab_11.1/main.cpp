#include<iostream>
#include"aes.h"
#include"tests.h"

using namespace std;

void print_hex(string_view sv) {
    for (int i = 0; i < sv.size(); i++)
        cout << hex << "0x" << ((uint)sv[i] & 0xff) << " ";
    cout << endl;
}

int main() {
    test_mix_columns();
    test_inv_mix_columns();
    test_key_expansion();
    test_encrypt_block();
    test_decrypt_block();

    const char* raw_key = "0123456789ABCDEF";
    uchar key[kBlockSize];
    for (int i = 0; i < kBlockSize; i++)
        key[i] = raw_key[i];

    string raw_text = "ABCDEF";
    cout << "Raw text:" << endl;
    cout << "\tPlain text: " << raw_text << endl;
    cout << "\tHex: ";
    print_hex(raw_text);
    cout << endl;

    string enc_text = Encrypt(raw_text, key);
    cout << "Encrypted text:" << endl;
    cout << "\tPlain text: " << enc_text << endl;
    cout << "\tHex: ";
    print_hex(enc_text);
    cout << endl;

    string dec_text = Decrypt(enc_text, key);
    cout << "Decrypted text:" << endl;
    cout << "\tPlain text: " << dec_text << endl;
    cout << "\tHex: ";
    print_hex(dec_text);
    cout << endl;
}