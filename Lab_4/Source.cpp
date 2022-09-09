#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<map>

using namespace std;

using bigint = int64_t;
using biguint = uint64_t;

const int small_primes[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53 };

biguint divUntilItCan(bigint& x, bigint div){
	biguint n = 0;
	do { n++; x /= div; } while (x % div == 0);
	return n;
}

bool prime(bigint x) {
	for (bigint i = 2; i <= sqrt(x) + 1; i++)
		if (x % i == 0)
			return false;
	return true;
}

map<bigint, biguint> fact(bigint x) {
	map<bigint, biguint> divs;
	for (bigint i : small_primes) {
		if (x % i == 0) {
			biguint n = divUntilItCan(x, i);
			divs.insert({ i, n });
		}
	}

	bigint large_prime = 59;
	while (x > 1) {
		if (prime(large_prime) && x % large_prime == 0) {
			bigint n = divUntilItCan(x, large_prime);
			divs.insert({ large_prime, n });
		}
		large_prime += 2;
	}

	return divs;
}

biguint factorial(biguint x) {
	if (x <= 1) return 1;
	else {
		biguint p = 1;
		for (biguint i = 1; i <= x; i++)
			p *= i;
		return p;
	}
}

biguint C(biguint n, biguint k) {
	biguint proizv = 1;
	for (biguint i = k + 1; i <= n; i++)
		proizv *= i;
	return proizv / factorial(n-k);
}

biguint f(biguint k) {
	biguint result = 1;
	map<bigint, biguint> divs = fact(k);
	biguint count = 0;
	for (const auto& [i, j] : divs)
		count += j;

	biguint put = 0;
	for (const auto& [i, j] : divs) {
		result *= C(count - put, j);
		put += j;
	}

	return result;
}

int main() {
	ifstream fin("naturals.in");
	ofstream fout("naturals.out");

	vector<biguint> nums;

	biguint n;
	while (fin >> n) 
		nums.push_back(n);
	
	for (const auto& i : nums) {
		int j = 1;
		for (; f(j) != i; j++);
		fout << i << " " << j << endl;
	}

	fin.close();
	fout.close();
}