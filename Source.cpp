#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <map>


using namespace std;

vector<string> split(string str, const string& delimiter) {
	size_t pos = 0;
	string token;
	vector<string> tokens;
	vector<string> buffer;
	while ((pos = str.find(delimiter)) != string::npos) {
		tokens.push_back(str.substr(0, pos));
		str.erase(0, pos + delimiter.length());
	}
	if (str.find(delimiter) == string::npos)
		tokens.push_back(str);
	for (int t = 0; t < tokens.size(); t++) {
		if (tokens[t] == "") {
			tokens.erase(tokens.begin() + t);
		}
		if (delimiter == "<") {
			if (tokens[t].find(" ") != string::npos) {
				buffer = split(tokens[t], " ");
				tokens.erase(tokens.begin() + t);
				tokens.insert(tokens.begin() + t, buffer.begin(), buffer.end());
			}
			if (tokens[t].back() == '>')
				tokens[t] = "<" + tokens[t];
		}
	}		
	
	return tokens;
}

vector<vector<string>> split_rule(string str) {

	vector<string> tokens;
	vector<string> elements;
	vector<vector<string>> right_side;

	tokens = split(str, "::=");
	right_side.push_back(vector<string>());
	right_side.back().push_back(tokens[0]);
	tokens = split(tokens[1], " | ");

	for (int j = 0; j < tokens.size(); j++) {
		if (tokens[j].find("<") != string::npos && tokens[j].find(">") != string::npos)
			elements = split(tokens[j], "<");
		else
			elements.push_back(tokens[j]);
		right_side.push_back(elements);
		elements.clear();
	}	

	return right_side;
}
void fix_left_recursion (vector<vector<string>> &right_side, string &rule) {
	string subname = "";
	for (int j = 1; j < right_side.size(); j++) {
		if (right_side[j].front() == right_side.front().front()) {
			if (rule == "") {
				subname = right_side.front().front();
				subname.erase(subname.size() - 1);
				subname += "'>";
			}
			right_side[j].push_back(subname);
			right_side[j].erase(right_side[j].begin());
			for (string token : right_side[j]) {
				if (token.front() != '<') {
					if (rule == "") {
						rule += token;
						rule += " ";
					}
					else {
						if (rule.back() == ' ') {
							if (token != right_side[j].back())
							{
								rule += token;
								rule += " ";
							}
							else
								rule += token;
						}
						else {
							if (token != right_side[j].back())
							{
								rule += " ";
								rule += token;
								rule += " ";
							}
							else {
								rule += " ";
								rule += token;
							}

						}
					}

				}
				else
					rule += token;
			}
			rule += " | ";
			right_side.erase(right_side.begin() + j);
			j--;
		}
	}

	if (rule != "") {
		rule += "empty";
		for (auto& alternate : right_side) {
			alternate.push_back(subname);
		}
		rule.insert(0, subname + "::=");
	}

}

