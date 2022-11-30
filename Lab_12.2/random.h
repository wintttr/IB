#pragma once
#include <cstdint>
#include <ctime>

// де√енератор рандомных чисел
class Random {
	uint64_t s[4];

	static inline uint64_t rotl(const uint64_t x, int k) {
		return (x << k) | (x >> (64 - k));
	}

public:
	uint64_t Next() {
		const uint64_t result = rotl(s[0] + s[3], 23) + s[0];
		const uint64_t t = s[1] << 17;
		s[2] ^= s[0];
		s[3] ^= s[1];
		s[1] ^= s[2];
		s[0] ^= s[3];
		s[2] ^= t;
		s[3] = rotl(s[3], 45);
		return result;
	}

	Random() {
		for (int i = 0; i < 4; i++)
			s[i] = time(0)*(i+1) - i;
		for (int i = 0; i < 4; i++)
			s[i] = Next();
	}
};