#include<iostream>

using namespace std;

// a^w (mod n)
uint64_t FastDegree(uint64_t a, uint64_t w, uint64_t n) {
	uint64_t S = 1, V = w, c = a;
	while (V != 0) {
		if (V % 2 == 1)
			S = (S * c) % n;
		V /= 2;
		c = (c * c) % n;
	}
	return a == 0 ? 0 : S;
}

uint32_t GCD(uint32_t a, uint32_t b) {
	while (b) {
		a %= b;
		swap(a, b);
	}

	return a;
}

uint32_t AdvancedGCD(uint32_t a, uint32_t b, int* x, int* y) {
	if (a < b) {
		swap(a, b);
		swap(x, y);
	}
	int r0 = a, r1 = b, r2 = 0;
	int s0 = 1, s1 = 0, s2 = 0;
	int t0 = 0, t1 = 1, t2 = 0;
	
	do {
		r2 = r0 - (r0 / r1) * r1;
		s2 = s0 - (r0 / r1) * s1;
		t2 = t0 - (r0 / r1) * t1;

		r0 = r1; r1 = r2;
		s0 = s1; s1 = s2;
		t0 = t1; t1 = t2;
	} while (r2 != 0);

	if(x != nullptr)
		*x = s0;
	if(y != nullptr)
		*y = t0;
	return r0;
}

bool IsCoPrime(uint32_t a, uint32_t b) {
	return GCD(a, b) == 1;
}

uint32_t ChoosePublicExp(uint32_t f) {
	uint32_t cool[] = {3, 5, 17, 257, 65537};
	for (auto i : cool)
		if (i < f && IsCoPrime(i, f))
			return i;
	for (int i = 2; i < f; i++)
		if (IsCoPrime(i, f))
			return i;

	throw; // хз, возможно ли это, проверять лень
}

uint32_t ChoosePrivateExp(uint32_t f, uint32_t e) {
	int x;
	AdvancedGCD(e, f, &x, nullptr);
	return x > 0 ? x : x + f;
}

int main() {
	uint32_t p = 5, q = 7;
	uint32_t n = p * q, f = (p - 1) * (q - 1);
	uint32_t e = ChoosePublicExp(f);
	uint32_t d = ChoosePrivateExp(f, e);

	cout << e << " " << d;
}