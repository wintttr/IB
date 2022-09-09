#include<iostream>
#include<string>
#include<fstream>
#include<vector>

using namespace std;

bool fuzzy(const string& s1, const string& s2) {
	int error_count1 = 1;
	int error_count2 = 1;

	int i, j;
	for (i = 0, j = 0; i < s1.size() && j < s2.size(); i++, j++) {
		if (s1[i] == s2[j]);
		else if (s1[i + 1] == s2[j]) {
			i++;
			error_count1--;
		}
		else if (s1[i] == s2[j + 1]) {
			j++;
			error_count2--;
		}
		else if (s1[i + 1] == s2[j + 1]) {
			error_count1--;
			error_count2--;
		}
		else
			return false;

		if (error_count1 < 0 || error_count2 < 0)
			return false;
	}


	return true && i >= s1.size() - error_count1 && j >= s2.size() - error_count2;
}

int main() {
	ifstream fin("input.txt");
	ofstream fout("output.txt");

	int N, M;
	vector<string> dict;
	vector<string> check;

	fin >> N >> M;
	for (int i = 0; i < N; i++) {
		string temp;
		fin >> temp;
		dict.push_back(move(temp));
	}

	for (int i = 0; i < M; i++) {
		string temp;
		fin >> temp;
		check.push_back(move(temp));
	}

	for (int i = 0; i < M; i++) {
		int cnt = 0;
		int d;
		for (int j = 0; j < N; j++)
			if (fuzzy(check[i], dict[j])) {
				cnt++;
				d = j;
			}
		
		fout << check[i] << " " << cnt << " " << (cnt == 1 ? dict[d] : "") << endl;
	}
	
	fin.close();
	fout.close();
}