vector<string> fix_2_rule(vector<vector<string>> &right_side, string &rule) {

	vector<string> new_rules;
	for (int j = 0; j < right_side.size(); j++) {
		for (int z = j + 1; z < right_side.size(); z++) {
			if (right_side[j].front() == right_side[z].front()) {
				if (right_side[z] == right_side.back() && rule != "") {
					break;
				}
				int p = 0;
				if (rule == "") {
					right_side.push_back(vector<string>());
					while (right_side[j][p] == right_side[z].front()) {
						{
							right_side.back().push_back(right_side[j][p]);
						}

						right_side[z].erase(right_side[z].begin());
						if (right_side[z].size() == 0 || p == right_side[j].size() - 1)
							break;
						p++;
					}
				}
				else {
					vector<string> buff;
					if (right_side[z].size() >= right_side.back().size()) {
						buff.insert(buff.begin(), right_side[z].begin(), right_side[z].begin() + right_side.back().size());
					}
					if (buff == right_side.back()) {
						right_side[z].erase(right_side[z].begin(), right_side[z].begin() + buff.size());
					}
					else
						continue;
				}

				if (right_side[z].size() == 0 && rule.find(" | empty") == string::npos)
					rule += "empty";
				else {
					for (string token : right_side[z]) {
						if (token.front() != '<') {
							if (rule == "") {
								rule += token;
								rule += " ";
							}
							else {
								if (rule.back() == ' ') {
									if (token != right_side[z].back())
									{
										rule += token;
										rule += " ";
									}
									else
										rule += token;
								}
								else {
									if (token != right_side[z].back())
									{
										rule += " ";
										rule += token;
										rule += " ";
									}
									else {
										rule += " ";
										rule += token;
									}

								}
							}

						}
						else
							rule += token;
					}
				}
				rule += " | ";
				right_side.erase(right_side.begin() + z);
				z--;
			}
		}
		if (rule != "") {
			right_side[j].erase(right_side[j].begin(), right_side[j].begin() + right_side.back().size());
			if (right_side[j].size() == 0 && rule.find(" | empty") == string::npos)
				rule += "empty";
			else {
				for (string token : right_side[j]) {
					if (token.front() != '<') {
						if (token == right_side[j].front()) {
							rule += token;
							rule += " ";
						}
						else {
							if (rule.back() == ' ') {
								if (token != right_side[j].back())
								{
									rule += token;
									rule += " ";
								}
								else
									rule += token;
							}
							else {
								if (token != right_side[j].back())
								{
									rule += " ";
									rule += token;
									rule += " ";
								}
								else {
									rule += " ";
									rule += token;
								}

							}
						}

					}
					else
						rule += token;
				}
			}
			if (new_rules.empty()) 
				right_side.back().push_back(right_side.front().front());
			else
				right_side.back().push_back(split(new_rules.back(),"::=")[0]);
			right_side.back().back().erase(right_side.back().back().end() - 1);
			right_side.back().back() += "'>";
			right_side.erase(right_side.begin() + j);
			rule.insert(0, right_side.back().back() + "::=");
			new_rules.push_back(rule);
			rule.clear();
			j--;
		}

	}

	return new_rules;
}

int find(vector<pair<string, string>>& vect, const string& symbol) {

	for (int i = 0; i < vect.size(); i++) {
		if (vect[i].first == symbol)
			return i;
	}
	return -1;
}


int find_vector(vector<vector<pair<string, string>>> & table, string & non_term) {

	for (int i = 0; i < table.size(); i++) {
		if (table[i].begin()->first == non_term)
			return i;
	}

	return -1;

	
}

