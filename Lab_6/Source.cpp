#include<iostream>
#include<vector>
#include"matrix.h"

using namespace std;

enum rights {READ, WRITE};

bool check_rights(const vector<int>& ls, const vector<int>& lo, const matrix<rights>& A)
{
	for (int i = 0; i < ls.size(); i++)
		for (int j = 0; j < lo.size(); j++)
			if (!((ls[i] > lo[j] && A(i, j) == WRITE) || (ls[i] <= lo[j] && A(i, j) == READ)))
				return false;
	return true;
}

bool get_rights_vectors(const matrix<rights>& A, vector<int>& ls, vector<int>& lo)
{
	ls.clear();
	lo.clear();

	for (int i = 0; i < A.getn(); i++) {
		int cnt_s = 1;

		for (int j = 0; j < A.getm(); j++)
			if (A(i, j) == WRITE)
				cnt_s++;

		ls.push_back(cnt_s);
	}

	for (int i = 0; i < A.getm(); i++) {
		int min = 1;

		for (int j = 0; j < A.getn(); j++)
			if (A(j, i) == READ && ls[j] > min)
				min = ls[j];

		lo.push_back(min);
	}

	return check_rights(ls, lo, A);
}

matrix<rights> random_rights(int n, int m)
{
	vector<int> ls, lo;
	matrix<rights> result(n, m);

	for (int i = 0; i < n; i++)
		ls.push_back(rand() % (n + m) + 1);

	for (int i = 0; i < m; i++)
		lo.push_back(rand() % (n + m) + 1);

	for (int i = 0; i < n; i++){
		for (int j = 0; j < m; j++) 
			if (ls[i] > lo[j])
				result.set(i, j, WRITE);
			else
				result.set(i, j, READ);
	}

	return result;
}

ostream& operator<<(ostream& out, const matrix<rights>& A) {
	for (int i = 0; i < A.getn(); i++) {
		for (int j = 0; j < A.getm(); j++)
			out << (A(i, j) == WRITE ? "w" : "r") << " ";
		out << endl;
	}
	return out;
}

ostream& operator<<(ostream& out, const vector<int>& vec) {
	for (int i = 0; i < vec.size(); i++)
		out << vec[i] << " ";
	out << endl;
	return out;
}

int main() {
	srand(time(NULL));
	int n = 10, m = 10;

	for (int i = 0; i < 100; i++) {
		matrix<rights> rights_system = random_rights(n, m);
		vector<int> ls, lo;
		bool successed = get_rights_vectors(rights_system, ls, lo);
		if (successed)
			cout << i << endl;
		else {
			cout << "Matrix: " << endl;
			cout << rights_system << endl;
			cout << "Vectors: " << endl;
			cout << "ls: " << ls;
			cout << "lo: " << ls;
			std::terminate();
		}
	}
}