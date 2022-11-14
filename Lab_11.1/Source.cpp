#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<iomanip>

using namespace std;

using uchar = uint8_t;
using uint = uint32_t;


constexpr int Nk = 4, Nb = 4, Nr = 10;

uchar Sbox[] = {
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
        0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
        0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

uchar InvSbox[] = {
        0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
        0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
        0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
        0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
        0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
        0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
        0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
        0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
        0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
        0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
        0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
        0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
        0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
        0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
        0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
        0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

uchar Rcon[] = {
        0x00000000,
        0x01000000,
        0x02000000,
        0x04000000,
        0x08000000,
        0x10000000,
        0x20000000,
        0x40000000,
        0x80000000,
        0x1b000000,
        0x36000000
};

uint Sum(uint op1, uint op2) {
    return op1 ^ op2;
}

uchar Mult(uchar op1, uchar op2) {
    const uint p = 0b100011011; // x^8 + x^4 + x^3 + x + 1

    uint result = 0;
    for (int i = 0; i < 8; i++) {
        if (op1 & (1 << i)) {
            result = Sum(result, op2 << i);
        }
    }

    while (result >= 256) {
        int d = 31 - __lzcnt(result);
        result = Sum(result, p << (d - 8));
    }

    return result;
}

void SubBytes(uchar state[4][4]) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            state[i][j] = Sbox[state[i][j]];
}

void InvSubBytes(uchar state[4][4]) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            state[i][j] = InvSbox[state[i][j]];
}

void MixColumns(uchar state[4][4]) {
    // Матрица, на которую умножаем:
    //    {2, 3, 1, 1}
    //    {1, 2, 3, 1}
    //    {1, 1, 2, 3}
    //    {3, 1, 1, 2}

    for (int i = 0; i < 4; i++) {
        int a = state[0][i];
        int b = state[1][i];
        int c = state[2][i];
        int d = state[3][i];
        state[0][i] = Sum(Sum(Sum(Mult(2, a), Mult(3, b)), c), d);
        state[1][i] = Sum(Sum(Sum(a, Mult(2, b)), Mult(3, c)), d);
        state[2][i] = Sum(Sum(Sum(a, b), Mult(2, c)), Mult(3, d));
        state[3][i] = Sum(Sum(Sum(Mult(3, a), b), c), Mult(2, d));
    }
}

void InvMixColumns(uchar state[4][4]) {
    // Матрица, на которую умножаем:
    //    {0x0e, 0x0b, 0x0d, 0x09}
    //    {0x09, 0x0e, 0x0b, 0x0d}
    //    {0x0d, 0x09, 0x0e, 0x0b}
    //    {0x0b, 0x0d, 0x09, 0x0e}

    for (int i = 0; i < 4; i++) {
        int a = state[0][i];
        int b = state[1][i];
        int c = state[2][i];
        int d = state[3][i];
        state[0][i] = Sum(Sum(Sum(Mult(0x0e, a), Mult(0x0b, b)), Mult(0x0d, c)), Mult(0x09, d));
        state[1][i] = Sum(Sum(Sum(Mult(0x09, a), Mult(0x0e, b)), Mult(0x0b, c)), Mult(0x0d, d));
        state[2][i] = Sum(Sum(Sum(Mult(0x0d, a), Mult(0x09, b)), Mult(0x0e, c)), Mult(0x0b, d));
        state[3][i] = Sum(Sum(Sum(Mult(0x0b, a), Mult(0x0d, b)), Mult(0x09, c)), Mult(0x0e, d));
    }
}

void AddRoundKey(uchar st[4][4], const uint* r) {
    uchar* state = reinterpret_cast<uchar*>(st);
    const uchar* round = reinterpret_cast<const uchar*>(r);
    for (int i = 0; i < 16; i++)
        state[i] ^= round[i];
}

uint SubWord(uint x) {
    uint temp = x;
    uchar* temp_as_bytes = reinterpret_cast<uchar*>(&temp);
    for (int i = 0; i < 4; i++)
        temp_as_bytes[i] = Sbox[temp_as_bytes[i]];
    return temp;
}

uint RotWord(uint x) {
    return (x << 8) | ((x&0xff000000) >> 24);
}

uint InvRotWord(uint x) {
    return (x >> 8) | ((x & 0xff) << 24);
}

void ShiftRows(uchar st[4][4]) {
    uint* state = reinterpret_cast<uint*>(st);

    state[1] = RotWord(state[1]);
    state[2] = RotWord(RotWord(state[2]));
    state[3] = RotWord(RotWord(RotWord(state[3])));
}

void InvShiftRows(uchar st[4][4]) {
    uint* state = reinterpret_cast<uint*>(st);

    state[1] = InvRotWord(state[1]);
    state[2] = InvRotWord(InvRotWord(state[2]));
    state[3] = InvRotWord(InvRotWord(InvRotWord(state[3])));
}