vector<vector<pair<string, string>>> firsts(vector<vector<vector<string>>>& grammar) {
	vector<vector<pair<string, string>>> first;
	vector<vector<vector<string>>> empty_case_array;
	

	for (int i = grammar.size() - 1; i >= 0; i--) {
		first.push_back(vector < pair<string, string>>());
		first.back().push_back(pair<string, string>(grammar[i].front().front(), ""));
		for (int j = 1; j < grammar[i].size(); j++) {
			if (grammar[i][j].size() == 1 && grammar[i][j][0].front() != '<') {
				first.back().push_back(pair<string, string>(grammar[i][j][0], "&"));
				continue;
			}
			if (grammar[i][j].size() != 1 && grammar[i][j][0].front() != '<')
				first.back().push_back(pair<string, string>(grammar[i][j][0], grammar[i][j][1]));
				
			else {
				int pos = 0;
				int ind = 0;
				do {
					bool empty_flag = false;
					if (grammar[i][j][pos].back() == '>') {
						ind = find_vector(first, grammar[i][j][pos]);
						if (ind!=-1) {
							for (int k = 1; k < first[ind].size(); k++) {
								if (first[ind][k].first == "empty")
									empty_flag = true;
								if (find(first.back(), first[ind][k].first) == -1)
									first.back().push_back(pair<string, string>(first[ind][k].first, grammar[i][j][pos]));
							}
						}
						else {
							if (empty_case_array.empty() || empty_case_array.back().front().front() != grammar[i].front().front()) {
								empty_case_array.push_back(vector<vector<string>>());
								empty_case_array.back().push_back(vector<string>());
								empty_case_array.back().front().push_back(first.back().front().first);
							}	
							empty_case_array.back().push_back(grammar[i][j]);
						}
						if (empty_flag) {
							pos++;
						}
						else
							break;
					}
					else {
						if (pos + 1 != grammar[i][j].size())
							first.back().push_back(pair<string, string>(grammar[i][j][pos], grammar[i][j][pos+1]));
						else
							first.back().push_back(pair<string, string>(grammar[i][j][pos], "&"));
						break;
					}

				} while (pos != grammar[i][j].size());

			}

		}
	}

	for (auto& kek : first) {
		for (auto& elem : kek) {
			if (elem == kek.front()) {
				cout << elem.first;
				cout << "       ";
			}
			else {
				cout << elem.first;
				cout << ";";
			}

		}
		cout << endl;
	}


	cout << "==============================================================================================================================" << endl;;


	for (auto& kek : empty_case_array) {
		for (auto& elem : kek) {
			for (int t = 0; t < elem.size(); t++) {
				if (t==0) {
					cout << elem[t];
					cout << "======";
				}
				else {
					cout << elem[t];
					cout << ";";
				}

			}
			
		}
	cout << endl;		
	}


	for (auto& vect : empty_case_array) {
		for (int i = 1; i < vect.size(); i++) {
			int pos = 0;
			int ind = 0;
			int current_ind;
			bool empty_flag = false;
			do {
				ind = find_vector(first, vect[i][pos]);
				vector<pair<string, string>> current_first;
				current_ind  = find_vector(first, vect[0].front());
				if (ind != -1 && current_ind != -1) {
					if (first[ind].size() <= 1) {
						for(auto& elem : vect[i])
						first[current_ind].push_back(pair<string, string>(vect[i][pos], "additional"));
					}
					for (int j = 1; j < first[ind].size(); j++) {
						if (first[ind][j].first == "empty")
							empty_flag = true;
						if (find(first[current_ind], first[ind][j].first) == -1)
							first[current_ind].push_back(pair<string, string>(first[ind][j].first, first[ind].begin()->first));
					}
				}
				else {
					first[current_ind].push_back(pair<string, string>(vect[i][pos], "additional"));
					empty_flag = true;
				}
				
				if (empty_flag)
					pos++;
				else
					break;
			} while (pos != vect[i].size());
		}
	
		
	}

	return first;

}

vector<vector<pair<string, string>>> follows(vector<vector<vector<string>>>& grammar, vector<vector<pair<string, string>>>& firsts) {
	vector<vector<pair<string, string>>> follow;
	follow.push_back(vector <pair<string, string>>());
	follow[0].push_back(pair<string, string>(grammar[0][0][0], ""));
	follow[0].push_back(pair<string, string>("$", ""));

	for (auto& elem : grammar) {
		for (int i = 1; i < elem.size(); i++) {
			for (int j = 0; j < elem[i].size(); j++) {
				if (elem[i][j].back() == '>' && elem[i][j].front()=='<') {
					int pos = 0;
					for (int y = follow.size() - 1; y >= 0; y--) {
						if (follow[y].begin()->first == elem[i][j]) {
							pos = y;
							break;
						}
						if ((y == 0) && follow[y].begin()->first != elem[i][j]) {
							follow.push_back(vector <pair<string, string>>());
							follow.back().push_back(pair<string, string>(elem[i][j], ""));
							pos = follow.size() - 1;
						}
					}
					if (j != elem[i].size() - 1) {
						if (find(follow[pos], elem[i][j + 1]) == -1)
							follow[pos].push_back(pair<string, string>(elem[i][j + 1], ""));
					}
					else
						follow[pos].push_back(pair<string, string>(elem[0][0], "follow"));
					}
					

				}
				

			}
			

		}


	for (auto& kek : follow) {
		for (auto& elem : kek) {
			if (elem == kek.front()) {
				cout << elem.first;
				cout << "       ";
			}
			else {
				cout << elem.first;
				cout << ";";
			}
			
		}
		cout << endl;
	}

	for (auto& vect : follow) {
		int ind = 0;
		for (int i = 1; i < vect.size(); i++) {
			if (vect[i].second == "follow") {
				ind = find_vector(follow, vect[i].first);
				for (int j = 1; j < follow[ind].size(); j++) {
					if (find(vect, follow[ind][j].first) == -1)
						vect.push_back(follow[ind][j]);
				}
				vect.erase(vect.begin() + i);
				i--;
			}
		}
	}


	for (auto& vect : follow) {
		for (int i = 1; i < vect.size(); i++) {
			if (vect[i].first.front() == '<') {
				for (auto& first : firsts) {
					if (first.begin()->first == vect[i].first) {
						for (int k = 1; k < first.size(); k++) {
							if (find(vect, first[k].first) == -1 && first[k].first != "empty") {
								vect.push_back(pair<string, string>(first[k].first, ""));

							}
						}
						if (find(first, "empty") != -1) {
							for (auto& fol : follow) {
								if (fol[0].first == vect[i].first)
								{
									for (int j = 1; j < fol.size(); j++) {
										if (find(vect, fol[j].first) == -1)
											vect.push_back(fol[j]);
									}
									break;
								}
							}

						}
						vect.erase(vect.begin() + i);
						i--;
						break;
					}
				}
			}
		}
	}
	return follow;

}

