#include<iostream>
#include<string>
#include<iomanip>
#include<cassert>
#include<intrin.h>
#include<bitset>

using namespace std;

using uchar = uint8_t;
using uint = uint32_t;

static const int kBlockSize = 8;

static const uchar kInvReplace[] = {
  58, 50, 42, 34, 26, 18, 10,  2,
  60, 52, 44, 36, 28, 20, 12,  4,
  62, 54, 46, 38, 30, 22, 14,  6,
  64, 56, 48, 40, 32, 24, 16,  8,
  57, 49, 41, 33, 25, 17,  9,  1,
  59, 51, 43, 35, 27, 19, 11,  3,
  61, 53, 45, 37, 29, 21, 13,  5,
  63, 55, 47, 39, 31, 23, 15,  7
};

static const uchar kReplace[] = {
  40,  8, 48, 16, 56, 24, 64, 32,
  39,  7, 47, 15, 55, 23, 63, 31,
  38,  6, 46, 14, 54, 22, 62, 30,
  37,  5, 45, 13, 53, 21, 61, 29,
  36,  4, 44, 12, 52, 20, 60, 28,
  35,  3, 43, 11, 51, 19, 59, 27,
  34,  2, 42, 10, 50, 18, 58, 26,
  33,  1, 41,  9, 49, 17, 57, 25
};

static const uchar S[8][4][16] = {
    {
        {14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7},
        { 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8},
        { 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0},
        {15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13}
    },

    {
        {15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10},
        { 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5},
        { 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15},
        {13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9}
    },

    {
        {10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8},
        {13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1},
        {13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7},
        { 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12}
    },

    {
        { 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15},
        {13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9},
        {10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4},
        { 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14}
    },
    
    {
        { 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9},
        {14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6},
        { 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14},
        {11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3}
    },

    {
        {12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11},
        {10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8},
        { 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6},
        { 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13}
    },

    {
        { 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1},
        {13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6},
        { 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2},
        { 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12}
    },

    {
        {13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7},
        { 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2},
        { 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8},
        { 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11}
    }
};

const uchar P[] = {
    16,  7, 20, 21, 29, 12, 28, 17,
     1, 15, 23, 26,  5, 18, 31, 10,
     2,  8, 24, 14, 32, 27,  3,  9,
    19, 13, 30,  6, 22, 11,  4, 25
};

const uchar compress[] = {
    14, 17, 11, 24,  1,  5,  3, 28,
    15,  6, 21, 10, 23, 19, 12,  4,
    26,  8, 16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55, 30, 40,
    51, 45, 33, 48, 44, 49, 39, 56,
    34, 53, 46, 42, 50, 36, 29, 32
};

const uchar keyshift[] = {
    57, 49, 41, 33, 25, 17,  9,  1,
    58, 50, 42, 34, 26, 18, 10,  2,
    59, 51, 43, 35, 27, 19, 11,  3, 
    60, 52, 44, 36, 63, 55, 47, 39,
    31, 23, 15,  7, 62, 54, 46, 38,
    30, 22, 14,  6, 61, 53, 45, 37,
    29, 21, 13,  5, 28, 20, 12,  4
};

uint SwapToBe(uint x) {
    if constexpr (endian::native == endian::little)
        x = ((x & 0xff) << 24) | ((x & 0xff00) << 8) | ((x & 0xff0000) >> 8) | ((x & 0xff000000) >> 24);
    return x;
}

uint64_t SwapToBe(uint64_t x) {
    if constexpr (endian::native == endian::little)
        x =  ((x & 0xff) << 56) | ((x & 0xff00) << 40) | ((x & 0xff0000) << 24) | ((x & 0xff000000) << 8) | 
             ((x & 0xff00000000) >> 8) | ((x & 0xff0000000000) >> 24) | ((x & 0xff000000000000) >> 40) | 
             ((x & 0xff00000000000000) >> 56);
    return x;
}

void test_s(const uchar s[4][16]) {
    for (int i = 0; i < 4; i++) {
        bool f[16] = {};
        for (int j = 0; j < 16; j++)
            f[j] = false;

        for (int j = 0; j < 16; j++) {
            if (f[s[i][j]])
                terminate();
            f[s[i][j]] = true;
        }
    }
}

