#include<iostream>
#include<vector>
#include<map>
#include<fstream>
#include<string>
#include<sstream>
#include<Windows.h>

using namespace std;

enum Right {
	NONE = 0,
	READ = 1 << 0,
	WRITE = 1 << 1
};

Right ParseRight(char c) {
	switch (c) {
	case 'r':
		return READ;
	case 'w':
		return WRITE;
	default:
		throw "Unknown Right: "s + c;
	}
}

ostream& operator<<(ostream& out, const vector<int>& v) {
	for (int i = 0; i < v.size(); i++)
		out << v[i] << " ";
	return out;
}

class SparceMatrix
{
	map<pair<int, int>, int> m_;
public:
	bool Exists(int i, int j) {
		if (m_.find({ i, j }) != m_.end())
			return true;
		return false;
	}

	int Get(int i, int j) {
		if (!Exists(i, j))
			return 0;

		int result;
		tie(std::ignore, result) = *m_.find({i, j});
		return result;
	}

	void Set(int i, int j, int var) {
		if (var == 0 && Exists(i, j))
			m_.erase(m_.find({i, j}));
		
		m_[{i, j}] = var;
	}

	void ClearCol(int j) {
		vector<map<pair<int, int>, int>::iterator> to_delete;
		for (auto iter = m_.begin(); iter != m_.end(); iter++)
			if (iter->first.second == j)
				to_delete.push_back(iter);
		
		for (int i = 0; i < to_delete.size(); i++)
			m_.erase(to_delete[i]);
	}

	void ClearRow(int i) {
		vector<map<pair<int, int>, int>::iterator> to_delete;
		for (auto iter = m_.begin(); iter != m_.end(); iter++)
			if (iter->first.first == i)
				to_delete.push_back(iter);

		for (int i = 0; i < to_delete.size(); i++)
			m_.erase(to_delete[i]);
	}

	int Size() {
		return m_.size();
	}

	map<pair<int, int>, int> const & GetMap() const {
		return m_;
	}
};

class RightsMatrix {
	SparceMatrix rights_;
	int objects_;
	int subjects_;

public:
	RightsMatrix() : objects_(0), subjects_(0) {};

	int AddSubject() {
		return subjects_++;
	}
	
	void DelSubject(int i) {
		rights_.ClearRow(i);
	}

	int AddObject() {
		return objects_++;
	}

	void DelObject(int i) {
		rights_.ClearCol(i);
	}

	void AddRight(int s, int o, Right r) {
		if (s > subjects_ || o > objects_) {
			stringstream error_stream;
			error_stream << "Subject " << s << " or object " << o << " doesn\'t exists.";
			throw error_stream.str();
		}
		rights_.Set(s, o, rights_.Get(s, o) | r);
	}

	void DelRight(int s, int o, Right r) {
		if (s > subjects_ || o > objects_) {
			stringstream error_stream;
			error_stream << "Subject " << s << " or object " << o << " doesn\'t exists.";
			throw error_stream.str();
		}
		rights_.Set(s, o, rights_.Get(s, o) & ~r);
	}

	int FullnessPercent() {
		return (double)rights_.Size() / (double)(objects_ * subjects_) * 100.f;
	}

	vector<int> FindPrivSubjs(int o, Right r) { // Найти привилегированные субъекты
		vector<int> result;
		for (auto it : rights_.GetMap()) 
			if (it.first.second == o && it.second & r) 
				result.push_back(it.first.first);
		return result;
	}

	friend ostream& operator<<(ostream& out, const RightsMatrix& rm) {
		for (auto i : rm.rights_.GetMap()) 
			out << i.first.first << " " << i.first.second << " " << (i.second & READ ? "r" : "") << (i.second & WRITE ? "w" : "") << endl;
		return out;
	}
};

class Interp {
	RightsMatrix rm_;
	ifstream iEnv_;
public:
	Interp(string e) : iEnv_(e) {
		int n, m;
		iEnv_ >> n >> m;
		for (int i = 0; i < n; i++)
			rm_.AddSubject();
		for (int i = 0; i < m; i++)
			rm_.AddObject();
		
		string temp;
		getline(iEnv_, temp); // Пропустим одну строку. Костыльно, но что поделать
		while (getline(iEnv_, temp)) {
			stringstream ss;
			ss << temp;
			int s, o;
			char c;
			ss >> s >> o >> c;
			AddRight(s, o, ParseRight(c));
		}
	};
	
	void AddSubj(string name) {
		int index = rm_.AddSubject();
		cout << "Субъект " << name << " добавлен под номером " << index << endl;
	}
	
	void AddObj(string name) {
		int index = rm_.AddObject();
		cout << "Объект " << name << " добавлен под номером " << index << endl;
	}

	void DelSubj(int s) {
		rm_.DelSubject(s);
	}

	void DelObj(int o) {
		rm_.DelObject(o);
	}

	void AddRight(int s, int o, Right r) {
		rm_.AddRight(s, o, r);
	}
	
	void DelRight(int s, int o, Right r) {
		rm_.DelRight(s, o, r);
	}

	void FullnessPercent() {
		cout << "Матрица заполнена на " << rm_.FullnessPercent() << "%" << endl;
	}

	void SubjWithRight(int o, Right r) {
		cout << rm_.FindPrivSubjs(o, r);
	}

	bool ParseCmd(stringstream& ss) {
		string cmd;
		ss >> cmd;

		if (cmd == "addSubj") {
			string name;
			ss >> name;
			AddSubj(name);
		}
		else if (cmd == "addObj") {
			string name;
			ss >> name;
			AddObj(name);
		}
		else if (cmd == "delSubj") {
			int id;
			ss >> id;
			DelSubj(id);
		}
		else if (cmd == "delObj") {
			int id;
			ss >> id;
			DelObj(id);
		}
		else if (cmd == "addRight") {
			int s, o;
			char c;
			ss >> s >> o >> c;
			AddRight(s, o, ParseRight(c));
		}
		else if (cmd == "delRight") {
			int s, o;
			char c;
			ss >> s >> o >> c;
			DelRight(s, o, ParseRight(c));
		}
		else if (cmd == "fullnessPercent") {
			FullnessPercent();
		}
		else if (cmd == "subjWithRight") {
			int o; 
			char c;
			ss >> o >> c;
			SubjWithRight(o, ParseRight(c));
		}
		else if (cmd == "exit") {
			return false;
		}
		else {
			throw "Unknown command: "s + cmd;
		}

		return true;
	}

	void Print() {
		cout << rm_ << endl;
	}
};


int main() {
	try {
		SetConsoleCP(1251);
		SetConsoleOutputCP(1251);
		string env = "environ";
		Interp interp(env);
		string cmd_line;
		while (getline(cin, cmd_line)) {
			stringstream ss;
			ss << cmd_line;
			if (!interp.ParseCmd(ss))
				break;
		}
		interp.Print();
	}
	catch (string s) {
		cout << s; 
	}
	catch (const char* cstr) {
		cout << cstr;
	}
}