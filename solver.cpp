# include "solver.h"
# include <iostream>
# include <sstream>
# include <fstream>
# include <exception>
using namespace std;

bool Solver::load_data(string filename) {
    ifstream infile;
    try {
        infile.open(filename);
        cout << "filename = " << filename << endl;
        if (!infile.good()) {
            cerr << "failure while load data" << endl;
            return false;
        }
        else {
            while(!infile.eof()) {
                char buffer[256];
                infile.getline(buffer, 250);
                if (buffer[0] == '\0' || buffer[0] == '\r' || buffer[0] == '\n')
                    break;

                istringstream iss(buffer);
                vector<string> vec;
                vec.clear();
                char delim = ',';
                int i = 0;
                do {
                    string subs;
                    getline(iss, subs, delim);
                    if (subs.empty())
                        break;
                    vec.push_back(subs);
                    if (!col_length) {
                        i++;
                    }

                }
                while (iss);
                data.push_back(vec);
                if (!col_length) {
                    col_length = i;
                }
            }
            infile.close();
        }
    }
    catch (exception& e) {
        cerr << e.what() << endl;
        cerr << "failure while load data" << endl;
        return false;
    }

    return true;
}

Solver::Solver() {
    col_length = 0;
    filename = "../data/test_data.txt";
    bool succ = load_data(filename);
    if (succ)
        cout << "Solver() succeed" << endl;
}

Solver::Solver(std::string& file) {
    col_length = 0;
    filename = file;
    bool succ = load_data(filename);
    if(succ)
        cout << "Solver(filename) succeed" << endl;
}

int Solver::get_size() {
    return (int)data.size();
}

bool Solver::print_result(std::string filename) {
    ofstream outfile;
    try {
        outfile.open(filename, ios::trunc);
        cout << "saved filename = " << filename << endl;
        if (!outfile.good()) {
            cerr << "failure while print data" << endl;
            return false;
        }
        else {
            size_t i;
            bool isChecked;
            for (auto it : result) {
                isChecked = false;
                for (i = 0; i < col_length; i++) {
                    if (it.test(16+i)) {
                        if (!isChecked)
                            isChecked = true;
                        outfile << (i+1) << " ";
                    }
                }
                if (isChecked) {
                    outfile << "-> ";
                    for (i = 0; i < col_length; i++) {
                        if (it.test(i)) {
                            outfile << (i+1) << " ";
                        }
                    }
                    outfile << endl;
                }
            }
            outfile.close();
        }
    }
    catch(exception& e) {
        cerr << e.what() << endl;
        cerr << "failure while print data" << endl;
        return false;
    }
    return true;
}

bool Solver::isUnique(size_t attr) {
    int i = 0, rSize = get_size();
    auto v = data[0][attr];
    map<string, int> m; // map from value to line index
    map<string, int>::iterator it;
    for (i = 0; i < rSize; i++) {
        v = data[i][attr];
        it = m.find(v);
        if (it != m.end()) {
            if (!(data[i] == data[it->second]))
                return false;
        } else {
            m[v] = i;
        }
    }
    return true;
}

void Solver::solve() {
    size_t i, j;
    for (i = 0; i < col_length; i++) {
        if (isUnique(i)) {
            for (j = 0; j < col_length; j++) {
                if (i == j)
                    continue;
                bitset<32> re;
                re.set(16 + i);
                re.set(j);
                result.insert(re);
            }
            uniqueAttr.push_back(true);
        }
        else {
            uniqueAttr.push_back(false);
        }
    }

    for (i = 0; i < col_length; i++) {
        set<bitset<32>> LHS = findLHSs((int)i);
        for(auto it: LHS) {
            it.set(i);
            result.insert(it);
        }
    }
}

set<bitset<32>> Solver::findLHSs(int RHS) {
    set<bitset<32>> seeds;
    set<bitset<32>> minDeps;
    set<bitset<32>> maxNonDeps;

    // seeds <- R\{A}
    size_t i;
    for (i = 0; i < col_length; i++) {
        if (i == RHS)
            continue;
        bitset<32> re;
        re.set(i + 16);
        seeds.insert(re);
    }

    while(!seeds.empty()) {
        for (auto iter : seeds) {
            auto it = iter;
            do {
                if (isVisited(it)) {
                    if (isCandidate(it)) {
                        if (isDependency(it)) {
                            if (isMinimal(it)) {
                                minDeps.insert(it);
                                // update dependency types
                                candidate_minimal_dependency[it] = false;
                                minimal_dependency[it] = true;
                            }
                        }
                        else {
                            if (isMaximal(it)) {
                                maxNonDeps.insert(it);
                                // update dependency types
                                candidate_maximal_non_dependency[it] = false;
                                maximal_non_dependency[it] = true;
                            }
                        }
                    }
                }
                else {
                    inferCategory(it);
                    if (!isDependency(it) && !isNonDependency(it)) {
                        computePartitions(it, RHS);
                    }
                }
            }
            while(it != 0);
        }
        seeds = generateNextSeeds();
    }

    return minDeps;
}

bool Solver::isVisited(std::bitset<32> LHS) {
    auto it = visited.find(LHS);
    if (it == visited.end()) {
        visited[LHS] = false;
    }
    return visited[LHS];
}

bool Solver::isCandidate(std::bitset<32> LHS) {
    auto it = candidate_minimal_dependency.find(LHS);
    if (it == candidate_minimal_dependency.end()) {
        candidate_minimal_dependency[LHS] = false;
    }

    auto itr = candidate_maximal_non_dependency.find(LHS);
    if (itr == candidate_maximal_non_dependency.end()) {
        candidate_maximal_non_dependency[LHS] = false;
    }

    return it->second || itr->second;
}

bool Solver::isDependency(std::bitset<32> LHS) {
    auto it1 = dependency.find(LHS);
    if (it1 == dependency.end()) {
        dependency[LHS] = false;
    }

    auto it2 = candidate_minimal_dependency.find(LHS);
    if (it2 == candidate_minimal_dependency.end()) {
        candidate_minimal_dependency[LHS] = false;
    }

    auto it3 = minimal_dependency.find(LHS);
    if (it3 == minimal_dependency.end()) {
        minimal_dependency[LHS] = false;
    }

    return it1->second || it2->second || it3->second;
}

bool Solver::isNonDependency(std::bitset<32> LHS) {
    auto it1 = non_dependency.find(LHS);
    if (it1 == non_dependency.end()) {
        non_dependency[LHS] = false;
    }

    auto it2 = candidate_maximal_non_dependency.find(LHS);
    if (it2 == candidate_maximal_non_dependency.end()) {
        candidate_maximal_non_dependency[LHS] = false;
    }

    auto it3 = maximal_non_dependency.find(LHS);
    if (it3 == maximal_non_dependency.end()) {
        maximal_non_dependency[LHS] = false;
    }

    return it1->second || it2->second || it3->second;
}