# include "solver.h"
# include <iostream>
# include <sstream>
# include <fstream>
# include <exception>
# include <algorithm>
using namespace std;

bool Solver::load_data(string filename) {
    ifstream infile;
    try {
        infile.open(filename);
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
}

Solver::Solver(std::string& file) {
    col_length = 0;
    filename = file;
    bool succ = load_data(filename);
}

unsigned long Solver::get_size() {
    return data.size();
}

bool Solver::print_result(std::string filename) {
    ofstream outfile;
    try {
        outfile.open(filename, ios::trunc);
        if (!outfile.good()) {
            cerr << "failure while print data" << endl;
            return false;
        }
        else {
            size_t i;
            set<bitset<32>, compare> new_result;
            for (auto &it : result) {
                bitset<32> new_value;
                for (i = 0; i < col_length; i++) {
                    if (it.test(16 + i)) {
                        new_value.set(31 - i);
                    }
                    if (it.test(i)) {
                        new_value.set(16 - i);
                    }
                }
                new_result.insert(new_value);
            }

            bool isChecked;
            auto it = new_result.rbegin();
            auto rend = new_result.rend();
            for (; it != rend; it++) {
                isChecked = false;
                for (i = 0; i < col_length; i++) {
                    if (it->test(31 - i)) {
                        if (!isChecked)
                            isChecked = true;
                        outfile << (i+1) << " ";
                    }
                }
                if (isChecked) {
                    outfile << "-> ";
                    for (i = 0; i < col_length; i++) {
                        if (it->test(16 - i)) {
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

// this function not only serves to judge whether the attribute is unique,
// but alse helps to build the probe_table
bool Solver::isUnique(size_t attr) {
    int i = 0, rSize = get_size();
    auto v = data[0][attr];
    map<string, int> m; // map from value to line index
    map<string, int>::iterator it;
    map<string, set<int>> dict;
    bool ans = true;

    for (i = 0; i < rSize; i++) {
        v = data[i][attr];
        it = m.find(v);
        if (it != m.end()) {
            if (!(data[i] == data[it->second])) {
                dict[v].insert(i);
                ans = false;
            }
        } else {
            m[v] = i;
            dict[v].insert(i);
        }
    }
    bitset<32> index;
    index.set(16 + attr);
    partition_check[index] = true;
    set<set<int>> table;
    for (auto sit : dict) {
        table.insert(sit.second);
    }
    probe_table[index] = table;

    return ans;
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
        clear_records();
        set<bitset<32>, compare> LHS = findLHSs((int)i);
        for(auto it: LHS) {
            it.set(i);
            result.insert(it);
        }
    }
}

void Solver::clear_records() {
    visited.clear();
    dependency.clear();
    minimal_dependency.clear();
    candidate_minimal_dependency.clear();
    non_dependency.clear();
    maximal_non_dependency.clear();
    candidate_maximal_non_dependency.clear();
    while(!trace.empty()) {
        trace.pop();
    }
}

set<bitset<32>, compare> Solver::findLHSs(int RHS) {
    set<bitset<32>, compare> seeds;
    set<bitset<32>, compare> minDeps;
    set<bitset<32>, compare> maxNonDeps;

    // seeds <- R\{A}
    size_t i;
    for (i = 0; i < col_length; i++) {
        if (i == RHS || uniqueAttr[i])
            continue;
        bitset<32> re;
        re.set(i + 16);
        seeds.insert(re);
    }

    while(!seeds.empty()) {
        for (auto &iter : seeds) {
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
                it = pickNextNode(it, RHS, minDeps, maxNonDeps);
            }
            while(it != 0);
        }
        seeds = generateNextSeeds(RHS, minDeps, maxNonDeps);
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
    bool b1, b2;
    auto it = candidate_minimal_dependency.find(LHS);
    if (it == candidate_minimal_dependency.end()) {
        candidate_minimal_dependency[LHS] = false;
        b1 = false;
    }
    else {
        b1 = it->second;
    }

    auto itr = candidate_maximal_non_dependency.find(LHS);
    if (itr == candidate_maximal_non_dependency.end()) {
        candidate_maximal_non_dependency[LHS] = false;
        b2 = false;
    }
    else {
        b2 = itr->second;
    }

    return b1 || b2;
}

bool Solver::isDependency(std::bitset<32> LHS) {
    bool b1, b2, b3;
    auto it1 = dependency.find(LHS);
    if (it1 == dependency.end()) {
        dependency[LHS] = false;
        b1 = false;
    }
    else {
        b1 = it1->second;
    }

    auto it2 = candidate_minimal_dependency.find(LHS);
    if (it2 == candidate_minimal_dependency.end()) {
        candidate_minimal_dependency[LHS] = false;
        b2 = false;
    }
    else {
        b2 = it2->second;
    }

    auto it3 = minimal_dependency.find(LHS);
    if (it3 == minimal_dependency.end()) {
        minimal_dependency[LHS] = false;
        b3 = false;
    }
    else {
        b3 = it3->second;
    }

    return b1 || b2 || b3;
}

bool Solver::isNonDependency(std::bitset<32> LHS) {
    bool b1, b2, b3;
    auto it1 = non_dependency.find(LHS);
    if (it1 == non_dependency.end()) {
        non_dependency[LHS] = false;
        b1 = false;
    }
    else {
        b1 = it1->second;
    }

    auto it2 = candidate_maximal_non_dependency.find(LHS);
    if (it2 == candidate_maximal_non_dependency.end()) {
        candidate_maximal_non_dependency[LHS] = false;
        b2 = false;
    }
    else {
        b2 = it2->second;
    }

    auto it3 = maximal_non_dependency.find(LHS);
    if (it3 == maximal_non_dependency.end()) {
        maximal_non_dependency[LHS] = false;
        b3 = false;
    }
    else {
        b3 = it3->second;
    }

    return b1 || b2 || b3;
}

bool Solver::isMinimal(bitset<32> LHS) {
    size_t i;
    bitset<32> temp;
    for (i = 0; i < col_length; i++) {
        if (LHS.test(16 + i)) {
            temp = LHS;
            temp.reset(i + 16);
            if (isVisited(temp)) {
                if (isDependency(temp)) {
                    candidate_minimal_dependency[LHS] = false;
                    minimal_dependency[LHS] = false;
                    dependency[LHS] = true;
                    return false;
                }
            }
            else {
                return false;
            }
        }
    }
    return true;
}

bool Solver::isMaximal(std::bitset<32> LHS) {
    size_t i;
    bitset<32> temp;
    for (i = 0; i < col_length; i++) {
        if (uniqueAttr[i])
            continue;
        if (!LHS.test(16 + i)) {
            temp = LHS;
            temp.set(16 + i);
            if (isVisited(temp)) {
                if (isNonDependency(temp)) {
                    candidate_maximal_non_dependency[LHS] = false;
                    maximal_non_dependency[LHS] = false;
                    non_dependency[LHS] = true;
                    return false;
                }
            }
            else {
                return false;
            }
        }
    }
    return true;
}

void Solver::inferCategory(std::bitset<32> LHS) {
    visited[LHS] = true;
    for (auto &it : minimal_dependency) {
        if (!it.second)
            continue;
        auto temp = it.first.to_ulong() & LHS.to_ulong();
        if (it.first != LHS && temp == it.first.to_ulong()) {
            dependency[LHS] = true;
        }
    }

    for (auto &it : candidate_minimal_dependency) {
        if (!it.second)
            continue;
        auto temp = it.first.to_ulong() & LHS.to_ulong();
        if (it.first != LHS && temp == it.first.to_ulong()) {
            dependency[LHS] = true;
        }
    }

    for (auto &it : maximal_non_dependency) {
        if (!it.second)
            continue;
        auto temp = it.first.to_ulong() & LHS.to_ulong();
        if (it.first != LHS && temp == LHS.to_ulong()) {
            non_dependency[LHS] = true;
        }
    }

    for (auto &it : candidate_maximal_non_dependency) {
        if (!it.second)
            continue;
        auto temp = it.first.to_ulong() & LHS.to_ulong();
        if (it.first != LHS && temp == LHS.to_ulong()) {
            non_dependency[LHS] = true;
        }
    }
}

bitset<32> Solver::pickNextNode(bitset<32> LHS, int RHS, set<bitset<32>, compare> &minDeps, set<bitset<32>, compare> &maxNonDeps) {
    if (isCandidate(LHS) && isDependency(LHS)) {
        auto S = getPrunedSubset(LHS, RHS);
        if (S.empty()) {
            minDeps.insert(LHS);
            // update dependency types
            candidate_minimal_dependency[LHS] = false;
            minimal_dependency[LHS] = true;
        }
        else {
            bitset<32> nextNode = *(S.begin());
            trace.push(LHS);
            return nextNode;
        }
    }
    else if (isCandidate(LHS) && isNonDependency(LHS)) {
        auto S = getPrunedSuperset(LHS, RHS);
        if (S.empty()) {
            maxNonDeps.insert(LHS);
            // update dependency types
            candidate_maximal_non_dependency[LHS] = false;
            maximal_non_dependency[LHS] = true;
        }
        else {
            bitset<32> nextNode = *(S.begin());
            trace.push(LHS);
            return nextNode;
        }
    }
    if (!trace.empty()) {
        bitset<32> nextNode = trace.top();
        trace.pop();
        return nextNode;
    }

    bitset<32> nullbit(0);
    return nullbit;
}

set<bitset<32>, compare> Solver::getPrunedSubset(bitset<32> LHS, int RHS) {
    size_t i;
    set<bitset<32>, compare> subset;
    subset.clear();
    for (i = 0; i < col_length; i++) {
        if (i == RHS)
            continue;
        if (LHS.test(16 + i)) {
            bitset<32> next = LHS;
            next.reset(16 + i);
            bitset<32> zero;
            if (!isVisited(next) && next != zero) {
                subset.insert(next);
            }
        }
    }

    return subset;
}

set<bitset<32>, compare> Solver::getPrunedSuperset(std::bitset<32> LHS, int RHS) {
    size_t i, j;
    set<bitset<32>, compare> superset;
    superset.clear();
    bool excluded;
    for (i = 0; i < col_length; i++) {
        if (uniqueAttr[i] || i == RHS)
            continue;
        if (!LHS.test(16 + i)) {
            bitset<32> next = LHS;
            next.set(16 + i);
            if (!isVisited(next)) {
                excluded = false;
                for (j = 0; j < col_length; j++) {
                    if (i == j)
                        continue;
                    if (next.test(16 + j)) {
                        bitset<32> temp = next;
                        temp.reset(16 + j);
                        if (isVisited(temp)) {
                            if (isDependency(temp)) {
                                excluded = true;
                                break;
                            }
                        }
                    }
                }
                if (!excluded)
                    superset.insert(next);
            }
        }
    }
    return superset;
}

set<bitset<32>, compare> Solver::generateNextSeeds(int RHS, set<bitset<32>, compare> &minDeps, set<bitset<32>, compare> &maxNonDeps) {
    set<bitset<32>, compare> seeds;
    set<bitset<32>, compare> newSeeds;
    size_t i;

    for (auto &it : maxNonDeps) {
        bitset<32> complement(~it.to_ulong());
        if (seeds.empty()){
            for (i = 0; i < col_length; i++) {
                if (uniqueAttr[i] || i == RHS)
                    continue;
                if (complement.to_ulong() & (1 << (16 + i))) {
                    bitset<32> value;
                    value.set(16 + i);
                    seeds.insert(value);
                }
            }
        }
        else {
            for (auto &its: seeds) {
                for (i = 0; i < col_length; i++) {
                    if (uniqueAttr[i] || i == RHS)
                        continue;
                    if (complement.to_ulong() & (1 << (16 + i))) {
                        bitset<32> value = its;
                        value.set(16 + i);
                        newSeeds.insert(value);
                    }
                }
            }
            auto minimizedNewDeps = minimize(newSeeds);
            seeds.clear();
            seeds = minimizedNewDeps;
            newSeeds.clear();
        }
    }

    for (auto it: minDeps) {
        seeds.erase(it);
    }
    return seeds;
}

set<bitset<32>, compare> Solver::minimize(set<bitset<32>, compare> &newSeeds) {
    map<bitset<32>, bool, compare> keep;
    for (auto &it : newSeeds) {
        keep[it] = true;
    }

    for (auto &it : newSeeds) {
        if (!keep[it])
            continue;
        for (auto &its : newSeeds) {
            if (its == it)
                continue;
            u_long temp1 = its.to_ulong();
            u_long temp2 = it.to_ulong();
            if ((temp1 & temp2) == temp1) {
                // temp2 shows that it is the superset
                keep[it] = false;
            }
            else if ((temp1 & temp2) == temp2) {
                keep[its] = false;
            }
        }
    }

    set<bitset<32>, compare> result;
    for (auto &it : keep) {
        if (it.second) {
            result.insert(it.first);
        }
    }
    return result;
}

void Solver::computePartitions(bitset<32> LHS, int RHS) {
    bitset<32> RHS_t(1 << (16 + RHS));
    bitset<32> LHS_RHS = LHS;
    LHS_RHS.set((size_t)16 + RHS);
    auto it1 = partition_check.find(LHS);
    auto it2 = partition_check.find(RHS_t);
    auto it3 = partition_check.find(LHS_RHS);

    if (it1 != partition_check.end()) {
        if (it3 != partition_check.end()) {
            int size1 = probe_table[it1->first].size();
            int size2 = probe_table[it3->first].size();
            if (size1 == size2) {
                candidate_minimal_dependency[LHS] = true;
                return;
            }
            else {
                candidate_maximal_non_dependency[LHS] = true;
                return;
            }
        }
        else if (it2 != partition_check.end()) {
            strippedProduct(LHS, RHS_t);
            computePartitions(LHS, RHS);
        }
        else if (it2 == partition_check.end()) {
            // this can hardly be reached, for the case for one attr is solved
            getPartition(RHS_t);
            computePartitions(LHS, RHS);
        }
    }
    else {
        getPartition(LHS);
        if (it2 == partition_check.end()) {
            // this can hardly be reached, for the case for one attr is solved
            getPartition(RHS_t);
        }
        computePartitions(LHS, RHS);
    }
}

void Solver::strippedProduct(bitset<32> LHS, bitset<32> RHS) {
    vector<int> T(get_size() + 1);
    vector<bool> inserted(get_size());
    vector<set<int>> S;
    auto &table1 = probe_table[LHS];
    auto &table2 = probe_table[RHS];

    int i;
    int size = (int)get_size();
    for (i = 0; i < size; i++) {
        T[i] = -1;
        inserted[i] = false;
    }

    i = 0;
    for (auto &it1 : table1) {
        for (auto &it2 : it1) {
            T[it2] = i;
        }
        set<int> st;
        S.push_back(st);
        i++;
    }

    i = 0;
    set<set<int>> table;
    for (auto &it1 : table2) {
        for (auto &it2 : it1) {
            if (T[it2] >= 0) {
                S[T[it2]].insert(it2);
            }
        }
        for (auto &it2 : it1) {
            if (S[T[it2]].size() >= 2) {
                table.insert(S[T[it2]]);
                for (auto &it: S[T[it2]]) {
                    inserted[it] = true;
                }
            }
            S[T[it2]].clear();
        }
        i++;
    }

    for (i = 0; i < size; i++) {
        if (!inserted[i]) {
            set<int> s;
            s.insert(i);
            table.insert(s);
        }
    }

    bitset<32> LHS_RHS(((u_long)LHS.to_ulong() | RHS.to_ulong()));
    probe_table[LHS_RHS] = table;
    partition_check[LHS_RHS] = true;
}

void Solver::getPartition(std::bitset<32> node) {
    size_t i;
    int first = -1;
    for (i = 0; i < col_length; i++) {
        if (node.test(16 + i)) {
            if (first == -1) {
                first = (int)i;
            }
            bitset<32> temp = node;
            temp.reset(16 + i);
            auto result = partition_check.find(temp);
            if (result != partition_check.end() && result->second) {
                bitset<32> RHS(1 << (16 + i));
                strippedProduct(temp, RHS);
                return;
            }
        }
    }

    if (first != -1) {
        bitset<32> temp = node;
        temp.reset(16 + first);
        getPartition(temp);
        bitset<32> RHS(1 << (16 + first));
        strippedProduct(temp, RHS);
        return;
    }
}