uint64_t Replace(uint64_t x) {
    uint64_t result = 0;
    for (int i = 0; i < 64; i++)
        if (x & (1ull << i))
            result |= (1ull << (64 - kReplace[63 - i]));
    return result;
}

uint64_t InvReplace(uint64_t x) {
    uint64_t result = 0;
    for (int i = 0; i < 64; i++)
        if (x & (1ull << i))
            result |= (1ull << (64 - kInvReplace[63 - i]));
    return result;
}

uint32_t DES(uint32_t R, uint64_t key) {
    uint8_t key_parts[8] = {};

    // ????? ???? ?? 8 ?????? ?? 6 ??? ??????
    for (int i = 0; i < 8; i++) {
        key_parts[i] = (key & 0xFC00000000000000) >> 56;
        key <<= 6;
    }

    // ????????? past_R ? 32 ??? ?? 48 ???
    uint8_t R_parts[8] = {};
    for (int i = 0; i < 8; i++)
        R_parts[i] = ((R >> (32 - 4 * (i + 1))) & 0xf) << 3;
    for (int i = 1; i < 7; i++)
        R_parts[i] = ((R_parts[i - 1] & 0b00001000) << 4) | R_parts[i] | ((R_parts[i + 1] & 0b01000000) >> 4);
    R_parts[0] = ((R_parts[7] & 0b00001000) << 4) | R_parts[0] | ((R_parts[1] & 0b01000000) >> 4);
    R_parts[7] = ((R_parts[6] & 0b00001000) << 4) | R_parts[7] | ((R_parts[0] & 0b01000000) >> 4);

    // XOR??
    for (int i = 0; i < 8; i++)
        R_parts[i] ^= key_parts[i];

    // S-?????????????? (?????? ?? 32 ???)
    uint32_t new_R = 0;
    for (int i = 0; i < 8; i++) {
        uint8_t row = 0, col = 0;
        row = ((R_parts[i] >> 6) & 0b10) | ((R_parts[i] >> 2) & 1);
        col = (R_parts[i] >> 3) & 0xf;
        new_R = (new_R << 4) | S[i][row][col];
    }

    // ?????? ????????????
    uint32_t new_R2 = 0;
    for (int i = 0; i < 32; i++) {
        new_R2 = (new_R2 << 1) | ((new_R & (1ul << ((32 - P[i])))) ? 1 : 0);
    }

    return new_R2;
}

uint LeftShift(uint x, uint num) {
    uint temp = (x & (((1ul << num) - 1) << (32 - num))) >> (28 - num);
    return (x << num) | temp;
}

uint RightShift(uint x, uint num) {
    uint temp = (x & (((1ul << num) - 1) << 4)) << (28 - num);
    return ((x >> num) | temp) & 0xfffffff0;
}

uint64_t Compressor(uint32_t left, uint32_t right) {
    uint64_t leftnright = (static_cast<uint64_t>(left) << 32) | (static_cast<uint64_t>(right) << 4);
    uint64_t result = 0;

    for (int i = 0; i < 48; i++)
        result = (result << 1) | ((leftnright & (1ull << (64 - compress[i]))) ? 1 : 0);
    
    return result << 16;
}

void CreateKeyTable(const uchar raw_key[8], uint64_t keys[16]) {
    uint64_t key64 = 0;
    for (int i = 0; i < 8; i++)
        key64 = (key64 << 8) | raw_key[i];

    uint64_t key = 0;
    for (int i = 0; i < 56; i++)
        key = (key << 1) | ((key64 & (1ull << (64 - keyshift[i]))) ? 1 : 0);
    key <<= 8;

    uint32_t left = 0, right = 0;
    left  = (key & 0xfffffff000000000) >> 32;
    right = static_cast<uint>((key & 0x0000000fffffff00) >> 4);

    for (int i = 0; i < 16; i++) {
        if (i == 0 || i == 1 || i == 8 || i == 15) {
            left = LeftShift(left, 1);
            right = LeftShift(right, 1);
        }
        else {
            left = LeftShift(left, 2);
            right = LeftShift(right, 2);
        }
        keys[i] = Compressor(left, right);
    }
}