void KeyExpansion(const uchar* key, uint w[Nb*(Nr+1)]) {
    for (int i = 0; i < Nk; i++)
        w[i] = *reinterpret_cast<const uint*>(&key[i*4]);

    for (int i = 4; i < Nb * (Nr + 1); i++) {
        uint temp = w[i - 1];
        if (i % Nk == 0)
            temp = SubWord(RotWord(temp)) ^ Rcon[i / Nk];
        else if (Nk > 6 && i % Nk == 4)
            temp = SubWord(temp);
        w[i] = w[i - Nk] ^ temp;
    }
}

// Block == 128 bit == 16 byte
// Размер w = [Nb*(Nr+1)]
void EncryptBlock(const uchar* in, uchar* out, const uint* w) {
    uchar state[4][Nb];

    for (int r = 0; r < 4; r++)
        for (int c = 0; c < Nb; c++)
            state[r][c] = in[r + 4 * c];
    
    AddRoundKey(state, &w[0]);
    for (int round = 1; round < Nr; round++) {
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(state, &w[round * Nb]);
    }

    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(state, &w[Nr*Nb]);

    for (int r = 0; r < 4; r++)
        for (int c = 0; c < Nb; c++)
            out[r + 4 * c] = state[r][c];
}

// Block == 128 bit == 16 byte
void DecryptBlock(const uchar* in, uchar* out, const uint* w) { 
    uchar state[4][Nb];

    for (int r = 0; r < 4; r++)
        for (int c = 0; c < Nb; c++)
            state[r][c] = in[r + 4 * c];

    AddRoundKey(state, &w[Nr*Nb]);

    for (int round = Nr - 1; round > 0; round--) {
        InvShiftRows(state);
        InvSubBytes(state);
        AddRoundKey(state, &w[round * Nb]);
        InvMixColumns(state);
    }

    InvShiftRows(state);
    InvSubBytes(state);
    AddRoundKey(state, &w[0]);

    for (int r = 0; r < 4; r++)
        for (int c = 0; c < Nb; c++)
            out[r + 4 * c] = state[r][c];
}

string Encrypt(string_view text, uchar key[16]) {
    uchar* raw_result = new uchar[(text.size() / 16 + 1) * 16 + 1];
    raw_result[(text.size() / 16 + 1) * 16] = 0;
    uint w[Nb * (Nr + 1)];
    KeyExpansion(key, w);

    int i;
    for (i = 0; i < text.size() && (i + 15) < text.size(); i += 16)
        EncryptBlock(reinterpret_cast<const uchar*>(text.data()) + i, raw_result + i, w);
    
    if (i != text.size()) {
        uchar temp[16] = {}; // Такая инициализация заполняет массив нулями

        for (int j = (text.size() / 16) * 16; j < text.size(); j++)
            temp[j % 16] = text[j];
        
        EncryptBlock(temp, raw_result + i, w);
    }

    string result(reinterpret_cast<char*>(raw_result));
    delete[] raw_result;

    return result;
}

string Decrypt(string_view crypt_text, uchar key[16]) {
    uchar* raw_result = new uchar[(crypt_text.size() / 16 + 1) * 16 + 1];
    raw_result[(crypt_text.size() / 16 + 1) * 16] = 0;
    uint w[Nb * (Nr + 1)];
    KeyExpansion(key, w);

    int i;
    for (i = 0; i < crypt_text.size() && (i + 15) < crypt_text.size(); i += 16)
        DecryptBlock(reinterpret_cast<const uchar*>(crypt_text.data()) + i, raw_result + i, w);

    if (i != crypt_text.size()) {
        uchar temp[16] = {}; // Такая инициализация заполняет массив нулями

        for (int j = (crypt_text.size() / 16) * 16; j < crypt_text.size(); j++)
            temp[j % 16] = crypt_text[j];

        DecryptBlock(temp, raw_result + i, w);
    }

    string result(reinterpret_cast<char*>(raw_result));
    delete[] raw_result;

    return result;
}


int main() {
    const char* raw_key = "0123456789ABCDEF";
    uchar key[16];
    for (int i = 0; i < 16; i++)
        key[i] = raw_key[i];

    string raw_text = "ABCDEF"
        ;
    cout << "Raw text:" << endl;
    cout << "\Plain text: " << raw_text << endl;
    cout << "\tHex: ";
    for (int i = 0; i < raw_text.size(); i++)
        cout << hex << "0x" << ((uint)raw_text[i] & 0xff) << " ";
    cout << endl << endl;

    string enc_text = Encrypt(raw_text, key);
    cout << "Encrypted text:" << endl;
    cout << "\tPlain text: " << enc_text << endl;
    cout << "\tHex: ";
    for (int i = 0; i < enc_text.size(); i++)
        cout << hex << "0x" << ((uint) enc_text[i] & 0xff) << " ";
    cout << endl << endl;

    string dec_text = Decrypt(enc_text, key);
    cout << "Decrypted text:" << endl;
    cout << "\tPlain text: " << dec_text << endl;
    cout << "\tHex: ";
    for (int i = 0; i < dec_text.size(); i++)
        cout << hex << "0x" << ((uint) dec_text[i] & 0xff) << " ";
    cout << endl << endl;
}