//vector<tree*> build_table(vector<vector<string>>& grammar) {
//	vector<tree*> syntax_tree;
//	struct tree* current = new tree;
//
//	for (int i = 0; i < grammar.size(); i++) {
//
//		current = init_tree(grammar[i].front());
//		for (int j = 1; j < grammar[i].size(); j++) {
//			current->children.push_back(grammar[i][j]);
//		}
//		syntax_tree.push_back(current);
//	}
//
//	return syntax_tree;
//
//}

int main() {
	string line;
	ifstream myfile;
	string path;
	vector<string> grammar;
	vector<string> additional_grammar;
	cout << "Enter grammar description file path: ";
	while (grammar.size() == 0) {
		cin >> path;
		myfile.open(path);
		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				if (line.front() == '=') {
					while (getline(myfile, line)) {
						additional_grammar.push_back(line);
					}
					myfile.close();
					break;
				}
			
				if (line.front() != '<') {
					cout << "File does not contain a grammar description. Try again: ";
					grammar.clear();
					break;
				}

				grammar.push_back(line);
			}
			myfile.close();
		}

		else cout << "File not found. Try again: ";
	}

	vector<vector<vector<string>>> gramm;
	vector<string> tokens;
	ofstream fout("output.txt");
	if (fout.is_open()) {
		fout << "GRAMMAR" << endl;
		for (int i = 0; i < grammar.size(); i++) {

			fout << grammar[i];
			fout << endl;

			gramm.push_back(vector<vector<string>>());
			
			vector<vector<string>> right_side = split_rule(grammar[i]);

			string rule = "";
			fix_left_recursion(right_side, rule);

			if (rule!="") {
				grammar.insert(grammar.begin() + gramm.size() - 1, rule);

				fout << endl;
				fout << "Production " << right_side.front().front() << " has left recursion. Fixed production:" << endl;
				fout << right_side.front().front() << "::=";
				for (int z = 1; z < right_side.size(); z++) {
					for (int k = 0; k < right_side[z].size(); k++) {
						fout << right_side[z][k];
					}
					if (z != right_side.size() - 1)
						fout << " | ";
				}
				fout << endl;
			}
			rule.clear();


			vector<string> subrules = fix_2_rule(right_side, rule);
			gramm[i].insert(gramm[i].begin(), right_side.begin(), right_side.end());
			
			if (!subrules.empty()) {

				grammar.insert(grammar.begin() + gramm.size(), subrules.begin(), subrules.end());

				fout << endl;
				fout << "Production " << gramm[i].front().front() << " does not fulfill I grammatical rule. Applying left-factoring and rewriting production:" << endl;
				fout << endl;
				fout << gramm[i].front().front() + "::=";
				for (int z = 1; z < right_side.size(); z++) {
					for (int k = 0; k < right_side[z].size(); k++) {
						fout << right_side[z][k];
					}
					if (z != right_side.size() - 1)
						fout << " | ";
				}
				fout << endl;
			}
			
							
		}

		vector<vector<vector<string>>> add_gramm;
		for (int i = 0; i < additional_grammar.size(); i++) {

			fout << additional_grammar[i];
			fout << endl;

			add_gramm.push_back(vector<vector<string>>());

			vector<vector<string>> right_side = split_rule(additional_grammar[i]);

			string rule = "";
			fix_left_recursion(right_side, rule);

			if (rule != "") {
				additional_grammar.insert(additional_grammar.begin() + add_gramm.size() - 1, rule);

				fout << "==============================================================================================================================" << endl;

				fout << "Production " << right_side.front().front() << " has left recursion. Fixed production:" << endl;
				fout << right_side.front().front() << "::=";
				for (int z = 1; z < right_side.size(); z++) {
					for (int k = 0; k < right_side[z].size(); k++) {
						fout << right_side[z][k];
					}
					if (z != right_side.size() - 1)
						fout << " | ";
				}
				fout << endl;
			}
			rule.clear();


			vector<string> subrules = fix_2_rule(right_side, rule);
			add_gramm[i].insert(add_gramm[i].begin(), right_side.begin(), right_side.end());

			if (!subrules.empty()) {

				additional_grammar.insert(additional_grammar.begin() + add_gramm.size(), subrules.begin(), subrules.end());

				fout << "========================================================================================================================================" << endl;
				fout << "Production " << add_gramm[i].front().front() << " does not fulfill I grammatical rule. Applying left-factoring and rewriting production:" << endl;
				fout << add_gramm[i].front().front() + "::=";
				for (int z = 1; z < right_side.size(); z++) {
					for (int k = 0; k < right_side[z].size(); k++) {
						fout << right_side[z][k];
					}
					if (z != right_side.size() - 1)
						fout << " | ";
				}
				fout << endl;
			}


		}

		fout << "==============================================================================================================================" << endl;

	/*	for (auto& gram : gramm) {
			fout << gram[0].front() << "::=";
			for (int p = 1; p < gram.size(); p++) {
				for (auto& pipka : gram[p]) {
					fout << pipka;
				}
				if(gram[p]!=gram.back())
					fout << " | ";
			}
			fout << endl;
		}*/

		
		vector<vector<vector<string>>> grammar_union;

		grammar_union.insert(grammar_union.begin(), gramm.begin(), gramm.end());

		grammar_union.insert(grammar_union.end(), add_gramm.begin(), add_gramm.end());

		//vector<vector<pair<string, string>>> add_first = firsts(add_gramm);

		//vector<vector<pair<string, string>>> add_follow = follows(add_gramm, add_first);

		vector<vector<pair<string, string>>> first = firsts(grammar_union);

		vector<vector<pair<string, string>>> follow = follows(grammar_union, first);

		fout << "==============================================================================================================================" << endl;
		fout << "FIRSTS" << endl;
		for (int i = 0; i < first.size(); i++)
		{
			for (auto& it : first[i]) {
				fout << "(" << it.first << "," << it.second << ")";
			}
			fout << endl;
		}
		fout << "==============================================================================================================================" << endl;
		fout << "FOLLOWS" << endl;
		for (int i = 0; i < follow.size(); i++)
		{
			for (auto& it : follow[i]) {
				fout << "(" << it.first << "," << it.second << ")";
			}
			fout << endl;
		}

		for (auto& production : gramm) {
			for (auto& altenative : production) {
				int firsts_ind = find_vector(first, altenative[0]);
				int follow_ind = find_vector(follow, altenative[0]);

				if (firsts_ind != -1 && follow_ind != -1) {
					for (int i = 1; i < first[firsts_ind].size(); i++) {
						if (find(follow[follow_ind], first[firsts_ind][i].first) != -1) {
							if (find(first[firsts_ind], "empty") != -1) {
								fout << "Error. Production " << altenative[0] << " does not fulfull gramatical rule 2.";
								fout.close();
								return 0;
							}
						}
					}
				}
				
			}
		}			

		fout << "==============================================================================================================================" << endl;

		path.clear();
		vector<string> text_one;
		vector<string> text_two;
		cout << "Enter  file 1 path: ";
		while (grammar.size() == 0) {
			cin >> path;
			myfile.open(path);
			if (myfile.is_open())
			{
				while (getline(myfile, line)) {
					text_one.push_back(line);
				}

				myfile.close();
			}

			else cout << "File not found. Try again: ";
		}

		cout << "Enter  file 2 path: ";
		while (grammar.size() == 0) {
			cin >> path;
			myfile.open(path);
			if (myfile.is_open())
			{
				while (getline(myfile, line)) {
					text_two.push_back(line);
				}

				myfile.close();
			}

			else cout << "File not found. Try again: ";
		}






		return 0;
	}
}