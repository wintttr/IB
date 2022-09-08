#include<fstream>
#include<iostream>
#include<vector>
#include<string>

using namespace std;

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

string nextMask(const string& mask) {
	if (mask == "tttttttttt")
		return "";

	string newMask(mask);

	for (int i = newMask.size() - 1; i >= 0; i--) {
		if (newMask[i] == 'f') {
			newMask[i] = 't';
			break;
		}
		else
			newMask[i] = 'f';
	}
	
	return newMask;
}

int main() {
	int N;
	string mask{ 10, 'f' };

	string trueSolution;

	vector<string> ans;
	vector<int> solutionCount;

	bool one = false;

	ifstream fin("input.txt");
	ofstream fout("output.txt");

	fin >> N;

	for (int i = 0; i < N; i++) {
		string temp;
		fin >> temp;
		if (temp.size() != 11)
			throw;
		solutionCount.push_back((int)(temp[10] - '0'));
		ans.push_back((move(temp)).substr(0, 10));
	}

	for (int i = 0; i < N; i++)
		if (solutionCount[i] == 0) {
			trueSolution = move(inv(ans[i]));
			one = true;
		}

	int solCnt = 0;
	if (one != true)
		while (mask != "") {
			if (match(mask, ans, solutionCount)) {
				solCnt++;
				trueSolution = mask;
			}
			mask = nextMask(mask);
		}

	if (solCnt == 1)
		one = true;

	fout << (one ? "ONE SOLUTION" : "POSSIBLE SOLUTION") << endl;
	fout << trueSolution << endl;
	
	fin.close();
	fout.close();
}