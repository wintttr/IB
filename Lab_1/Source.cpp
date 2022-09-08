#include<fstream>
#include<iostream>
#include<vector>
#include<string>

using namespace std;

class mask {
	int n;
	string curMask;

public:
	mask(int _n) : n(_n) {
		curMask = string(_n, 'f');
	}

	const string& getMask() const & {
		return curMask;
	}

	string getMask() && {
		return move(curMask);
	}


	bool nextExists() {
		for (int i = 0; i < curMask.size(); i++)
			if (curMask[i] == 't')
				return true;
		return false;
	}

	void next() {
		for (int i = curMask.size() - 1; i >= 0; i--) {
			if (curMask[i] == 'f') {
				curMask[i] = 't';
				break;
			}
			else
				curMask[i] = 'f';
		}
	}
};


string inv(const string& s) {
	string result;
	for (int i = 0; i < s.size(); i++)
		result += s[i] == 't' ? 'f' : 't';
	return result;
}

int match(const string& mask, const string& str) {
	int cnt = 0;
	for (int i = 0; i < mask.size(); i++)
		if (mask[i] == str[i])
			cnt++;
	return cnt;
}

bool match(const string& mask, const vector<string>& v, const vector<int>& c) {
	for (int i = 0; i < v.size(); i++)
		if (match(mask, v[i]) != c[i])
			return false;
	return true;
}

int main() {
	// Начнём с ввода
	int N;

	ifstream fin("input.txt");
	ofstream fout("output.txt");

	vector<string> ans; // Ответы, введённые игроком
	vector<int> solutionCount; // Количество правильных ответов

	fin >> N;
	for (int i = 0; i < N; i++) {
		string temp;
		fin >> temp;
		if (temp.size() != 11)
			throw;
		solutionCount.push_back((int)(temp[10] - '0'));
		ans.push_back((move(temp)).substr(0, 10));
	}

	mask m(10); 
	string trueSolution;
	
	bool one = false;
	bool found = false;

	int solCnt = 0; // Количество масок, подходящих
					// под условие
					

	// Ищем решение, в котором было 0 правильных ответов
	// Если такое есть, то правильное решение - это данное
	// решение наоборот, причём оно единственное
	for (int i = 0; i < N; i++) 
		if (solutionCount[i] == 0) {
			trueSolution = move(inv(ans[i]));
			found = true;
			solCnt = 1;
		}


	if (!found)
		do {
			if (match(m.getMask(), ans, solutionCount)) {
				solCnt++;
				trueSolution = m.getMask();
			}

			m.next();
		} while (m.nextExists());

	fout << (solCnt == 1 ? "ONE SOLUTION" : "POSSIBLE SOLUTION") << endl;
	fout << trueSolution << endl;
	
	fin.close();
	fout.close();
}