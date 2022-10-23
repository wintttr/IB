#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<sstream>
#include"matrix.h"

using namespace std;

enum rights {
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
	vector<int> obj_image;
	vector<int> subj_image;
	matrix<int> matr;
public:
	HRU() : matr(0, 0) {}

	HRU(const matrix<int>& m) : matr(m) {
		for (int i = 0; i < m.getn(); i++)
			subj_image.push_back(i);
		for (int i = 0; i < m.getm(); i++)
			obj_image.push_back(i);
	}

	bool obj_exist(int image) {
		return find(obj_image.begin(), obj_image.end(), image) != obj_image.end();
	}

	bool subj_exist(int image) {
		return find(subj_image.begin(), subj_image.end(), image) != subj_image.end();
	}

	int find_object(int i) {
		if (obj_exist(i))
			return find(obj_image.begin(), obj_image.end(), i) - obj_image.begin();
		else
			throw "Object not found";
	}

	int find_subject(int i) {
		if (subj_exist(i))
			return find(subj_image.begin(), subj_image.end(), i) - subj_image.begin();
		else
			throw "Subject not found";
	}

	void create_obj(int image) {
		if (obj_exist(image))
			throw "Object already exists";

		obj_image.push_back(image);
		
		matr.resize(matr.getn(), matr.getm() + 1);

		for (int i = 0; i < matr.getn(); i++) {
			matr.set(i, matr.getm() - 1, 0);
		}
	}

	void create_subj(int image) {
		if (subj_exist(image))
			throw "Sybject already exists";

		subj_image.push_back(image);
		matr.resize(matr.getn() + 1, matr.getm());
			
		for (int i = 0; i < matr.getm(); i++)
			matr.set(matr.getn() - 1, i, 0);
	}

	void destroy_obj(int image) {
		if (!obj_exist(image))
			throw "Object doesn't exists";

		int index = find(obj_image.begin(), obj_image.end(), image) - obj_image.begin();
		for (int i = 0; i < matr.getn(); i++)
			matr.set(i, index, 0);
	}

	void destroy_subj(int image) {
		if (!subj_exist(image))
			throw "Subject doesn't exists";

		int index = find(subj_image.begin(), subj_image.end(), image) - subj_image.begin();
		for (int i = 0; i < matr.getm(); i++)
			matr.set(index, i, 0);
	}

	void enter(rights p, int i, int j) {
		matr.set(i, j, matr(i, j) | p);
	}

	void del(rights p, int i, int j) {
		matr.set(i, j, matr(i, j) & ~p);
	}

	bool check(rights p, int i, int j) {
		return matr(i, j) & p;
	}

	friend ostream& operator<<(ostream& out, const HRU& h) {
		return (out << h.matr);
	}
};

rights parseright(char c) {
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

int parserights(const string& s) {
	int result = 0;
	for (int i = 0; i < s.size(); i++)
		result |= parseright(s[i]);
	
	return result;
}

class interp {
	HRU h;
	ifstream env;
	ifstream prog;
	ofstream result;
public:
	interp(string e, string p, string r) : env(e), prog(p), result(r) {
		int n, m;
		env >> n;
		env >> m;
		matrix<int> temp(n, m);
		for(int i = 0; i < n; i++)
			for (int j = 0; j < m; j++) {
				string s;
				env >> s;
				temp.set(i, j, parserights(s));
			}
		h = HRU(temp);
	}

	void write_result() {
		result << h;
	}

	~interp() {
		write_result();
		env.close();
		prog.close();
		result.close();
	}

	void next_cmd(bool exec, stringstream& sstream) {
		string cmd;
		sstream >> cmd;
		cout << "Command: " << cmd << endl;
		if (cmd == "\'O+\'") {
			int image;
			sstream >> image;
			h.create_obj(image);
		}
		else if (cmd == "\'S+\'") {
			int image;
			sstream >> image;
			h.create_subj(image);
		}
		else if (cmd == "\'O-\'") {
			int image;
			sstream >> image;
			h.destroy_obj(image);
		}
		else if (cmd == "\'S-\'") {
			int image;
			sstream >> image;
			h.destroy_subj(image);
		}
		else if (cmd == "if") {
			string rstring;
			string ignore;
			string indexes;
			sstream >> rstring >> ignore >> indexes; // ignore == in
			rights r = parseright(rstring[0]);

			int i, j;
			int delim1 = indexes.find(','), delim2 = indexes.find(']');

			i = stoi(indexes.substr(2, delim1 - 2));
			j = stoi(indexes.substr(delim1 + 1, delim2 - delim1 - 1));

			i = h.find_subject(i);
			j = h.find_object(j);

			sstream >> ignore; // ignore == then
			next_cmd(h.check(r, i, j), sstream);
		}
		else if (cmd[cmd.size() - 2] == '+') {
			rights r = parseright(cmd[1]);
			int i, j;
			sstream >> i >> j;
			i = h.find_subject(i);
			j = h.find_object(j);
			h.enter(r, i, j);
		}
		else if (cmd[cmd.size() - 2] == '-') {
			rights r = parseright(cmd[1]);
			int i, j;
			sstream >> i >> j;

			i = h.find_subject(i);
			j = h.find_object(j);

			h.del(r, i, j);
		}
		else
			throw "Unknown command";
	}

	void run() {
		string line;
		cout << "Original matrix: " << endl;
		cout << h << endl;

		while (getline(prog, line)) {
			stringstream ss;

			if (line == "")
				continue;

			ss << line;
			next_cmd(true, ss);
			cout << h << endl;
		}
	}
};

int main() {
	try {
		interp I("environ", "progr", "rzlt");
		I.run();
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