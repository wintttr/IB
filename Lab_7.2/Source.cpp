#include<iostream>
#include<fstream>
#include<vector>
#include"matrix.h"

using namespace std;

enum rights {
	NONE = 0,
	READ = 1 << 0,
	WRITE = 1 << 1
};

ostream& operator<<(ostream& out, vector<int> v) {
	for (int i = 0; i < v.size(); i++)
		out << v[i] << " ";
	return out;
}

vector<int> no_rights_objects(const matrix<int>& m) {
	vector<int> result;
	for (int j = 0; j < m.getm(); j++) {
		bool f = true;
		for (int i = 0; i < m.getn(); i++)
			if (m(i, j) != 0) {
				f = false;
				break;
			}

		if (f)
			result.push_back(j);
	}

	return result;
}

vector<int> no_rights_subjects(const matrix<int>& m) {
	vector<int> result;
	for (int i = 0; i < m.getn(); i++){
		bool f = true;
		for (int j = 0; j < m.getm(); j++)
			if (m(i, j) != 0) {
				f = false;
				break;
			}

		if (f)
			result.push_back(i);
	}

	return result;
}

vector<int> all_rights_subjects(const matrix<int>& m) {
	vector<int> result;
	for (int i = 0; i < m.getn(); i++) {
		bool f = true;
		for (int j = 0; j < m.getm(); j++)
			if (m(i, j) != (READ | WRITE)) {
				f = false;
				break;
			}

		if (f)
			result.push_back(i);
	}

	return result;
}

vector<vector<int>> collision_subjects(const matrix<int>& m) {
	vector<vector<int>> result;
	for (int i = 0; i < m.getm(); i++) {
		vector<int> subj;
		for (int j = 0; j < m.getn(); j++) {
			if (m(i, j) & WRITE)
				subj.push_back(j);
		}
		if (subj.size() > 1)
			result.push_back(move(subj));
	}
	return result;
}

vector<int> all_rights_to_one_object(const matrix<int>& m) {
	vector<int> result;

	for (int i = 0; i < m.getn(); i++) {
		int cnt = 0;
		for (int j = 0; j < m.getm(); j++) 
			if (m(i, j) & WRITE)
				cnt++;
		
		if (cnt == 1)
			result.push_back(i);
	}

	return result;
}

rights parseright(char c) {
	switch (c) {
	case 'r':
		return READ;
	case 'w':
		return WRITE;
	case '0':
		return NONE;
	default:
		throw "Unknown symbol: "s + c;
	}
}

int main() {
	try {
		ifstream iFile("access_matr");
		int n, m;
		iFile >> n >> m;
		matrix<int> matr(n, m);
		for (int i = 0; i < n; i++)
			for (int j = 0; j < m; j++) {
				string temp;
				int r = 0;
				iFile >> temp;
				for (char k : temp)
					r |= parseright(k);

				matr.set(i, j, r);
			}
		iFile.close();

		cout << "a: " << no_rights_objects(matr) << endl;
		cout << "b: " << no_rights_subjects(matr) << endl;
		cout << "c: " << all_rights_subjects(matr) << endl;
		cout << "d:" << endl;
		vector<vector<int>> d = collision_subjects(matr);
		for (int i = 0; i < d.size(); i++) {
			cout << "\t" << "C" << i << ": " << d[i] << endl;
		}
		cout << "e: " << all_rights_to_one_object(matr) << endl;
	}
	catch (string s) {
		cout << s << endl;
		std::terminate();
	}
	catch (...) {
		cout << "Неопознанная ошибка!" << endl;
		std::terminate();
	}
}