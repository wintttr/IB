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
	bool should_stop_;
public:
	Interp(string e) : iEnv_(e), should_stop_(false) {
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
			AddRight(ss);
		}
	};
	
	void AddSubj(stringstream& ss) {
		int index = rm_.AddSubject();
		string name;
		ss >> name;
		cout << "Субъект " << name << " добавлен под номером " << index << endl;
	}
	
	void AddObj(stringstream& ss) {
		int index = rm_.AddObject();
		string name;
		ss >> name;
		cout << "Объект " << name << " добавлен под номером " << index << endl;
	}

	void DelSubj(stringstream& ss) {
		int s;
		ss >> s;
		rm_.DelSubject(s);
	}

	void DelObj(stringstream& ss) {
		int o;
		ss >> o;
		rm_.DelObject(o);
	}

	void AddRight(stringstream& ss) {
		int s, o;
		char c;
		ss >> s >> o >> c;
		rm_.AddRight(s, o, ParseRight(c));
	}
	
	void DelRight(stringstream& ss) {
		int s, o;
		char c;
		ss >> s >> o >> c;
		rm_.DelRight(s, o, ParseRight(c));
	}

	void FullnessPercent(stringstream& ss) {
		cout << "Матрица заполнена на " << rm_.FullnessPercent() << "%" << endl;
	}

	void SubjWithRight(stringstream& ss) {
		int o;
		char c;
		ss >> o >> c;
		cout << rm_.FindPrivSubjs(o, ParseRight(c));
	}

	void Exit(stringstream& ss) {
		should_stop_ = true;
	}

	bool ParseCmd(stringstream& ss) {
		string cmd;
		ss >> cmd;

		using parse_func = void(Interp::*)(stringstream&);

		map<string, parse_func> cmd_func;
		cmd_func["addSubj"] = &Interp::AddSubj;
		cmd_func["addObj"] = &Interp::AddObj;
		cmd_func["delSubj"] = &Interp::DelSubj;
		cmd_func["delObj"] = &Interp::DelObj;
		cmd_func["addRight"] = &Interp::AddRight;
		cmd_func["delRight"] = &Interp::DelRight;
		cmd_func["fullnessPercent"] =&Interp::FullnessPercent;
		cmd_func["subjWithRight"] = &Interp::SubjWithRight;
		cmd_func["exit"] = &Interp::Exit;

		if(cmd_func.find(cmd) != cmd_func.end())
			(this->*cmd_func[cmd])(ss);
		else {
			throw "Unknown command: "s + cmd;
		}

		return should_stop_;
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
			if (interp.ParseCmd(ss))
				break;
		}
		interp.Print();
	}
	catch (string s) {
		cout << s; 
	}
}