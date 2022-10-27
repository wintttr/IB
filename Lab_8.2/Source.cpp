#include<iostream>
#include<vector>
#include<set>
#include<map>
#include<sstream>
#include<fstream>

using namespace std;

enum Access {READ, WRITE};

bool IsSpace(int c) {
	return c == ' ' || c == '\t' || c == '\n';
}

template<typename T>
vector<string> Split(const string& s, T is_delimeter) {
	vector<string> result;

	size_t word_start = 0;
	bool in_word = false;
	for (size_t i = 0; i < s.size(); i++) {
		if (!is_delimeter(s[i]) && !in_word) {
			in_word = true;
			word_start = i;
		}
		else if (is_delimeter(s[i]) && in_word) {
			in_word = false;
			result.emplace_back(s.substr(word_start, i - word_start));
		}
	}

	if (in_word) result.emplace_back(s.substr(word_start, s.size() - word_start));

	return result;
}

void tolower(string& str) {
	for (int i = 0; i < str.size(); i++)
		str[i] = tolower(str[i]);
}

class ChineseWall {
	int n_, m_, f_;
	map<int, int> stock_;			// id объекта -> id фирмы
	map<int, int> collision_;		// id фирмы   -> id класса конфликта
	map<int, set<pair<int, Access>>> history;		// История обращения субъекта к объектам
public:
	ChineseWall() : n_(0), m_(0), f_(0) {}
	ChineseWall(int n, map<int, int> st, map<int, int> col) : n_(n), m_(st.size()), f_(col.size()) {}

	bool IsSubjInHistory(int s) {
		return history.find(s) != history.end();
	}

	void Start() {
		history.clear();
	}
	
	bool Read(int s, int o) {
		bool f = true;
		if (s >= n_ || o >= m_)
			throw "Subject "s + to_string(s) + " or object "s + to_string(o) + " doesn't exist.";

		if (IsSubjInHistory(s)) {
			const set<pair<int, Access>>& hs = history[s];

			for (auto &[i, _] : hs) {
				if (stock_[o] == stock_[i]) {
					f = true;
					break;
				}
				if (collision_[stock_[o]] == collision_[stock_[i]]) {
					f = false;
					break;
				}
			}
		}
		else 
			history[s] = set<pair<int, Access>>();

		if (f)
			history[s].insert({ o , READ});
		return f;
	}

	bool Write(int s, int o) {
		if (!Read(s, 0))
			return false;

		if (s >= n_ || o >= m_)
			throw "Subject "s + to_string(s) + " or object "s + to_string(o) + " doesn't exist.";
		
		bool f = true;

		if (IsSubjInHistory(s)) {
			const set<pair<int, Access>>& hs = history[s];
			int firm_own_o = stock_[o];
			for (auto &[i, r] : hs) {
				int firm_own_i = stock_[i];
				if (r == READ && firm_own_i != firm_own_o && collision_[firm_own_i] == collision_[firm_own_o]) {
					f = false;
					break;
				}
			}
		}
		else
			history[s] = set<pair<int, Access>>();
			
		if (f)
			history[s].insert({ o, WRITE });

		return f;
	}

	void ReportSubj(int s) {
		if (s >= n_)
			throw "Subject "s + to_string(s) + " doesn't exist.";
		
		for (auto& [i, r] : history[s]) {
			string r_w = (r == READ ? "READ" : "WRITE");
			cout << r_w << ": obj(" << i << "), firm(" << stock_[i] << ")" << endl;
		}
	}

	void ReportObj(int o) {
		if (o >= n_)
			throw "Object "s + to_string(o) + " doesn't exist.";

		for (int s = 0; s < n_; s++) {
			for (auto& [i, r] : history[s]) {
				if (i == o) {
					string r_w = (r == READ ? "READ" : "WRITE");
					cout << r_w << ": subj(" << i << ")" << endl;
				}
			}
		}
	}

	void BriefCase(int f) {
		if(f > f_)
			throw "Firm "s + to_string(f) + " doesn't exist.";

		for (auto& [obj, firm] : stock_) {
			if (f == firm)
				cout << obj << " ";
		}
		cout << endl;
	}
};

class Interp {
	ChineseWall c;
	ifstream iEnv;

	void Start(stringstream& ss) {
		c.Start();
		cout << "History was cleared." << endl;
	}

	void Write(stringstream& ss) {
		int s, o;
		ss >> s >> o;
		if (c.Write(s, o))
			cout << "Accepted" << endl;
		else
			cout << "Rejected" << endl;

	}

	void Read(stringstream& ss) {
		int s, o;
		ss >> s >> o;
		if (c.Read(s, o))
			cout << "Accepted" << endl;
		else
			cout << "Rejected" << endl;
	}

	void ReportSubj(stringstream& ss) {
		int s;
		ss >> s;
		c.ReportSubj(s);
	}

	void ReportObj(stringstream& ss) {
		int o;
		ss >> o;
		c.ReportObj(o);
	}

	void BriefCase(stringstream& ss) {
		int f;
		ss >> f;
		c.BriefCase(f);
	}

public:
	Interp(string env) : iEnv(env) {
		int n, m, f;
		iEnv >> n >> m >> f;

		map<int, int> stock;
		for (int i = 0; i < f; i++) {
			string line;
			getline(iEnv, line);
			vector<string> splitted_line = Split(line, IsSpace);
			for (auto s : splitted_line) {
				stock[stoi(s)] = i;
			}
		}

		int fill = 0;
		int last_col_class = 0;
		map<int, int> col;
		while (fill < f) {
			string line;
			getline(iEnv, line);
			vector<string> splitted_line = Split(line, IsSpace);
			fill += splitted_line.size();
			
			for (auto s : splitted_line)
				col[stoi(s)] = last_col_class;
			
			last_col_class++;
		}

		c = ChineseWall(n, stock, col);
	}

	void ParseCmd(stringstream& ss) {
		using cmd_impl = void (Interp::*)(stringstream&);
		map<string, cmd_impl> cmd_func;
		cmd_func.insert({ "start", &Interp::Start });
		cmd_func.insert({ "write", &Interp::Write });
		cmd_func.insert({ "read", &Interp::Read });
		cmd_func.insert({ "report_subj", &Interp::ReportSubj });
		cmd_func.insert({ "report_obj", &Interp::ReportObj });
		cmd_func.insert({ "brief_case", &Interp::BriefCase });

		string cmd;
		ss >> cmd;
		tolower(cmd);
		if (cmd_func.find(cmd) != cmd_func.end())
			(this->*cmd_func[cmd])(ss);
	}
};

int main() {
	string env_file = "environ";
	Interp inp(env_file);

	string line;
	while (getline(cin, line)) {
		stringstream ss;
		ss << line;
		try {
			inp.ParseCmd(ss);
			cout << endl;
		}
		catch (const string& s) {
			cout << "Error: " << s << endl;
			cout << "Please try again." << endl << endl;
		}
	}
}