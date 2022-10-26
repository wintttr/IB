#include<iostream>
#include<vector>
#include<set>
#include<map>

using namespace std;

class ChineseWall {
	int n_, m_, f_;
	map<int, int> stock_;			// id объекта -> id фирмы
	map<int, int> collision_;		// id фирмы   -> id класса конфликта
	map<int, set<int>> history;		// История обращения субъекта к объектам
public:

	ChineseWall(int n, int m, int f) : n_(n), m_(m), f_(f) {

	}

	bool IsSubjInHistory(int s) {
		return history.find(s) != history.end();
	}

	void start() {
		history.clear();
	}
	
	bool read(int s, int o) {
		bool f = true;
		if (IsSubjInHistory(s)) {
			const set<int>& hs = history[s];
			for (auto i : hs) {
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
			history[s] = set<int>();

		if (f)
			history[s].insert(o);
		return f;
	}

	bool write(int s, int o) {
		if (!read(s, 0))
			return false;
		
		bool f = true;

		if (IsSubjInHistory(s)) {
			const set<int>& hs = history[s];
			int firm_own_o = stock_[o];
			for (auto i : hs) {
				int firm_own_i = stock_[i];
				if (firm_own_i != firm_own_o && collision_[firm_own_i] == collision_[firm_own_o]) {
					f = false;
					break;
				}
			}
		}
		else
			history[s] = set<int>();
			
		if (f)
			history[s].insert(o);

		return f;
	}

	void report_subj(int s) {

	}

	void report_obj(int o) {

	}
};

int main() {

}