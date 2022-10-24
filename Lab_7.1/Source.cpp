#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<sstream>
#include"matrix.h"
#include<map>

using namespace std;

enum Rights {
	NONE = 0,
	READ = 1 << 0,
	WRITE = 1 << 1,
	OWN = 1 << 2,
	EXEC = 1 << 3
};

ostream& operator<<(ostream& out, const matrix<int>& m) {
	for (int i = 0; i < m.getn(); i++) {
		for (int j = 0; j < m.getm(); j++) {
			if (m(i, j) & READ)
				out << 'r';

			if (m(i, j) & WRITE)
				out << 'w';

			if (m(i, j) & OWN)
				out << 'o';

			if (m(i, j) & EXEC)
				out << 'x';
			
			out << "\t";
		}
		out << endl;
	}
	return out;
}

class HRU {
	vector<int> obj_image_;
	vector<int> subj_image_;
	matrix<int> matr_;
public:
	HRU() : matr_(0, 0) {}

	HRU(const matrix<int>& m) : matr_(m) {
		for (int i = 0; i < m.getn(); i++)
			subj_image_.push_back(i);
		for (int i = 0; i < m.getm(); i++)
			obj_image_.push_back(i);
	}

	bool ObjExists(int image) {
		return find(obj_image_.begin(), obj_image_.end(), image) != obj_image_.end();
	}

	bool SubjExists(int image) {
		return find(subj_image_.begin(), subj_image_.end(), image) != subj_image_.end();
	}

	int FindObject(int i) {
		if (ObjExists(i))
			return find(obj_image_.begin(), obj_image_.end(), i) - obj_image_.begin();
		else
			throw "Object not found";
	}

	int FindSubject(int i) {
		if (SubjExists(i))
			return find(subj_image_.begin(), subj_image_.end(), i) - subj_image_.begin();
		else
			throw "Subject not found";
	}

	void CreateObj(int image) {
		if (ObjExists(image))
			throw "Object already exists";

		obj_image_.push_back(image);
		
		matr_.resize(matr_.getn(), matr_.getm() + 1);

		for (int i = 0; i < matr_.getn(); i++) {
			matr_.set(i, matr_.getm() - 1, 0);
		}
	}

	void CreateSubj(int image) {
		if (SubjExists(image))
			throw "Sybject already exists";

		subj_image_.push_back(image);
		matr_.resize(matr_.getn() + 1, matr_.getm());
			
		for (int i = 0; i < matr_.getm(); i++)
			matr_.set(matr_.getn() - 1, i, 0);
	}

	void DestroyObj(int image) {
		if (!ObjExists(image))
			throw "Object doesn't exists";

		int index = find(obj_image_.begin(), obj_image_.end(), image) - obj_image_.begin();
		for (int i = 0; i < matr_.getn(); i++)
			matr_.set(i, index, 0);
	}

	void DestroySubj(int image) {
		if (!SubjExists(image))
			throw "Subject doesn't exists";

		int index = find(subj_image_.begin(), subj_image_.end(), image) - subj_image_.begin();
		for (int i = 0; i < matr_.getm(); i++)
			matr_.set(index, i, 0);
	}

	void Enter(Rights p, int i, int j) {
		matr_.set(i, j, matr_(i, j) | p);
	}

	void Del(Rights p, int i, int j) {
		matr_.set(i, j, matr_(i, j) & ~p);
	}

	bool Check(Rights p, int i, int j) {
		return matr_(i, j) & p;
	}

	friend ostream& operator<<(ostream& out, const HRU& h) {
		return (out << h.matr_);
	}
};

Rights ParseRight(char c) {
	switch (c) {
	case '0':
		return NONE;
	case 'r':
		return READ;
	case 'w':
		return WRITE;
	case 'o':
		return OWN;
	case 'x':
		return EXEC;
	default:
		throw "Unknown symbol";
	}
}

int ParseRights(const string& s) {
	int result = 0;
	for (int i = 0; i < s.size(); i++)
		result |= ParseRight(s[i]);
	
	return result;
}

class Interp {
	HRU h;
	ifstream iEnv;
	ifstream iProg;
	ofstream oResult;
public:
		Interp(string e, string p, string r) : iEnv(e), iProg(p), oResult(r) {
		int n, m;
		iEnv >> n;
		iEnv >> m;
		matrix<int> temp(n, m);
		for(int i = 0; i < n; i++)
			for (int j = 0; j < m; j++) {
				string s;
				iEnv >> s;
				temp.set(i, j, ParseRights(s));
			}
		h = HRU(temp);
	}

	void WriteResult() {
		oResult << h;
	}

	~Interp() {
		WriteResult();
		iEnv.close();
		iProg.close();
		oResult.close();
	}

	void NextCmd(stringstream& sstream) {
		string cmd;
		sstream >> cmd;

		cout << "Command: " << cmd << endl;

		
		if (cmd == "\'O+\'") {
			int image;
			sstream >> image;
			h.CreateObj(image);
		}
		else if (cmd == "\'S+\'") {
			int image;
			sstream >> image;
			h.CreateSubj(image);
		}
		else if (cmd == "\'O-\'") {
			int image;
			sstream >> image;
			h.DestroyObj(image);
		}
		else if (cmd == "\'S-\'") {
			int image;
			sstream >> image;
			h.DestroySubj(image);
		}
		else if (cmd == "if") {
			string rstring;
			string ignore;
			string indexes;
			sstream >> rstring >> ignore >> indexes; // ignore == in
			Rights r = ParseRight(rstring[0]);

			int i, j;
			int delim1 = indexes.find(','), delim2 = indexes.find(']');

			i = stoi(indexes.substr(2, delim1 - 2));
			j = stoi(indexes.substr(delim1 + 1, delim2 - delim1 - 1));

			i = h.FindSubject(i);
			j = h.FindObject(j);

			sstream >> ignore; // ignore == then
			if(h.Check(r, i, j))
				NextCmd(sstream);
		}
		else if (cmd[cmd.size() - 2] == '+') {
			Rights r = ParseRight(cmd[1]);
			int i, j;
			sstream >> i >> j;
			i = h.FindSubject(i);
			j = h.FindObject(j);
			h.Enter(r, i, j);
		}
		else if (cmd[cmd.size() - 2] == '-') {
			Rights r = ParseRight(cmd[1]);
			int i, j;
			sstream >> i >> j;

			i = h.FindSubject(i);
			j = h.FindObject(j);

			h.Del(r, i, j);
		}
		else
			throw "Unknown command";
	}

	void Run() {
		string line;
		cout << "Original matrix: " << endl;
		cout << h << endl;

		while (getline(iProg, line)) {
			stringstream ss;

			if (line == "")
				continue;

			ss << line;
			NextCmd(ss);
			cout << h << endl;
		}
	}
};

int main() {
	try {
		Interp interp("environ", "progr", "rzlt");
		interp.Run();
	}
	catch (const char& str) {
		cout << str << endl;
		std::terminate();
	}
	catch (exception e) {
		cout << e.what() << endl;
		std::terminate();
	}	
}