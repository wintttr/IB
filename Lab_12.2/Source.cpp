#include<iostream>
#include<vector>
#include<set>
#include<stack>
#include<fstream>

#include"mini-gmp.h"
#include"mini-gmpxx.h"
#include"random.h"

using namespace std;

Random r;

// Быстрое возведение числа a в степень w по модулю n
mpz_class FastDegree(mpz_class a, mpz_class w, mpz_class n) {
	mpz_class S = 1, V = w, c = a;
	while (V != 0) {
		if (V % 2 == 1)
			S = (S * c) % n;
		V /= 2;
		c = (c * c) % n;
	}
	return a == 0 ? 0 : S;
}

// Рандомные 512 бит
mpz_class Random512bit() {
	mpz_class num = 0;

	for (int i = 0; i < 8; i++) {
		uint64_t temp = r.Next();
		uint32_t p1 = temp >> 32;
		uint32_t p2 = temp;
		num = (num << 32) | p1;
		num = (num << 32) | p2;
	}

	return num;
}

// Тест Миллера-Рабина
// Проверяет число n на простоту за rounds раундов
// Возвращает false, если число составное и
// true, если вероятно-простое
bool MillerRabin(mpz_class n, int rounds) {
	if (n % 2 == 0)
		return false;

	// Если число раундов > n-2, то не получится найти
	// нужное количество случайных уникальных чисел и 
	// алгоритм зациклится
	if (rounds > n - 2)
		rounds = n.get_ui() - 2;

	// Делим n - 1 на 2 пока делится, в итоге получаем 
	// выражение 2^s * t, где t - нечётное
	mpz_class s = 0, t = n - 1;
	do {
		t /= 2;
		s++;
	} while (t % 2 == 0);

	// Рандомим rounds чисел из промежутка [2, n - 1]
	vector<mpz_class> nums;
	for (int k = 0; k < rounds; k++) {
		mpz_class temp = (Random512bit() % (n - 2)) + 2;
		if (find(nums.begin(), nums.end(), temp) != nums.end()) {
			k--;
			continue;
		}
		nums.push_back(temp);
	}

	// Выполняем rounds раундов теста Миллера-Рабина
	for (const mpz_class& a : nums) {
		mpz_class x = FastDegree(a, t, n);
		if (x == 1 || x == n - 1)
			goto cntn;

		for (int i = 0; i < s - 1; i++) {
			x = FastDegree(x, 2, n);
			if (x == 1)
				return false;
			if (x == n - 1)
				goto cntn;
		}
		return false;
	cntn:
		continue;
	}

	return true;
}

bool IsPrime(mpz_class p) {
	for (mpz_class i = 2; i <= sqrt(p); i++)
		if (p % i == 0)
			return false;
	return true;
}

set<mpz_class> Factorization(mpz_class x) {
	set<mpz_class> primes;
	if (x % 2 == 0) {
		do {
			x /= 2;
		} while (x % 2 == 0);
		primes.insert(2);
	}
	for (mpz_class i = 3; i <= x; i += 2) {
		if (!MillerRabin(i, 10)) continue;
		else if (IsPrime(i) && x % i == 0) {
			do {
				x /= i;
			} while (x % i == 0);
			primes.insert(i);
		}
	}
	return primes;
}

// Алгоритм Евклида по поиску НОД
mpz_class GCD(mpz_class a, mpz_class b) {
	while (b != 0) {
		a %= b;
		swap(a, b);
	}
	return a;
}

// Проверка на взаимную простоту
bool IsCoPrime(mpz_class a, mpz_class b) {
	return GCD(a, b) == 1;
}

mpz_class ChooseRoot(mpz_class p) {
	mpz_class f = p - 1;
	for (mpz_class g = 2; g < p; g++) {
		if (FastDegree(g, f, p) != 1)
			continue;

		bool f = true;
		for (mpz_class l = 1; l < f; l++) {
			if (FastDegree(g, l, p) == 1) {
				f = false;
				break;
			}
		}
		if (f)
			return g;
	}

	throw;
}

