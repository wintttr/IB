#include<iostream>
#include"mini-gmp.h"
#include"mini-gmpxx.h"
#include"random.h"

#include<fstream>
#include<vector>
#include<bitset>
#include<chrono>
#include<stack>
#include"Windows.h"

using namespace std;
using namespace chrono;

static Random r;

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

// Алгоритм Евклида по поиску НОД
mpz_class GCD(mpz_class a, mpz_class b) {
	while (b != 0) {
		a %= b;
		swap(a, b);
	}

	return a;
}

// Расширенный алгоритм Евклида (хз, зачем сохранять
// тот, который выше, если уже есть расширенный
// но пусть будет)
mpz_class AdvancedGCD(mpz_class a, mpz_class b, mpz_class* x, mpz_class* y) {
	if (a < b) {
		swap(a, b);
		swap(x, y);
	}

	mpz_class r0 = a, r1 = b, r2 = 0;
	mpz_class s0 = 1, s1 = 0, s2 = 0;
	mpz_class t0 = 0, t1 = 1, t2 = 0;
	
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

// Проверка на взаимную простоту
bool IsCoPrime(mpz_class a, mpz_class b) {
	return GCD(a, b) == 1;
}

// Выбор публичной экспоненты
mpz_class ChoosePublicExp(mpz_class f) {
	uint32_t cool[] = {17, 257, 65537};
	for (auto i : cool)
		if (i < f && IsCoPrime(i, f))
			return i;
	
	for (mpz_class i = 2; i < f; i++)
		if (IsCoPrime(i, f))
			return i;

	throw; // хз, возможно ли это, проверять лень
}

// Выбор приватной экспоненты
mpz_class ChoosePrivateExp(mpz_class f, mpz_class e) {
	mpz_class x;
	AdvancedGCD(e, f, &x, nullptr);
	return x > 0 ? x : x + f;
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

// Уродливое шифрование
// Надо бы кодировать хотя бы по 4 байта, ибо шифро-
// текст раздувает неимоверно, но мне так лень это 
// делать :с
vector<mpz_class> Encrypt(const vector<uint8_t>& s, mpz_class e, mpz_class n) {
	const int charcount = 124;
	vector<mpz_class> v;
	size_t i = 0;
	for (; i + charcount-1 < s.size(); i += charcount) {
		mpz_class c = 0;
		for (int j = 0; j < charcount; j++)
			c = (c << 8) | s[i + j];
		v.push_back(FastDegree(c, e, n));
	}

	if (s.size() % charcount != 0) {
		int k = 0;
		mpz_class c = 0;
		for (; i < s.size(); i++, k++)
			c = (c << 8) | s[i];
		c <<= 8 * (charcount - k);
		v.push_back(FastDegree(c, e, n));
	}
	return v;
}

// Уродливое расшифрование
vector<uint8_t> Decrypt(const vector<mpz_class>& s, mpz_class d, mpz_class n) {
	const int charcount = 124;

	vector<uint8_t> v;
	for (auto c : s) {
		mpz_class a = FastDegree(c, d, n);
		for (int i = 0; i < charcount / 4; i++) {
			for (int j = 0; j < 4; j++)
				v.push_back((mpz_class(a >> 32 * (charcount / 4 - i - 1)).get_ui() >> (8 * (3 - j))) & 0xff);
		}
	}
	return v;
}

// Лучше пока придумать не смог
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
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);
	// Количество раундов теста Миллера-Рабина. Чем 
	// больше - тем меньше вероятность того, что 

	// вероятно простое число вдруг окажется составным
	size_t round_count = 20;

	// Рандомизируем два 512-битных числа
	mpz_class p = Random512bit(), q = Random512bit();
	
	// Выставляем старший бит в единичку, чтобы они 
	// точно были 512-битными и младший бит, чтобы они
	// точно были нечётными
	p |= (mpz_class(1) << 511) | 1;
	q |= (mpz_class(1) << 511) | 1;

	// Хз зачем, просто по приколу
	p &= (mpz_class(1) << 512) - 1 - (mpz_class(1) << 64);
	q &= (mpz_class(1) << 512) - 1 - (mpz_class(1) << 64);

	// Пока p не проходит тест Миллера-Рабина как 
	// вероятно-простое, увеличиваем на 2 и проходим заново
	while (!MillerRabin(p, round_count))
		p += 2;

	// с q то же самое
	while (!MillerRabin(q, round_count))
		q += 2;

	// в итоге p и q простые с вероятностью примерно 1 - 2^(-round_count);

	// f - функция Эйлера числа n
	mpz_class n = p * q, f = (p - 1) * (q - 1); 

	mpz_class e = ChoosePublicExp(f);		// Публичная экспонента
	mpz_class d = ChoosePrivateExp(f, e);	// Приватная экспонента

	string raw_text = "test";

	vector<uint8_t> raw_text_vector;
	copy(raw_text.begin(), raw_text.end(),	back_inserter(raw_text_vector));
	vector<mpz_class> crypt_text_vector = Encrypt(raw_text_vector, e, n);
	vector<uint8_t> decrypt_text_vector = Decrypt(crypt_text_vector, d, n);

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
	cout << "\te: " << e.get_str() << endl;
	cout << "\tn: " << n.get_str() << endl;

	ofstream fout("secret.key");

	fout << "Secret key: " << endl;
	fout << "\td: " << d.get_str() << endl;
	fout << "\tn: " << n.get_str() << endl;

	fout.close();
}