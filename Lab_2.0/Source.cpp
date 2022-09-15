#include<iostream>
#include<string>
#include<fstream>

using namespace std;

int main() {
	string raw;
	fstream fin("input.txt");
	fin >> raw;
	fin.close();

	string output;
	for (int i = 0; i < raw.size(); ) {
		char temp = raw[i];
		int count = 0;
		while (i < raw.size() && temp == raw[i]) {
			count++;
			i++;
		}
		if (count > 1)
			output += to_string(count);
		output += temp;
	}

	ofstream fout("output.txt");
	fout << output;
	fout.close();
}