#include<iostream>
#include"Header.h"

using namespace std;

constexpr unsigned int kBlockSize = 64;
constexpr unsigned int kBlockBitSize = (kBlockSize * 8);

constexpr unsigned int kOutputSize_512 = 64;
constexpr unsigned int kOutputSize_256 = 32;

enum HashSize{HASH256, HASH512};

class StribogHash {
	uint8_t h_[kBlockSize];
	uint8_t N_[kBlockSize];
	uint8_t S_[kBlockSize];

	HashSize size_;

public:
	StribogHash(HashSize size) : size_(size) {
		memset(N_, 0x00, kBlockSize);
		memset(S_, 0x00, kBlockSize);

		size_ = size;

		if (size_ == HASH512)
			memset(h_, 0, kBlockSize);
		else
			memset(h_, 1, kBlockSize);
	}

	void addmod512(uint8_t* dst, uint8_t* src, uint8_t* add) {
		uint8_t i;
		uint16_t overrun = 0;

		for (i = kBlockSize; i-- > 0;) {
			overrun = src[i] + add[i] + (overrun >> 8);
			dst[i] = (uint8_t)overrun;
		}
	}

	void addmod512_u32(uint8_t* dst, uint8_t* src, uint32_t add) {
		uint8_t i;

		for (i = kBlockSize; i-- > 0;) {
			add = (uint32_t)src[i] + add;
			dst[i] = add;
			add >>= 8;
		}
	}

	void xor512(uint8_t* dst, uint8_t* a, uint8_t* b) {
		uint8_t i;

		for (i = 0; i < kBlockSize; i++) {
			dst[i] = a[i] ^ b[i];
		}
	}

	void S(uint8_t* vect) {
		uint8_t i;

		for (i = 0; i < kBlockSize; i++) {
			vect[i] = sbox[vect[i]];
		}
	}

	void LP(uint8_t* vect) {
		uint8_t i, j, k;

		uint8_t tmp[kBlockSize];

		memcpy(tmp, vect, kBlockSize);

		uint64_t c;

		for (i = 0; i < 8; i++) {
			c = 0;
			for (j = 0; j < 8; j++) {
				for (k = 0; k < 8; k++) {
					if (tmp[j * 8 + i] & 0x80 >> k)
						c ^= A[j * 8 + k];
				}
			}

			for (j = 0; j < 8; j++) {
				vect[i * 8 + j] = c >> (7 - j) * 8;
			}
		}
	}

	void X(uint8_t* dst, uint8_t* a, uint8_t* b)
	{
		xor512(dst, a, b);
	}

	void E(uint8_t* dst, uint8_t* k, uint8_t* m)
	{
		uint8_t i;

		uint8_t K[kBlockSize];

		memcpy(K, k, kBlockSize);

		X(dst, K, m);

		for (i = 1; i < 13; i++) {
			this->S(dst);
			LP(dst);

			X(K, K, C[i - 1]);

			S(K);
			LP(K);

			X(dst, K, dst);
		}
	}

	void g_N(uint8_t* h, uint8_t* N, uint8_t* m)
	{
		uint8_t hash[kBlockSize];
		memcpy(hash, h, kBlockSize);

		xor512(h, h, N);

		S(h);
		LP(h);

		E(h, h, m);

		xor512(h, h, hash);
		xor512(h, h, m);
	}

	void g_0(uint8_t* h, uint8_t* m)
	{
		uint8_t hash[64];
		memcpy(hash, h, kBlockSize);

		S(h);
		LP(h);

		E(h, h, m);

		xor512(h, h, hash);
		xor512(h, h, m);
	}

	
	friend string Encrypt(const string_view sv, HashSize size);
};

string Encrypt(const string_view sv, HashSize size)
{
	StribogHash temp(size);

	const unsigned char* message = reinterpret_cast<const unsigned char*>(sv.data());
	uint64_t len = sv.size();
	uint8_t m[kBlockSize];

	uint8_t padding;

	while (len >= kBlockSize) {
		memcpy(m, message + len - kBlockSize, kBlockSize);

		temp.g_N(temp.h_, temp.N_, m);

		len -= kBlockSize;

		temp.addmod512_u32(temp.N_, temp.N_, kBlockBitSize);
		temp.addmod512(temp.S_, temp.S_, m);
	}

	padding = kBlockSize - len;

	if (padding) {
		memset(m, 0x00, padding - 1);
		memset(m + padding - 1, 0x01, 1);
		memcpy(m + padding, message, len);
	}

	temp.g_N(temp.h_, temp.N_, m);

	temp.addmod512_u32(temp.N_, temp.N_, len * 8);
	temp.addmod512(temp.S_, temp.S_, m);

	temp.g_0(temp.h_, temp.N_);
	temp.g_0(temp.h_, temp.S_);

	string result;
	for (int i = 0; i < (size == HASH512 ? kOutputSize_512 : kOutputSize_256); i++)
		result += temp.h_[i];

	return result;
}


int main() {
	for (auto i : Encrypt("Testestestest1", HASH256))
		cout << hex << "0x" << (((uint32_t)i) & 0xff) << " ";
	cout << endl << endl;

	for (auto i : Encrypt("Testestestest2", HASH256))
		cout << hex << "0x" << (((uint32_t)i) & 0xff) << " ";
	cout << endl << endl;
}