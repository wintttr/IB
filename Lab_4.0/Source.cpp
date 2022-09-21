#include<iostream>
#include<string>
#include<fstream>
#include<list>
#include<vector>
#include<array>
#include<cassert>

using namespace std;

class C2 {
	vector<int> els;
	vector<int> cur;
	int n;
public:
	C2(int l) {
		assert(l >= 2);
		for (int i = 0; i < l; i++)
			els.push_back(i);

		cur.push_back(0);
		cur.push_back(1);

		n = l;
	}

	C2(const vector<int>& l) {
		assert(l.size() >= 2);
		copy(begin(l), end(l), back_inserter(els));

		cur.push_back(0);
		cur.push_back(1);

		n = els.size();
	}

	bool last() {
		if (cur[0] == n - 1 && cur[1] == n)
			return true;
		else
			return false;
	}

	vector<int> current() {
		vector<int> ret_val(2);
		ret_val[0] = els[cur[0]];
		ret_val[1] = els[cur[1]];
		return ret_val;
	}

	void next() {
		cur[1]++;
		if (cur[1] < n) {

		}
		else {
			cur[0]++;
			cur[1] = cur[0] + 1;
		}
	}
};

ostream& operator<<(ostream& out, const vector<int>& arr) {
	for (int i = 0; i < arr.size(); i++)
		out << arr[i];
	return out;
}

vector<int> diff(const vector<int>& reduct, const vector<int>& sub) {
	vector<int> res;
	for (int i = 0; i < reduct.size(); i++) {
		bool flag = true;
		for (int j = 0; j < sub.size(); j++)
			if (reduct[i] == sub[j]) {
				flag = false;
				break;
			}
		if (flag)
			res.push_back(reduct[i]);
	}
	return res;
}

int main() {
	vector<int> inds = {0, 1, 2, 3, 4, 5, 6, 7};
	C2 pos2(inds);
	int cnt = 0;
	ofstream fout("output.txt");
	while (!pos2.last()) {
		vector<int> v2 = pos2.current();
		vector<int> indv5 = diff(inds, v2);
		C2 pos5(indv5);
		while (!pos5.last()) {
			vector<int> v5 = pos5.current();
			vector<int> v7 = diff(indv5, v5);

			vector<int> comb(8);
			for (int i = 0; i < v2.size(); i++)
				comb[v2[i]] = 2;
			for (int i = 0; i < v5.size(); i++)
				comb[v5[i]] = 5;
			for (int i = 0; i < v7.size(); i++)
				comb[v7[i]] = 7;
			fout << comb << endl;

			pos5.next();
		}
		pos2.next();
	}
	fout.close();
}