#include<iostream>
#include<fstream>
#include<string>

using namespace std;

int main() {
	string input;
	ifstream fin("input.txt");
	fin >> input;
	fin.close();

	string output;

	do {
		for (int i = 0; i < input.size(); i++) {
			if (i + 1 < input.size() && input[i] == input[i + 1]) {
				i++;
			}
			else
				output += input[i];
		}
		if (output.size() == input.size())
			break;

		input = move(output);
		output.clear();
	} while (true);

	ofstream fout("output.txt");
	fout << output;
	fout.close();
}