void Round(uint32_t past_L, uint32_t past_R, uint32_t* L, uint32_t* R, uint64_t round_key) {
    uint32_t new_R = DES(past_R, round_key);
    *R = past_L ^ new_R;
    *L = past_R;
}

void EncryptBlock(const uint8_t in_block[8], uint8_t out_block[8], const uint64_t r_keys[16]) {
    uint64_t word = 0;
    for (int i = 0; i < 8; i++)
        word = (word << 8) | in_block[i];

    word = Replace(word);

    uint32_t left = word >> 32;
    uint32_t right = word & 0xffffffff;
    for (int i = 0; i < 16; i++) {
        uint32_t new_L, new_R;
        Round(left, right, &new_L, &new_R, r_keys[i]);
        left = new_L;
        right = new_R;
    }
    swap(left, right);

    uint64_t out_word = (static_cast<uint64_t>(left) << 32) | static_cast<uint64_t>(right);

    out_word = InvReplace(out_word);

    for (int i = 0; i < 8; i++) {
        out_block[i] = (out_word >> (64 - 8 * (i + 1))) & 0xff;
    }
}

uint64_t PopCount(uint8_t x) {
    int count = 0;
    for (int i = 0; i < 8; i++)
        if (x & (1ull << i))
            count++;
    return count;
}

void KeyExpand(const uchar key_before[7], uchar key_after[8]) {
    uint64_t x = 0;
    for (int i = 0; i < 7; i++)
        x |= (x << 8) | key_before[i];
    x <<= 8;

    uint64_t result = 0;
    for (int i = 56; i > 0; i -= 7) {
        uint64_t temp = (x & (0xfeull << i)) >> i;
        temp |= 1 - PopCount(temp) % 2;
        result = (result << 8) | temp;
    }
    *reinterpret_cast<uint64_t*>(key_after) = SwapToBe(result);
}

void print_hex(const string_view sv) {
    for (int i = 0; i < sv.size(); i++)
        cout << hex << "0x" << ((uint)sv[i] & 0xff) << " ";
    cout << endl;
}

string Encrypt(const string_view text, const uchar key[7]) {
    uchar* raw_result = new uchar[(text.size() / (kBlockSize + 1) + 1) * kBlockSize + 1];
    raw_result[(text.size() / (kBlockSize + 1) + 1) * kBlockSize] = 0;
    uint64_t w[16];

    uchar cooked_key[8];
    KeyExpand(key, cooked_key);
    CreateKeyTable(cooked_key, w);

    int i;
    for (i = 0; i < text.size() && (i + kBlockSize - 1) < text.size(); i += kBlockSize)
        EncryptBlock(reinterpret_cast<const uchar*>(text.data()) + i, raw_result + i, w);

    if (i != text.size()) {
        uchar temp[kBlockSize] = {}; // ????? ????????????? ????????? ?????? ??????

        for (int j = (text.size() / kBlockSize) * kBlockSize; j < text.size(); j++)
            temp[j % kBlockSize] = text[j];

        EncryptBlock(temp, raw_result + i, w);
    }

    string result(reinterpret_cast<char*>(raw_result));
    delete[] raw_result;

    return result;
}

void Collapse(const string_view sv_key, uchar out_key[7]) {
    for (int i = 0; i < 7; i++)
        out_key[i] = 0;
    for (int k = 0, i = 0; i < sv_key.size(); i++, k = (k + 1) % 7) {
        out_key[k] ^= sv_key[i];
    }
}

int main() {
    uchar raw_key[7] = {};

    string key;
    cout << "Enter key: ";
    getline(cin, key);
    cout << endl;
    Collapse(key, raw_key);

    string text;
    cout << "Enter plain text: ";
    getline(cin, text);
    cout << endl;

    string cypher_text = Encrypt(text, raw_key);
    cout << "Plain text: " << cypher_text << endl;
    cout << "Hex: ";
    print_hex(cypher_text);
    cout << endl;
}