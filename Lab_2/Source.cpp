#include<iostream>
#include<fstream>
#include<string>
#include<cassert>
#include<map>
#include<stack>
using namespace std;



bool atLeastOneAlph(const string& str) {
	for (int i = 0; i < str.size(); i++)
		if (isalpha(str[i]))
			return true;
	return false;
}

bool parCheck(const string& str) {
	stack<char> s;
	for (int i = 0; i < str.size(); i++) {
		if (str[i] == '(')
			s.push(str[i]);
		else if (str[i] == ')')
			if (!s.empty() && s.top() == '(')
				s.pop();
			else
				return false;
	}
	return s.empty();
}

bool correct(const string& str) {
	if (parCheck(str) && atLeastOneAlph(str) && str[0] != '(')
		return true;
	return false;
}

int main() {
	ifstream fin("input.txt");
	ofstream fout("output.txt");

	string output;
	fin >> output;

	for (int i = 1; i <= output.size() / 2; i++) {
		for (int j = 0; (output.size() - j) / i > 1; j++) {
			int cnt = 0;
			string sample = output.substr(j, i);

			if (!correct(sample))
				continue;

			int k = 0;
			while (k < (output.size() - j) / i && output.substr(j + k * i, i) == sample) {
				k++;
				cnt++;
			}

			if (cnt > 1) {
				string newstr = to_string(cnt) + "(" + sample + ")";
				output.replace(j, k * i, move(newstr));
			}
		}
	}

	fout << output;

	fout.close();
	fin.close();
}