// Уродливое шифрование
vector<mpz_class> Encrypt(const vector<uint8_t>& s, mpz_class y, mpz_class g, mpz_class p, mpz_class k) {
	const int charcount = 60;
	vector<mpz_class> v;
	size_t i = 0;
	for (; i + charcount - 1 < s.size(); i += charcount) {
		mpz_class c = 0;
		for (int j = 0; j < charcount; j++)
			c = (c << 8) | s[i + j];
		v.push_back(FastDegree(g, k, p));
		v.push_back(FastDegree(y, k, p) * c % p);
	}

	if (s.size() % charcount != 0) {
		int k = 0;
		mpz_class c = 0;
		for (; i < s.size(); i++, k++)
			c = (c << 8) | s[i];
		c <<= 8 * (charcount - k);
		v.push_back(FastDegree(g, k, p));
		v.push_back(FastDegree(y, k, p) * c % p);
	}
	return v;
}

// Уродливое расшифрование
vector<uint8_t> Decrypt(const vector<mpz_class>& s, mpz_class x, mpz_class p) {
	const int charcount = 60;

	vector<uint8_t> v;
	for(size_t i = 0; i < s.size() - 1; i += 2){
		const mpz_class& a = s[i];
		const mpz_class& b = s[i + 1];

		mpz_class M = (FastDegree(a, p - 1 - x, p) * b) % p;

		for (int i = 0; i < charcount / 4; i++) {
			for (int j = 0; j < 4; j++)
				v.push_back((mpz_class(M >> 32 * (charcount / 4 - i - 1)).get_ui() >> (8 * (3 - j))) & 0xff);
		}
	}
	return v;
}

vector<uint8_t> ConvertToVectorOfUnsignedChars(const vector<mpz_class>& mpv) {
	vector<uint8_t> result;
	for (auto i : mpv) {
		mpz_class temp = i;
		stack<uint8_t> st;
		while (temp != 0) {
			st.push(temp.get_ui() & 0xff);
			temp >>= 8;
		}

		while (!st.empty()) {
			result.push_back(st.top());
			st.pop();
		}
	}
	return result;
}

int main() {
	constexpr int round_count = 20;
	mpz_class p = Random512bit() | 1 | (mpz_class(1) << 511);

	cout << "Генерация p" << endl;
	while (!MillerRabin(p, round_count))
		p += 2;

	cout << "Генерация g" << endl;
	mpz_class g = ChooseRoot(p);

	mpz_class x = Random512bit() & ((mpz_class(1) << 511) - 1) | 1;
	mpz_class y = FastDegree(g, x, p);
	mpz_class k = Random512bit() & ((mpz_class(1) << 511) - 1) | 1;

	cout << "Генерация k" << endl;
	while (!IsCoPrime(k, p - 1))
		k += 2;

	string raw_text = "ты чертила обоссаная закрой свой рот может у тебя мать сдохла \
если ты такой даун с 5 iq говоришь то что мать сдохла м да ммм шутить про мать в \
2020 году класс ты гений";

	vector<uint8_t> raw_text_vector;
	copy(raw_text.begin(), raw_text.end(), back_inserter(raw_text_vector));
	vector<mpz_class> crypt_text_vector = Encrypt(raw_text_vector, y, g, p, k);
	vector<uint8_t> decrypt_text_vector = Decrypt(crypt_text_vector, x, p);

	cout << "Raw: " << endl;
	cout << "\tPlain text: " << raw_text << endl;
	cout << "\tHex:";
	for (auto i : raw_text_vector) {
		cout << hex << "0x" << (((uint32_t)i) & 0xff) << " ";
	}
	cout << endl << endl;

	vector<uint8_t> crypt_char_vector = ConvertToVectorOfUnsignedChars(crypt_text_vector);
	cout << "Encrypted text: " << endl;
	cout << "\tHex:";
	for (auto i : crypt_char_vector)
		cout << hex << "0x" << (((uint32_t)i) & 0xff) << " ";

	cout << endl << endl;

	cout << "Decrypted text: " << endl;
	cout << "\tPlain text: ";
	for (auto i : decrypt_text_vector)
		cout << i;
	cout << endl;

	cout << "\tHex:";
	for (auto i : decrypt_text_vector)
		cout << hex << "0x" << (((uint32_t)i) & 0xff) << " ";


	cout << endl << endl;
	cout << "Open key: " << endl;
	cout << "\ty: " << y.get_str() << endl;
	cout << "\tg: " << g.get_str() << endl;
	cout << "\tp: " << p.get_str() << endl;

	ofstream fout("secret.key");

	fout << "Secret key: " << endl;
	fout << "\tx: " << x.get_str() << endl;

	fout.close();
}