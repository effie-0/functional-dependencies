#ifndef FD_SOLVER_H
#define FD_SOLVER_H

# include <vector>
# include <set>
# include <string>
# include <bitset>
# include <map>
# include <stack>

struct compare {
    bool operator() (const std::bitset<32>& b1, const std::bitset<32>& b2) const {
        auto t1 = b1.to_ulong();
        auto t2 = b2.to_ulong();
        return t1 < t2;
    }
};

class Solver {
private:
    std::vector<bool> uniqueAttr;
    std::map<std::bitset<32>, bool, compare> visited;
    std::map<std::bitset<32>, bool, compare> dependency;
    std::map<std::bitset<32>, bool, compare> minimal_dependency;
    std::map<std::bitset<32>, bool, compare> candidate_minimal_dependency;
    std::map<std::bitset<32>, bool, compare> non_dependency;
    std::map<std::bitset<32>, bool, compare> maximal_non_dependency;
    std::map<std::bitset<32>, bool, compare> candidate_maximal_non_dependency;
    std::stack<std::bitset<32>> trace;
    std::map<std::bitset<32>, bool, compare> partition_check;
    std::map<std::bitset<32>, std::set<std::set<int>>, compare> probe_table;

    bool isUnique(size_t attr);
    void clear_records();

    std::set<std::bitset<32>, compare> findLHSs(int RHS);
    std::set<std::bitset<32>, compare> generateNextSeeds(int RHS, std::set<std::bitset<32>, compare> &minDeps, std::set<std::bitset<32>, compare> &maxNonDeps);
    std::bitset<32> pickNextNode(std::bitset<32> node, int RHS, std::set<std::bitset<32>, compare> &minDeps, std::set<std::bitset<32>, compare> &maxNonDeps);
    std::set<std::bitset<32>, compare> getPrunedSubset(std::bitset<32> LHS, int RHS);
    std::set<std::bitset<32>, compare> getPrunedSuperset(std::bitset<32> LHS, int RHS);
    std::set<std::bitset<32>, compare> minimize(std::set<std::bitset<32>, compare> &newSeeds);

    bool isVisited(std::bitset<32> LHS);
    bool isCandidate(std::bitset<32> LHS);
    bool isDependency(std::bitset<32> LHS);
    bool isNonDependency(std::bitset<32> LHS);
    bool isMinimal(std::bitset<32> LHS);
    bool isMaximal(std::bitset<32> LHS);

    void inferCategory(std::bitset<32> LHS);
    void computePartitions(std::bitset<32> LHS, int RHS);

    void strippedProduct(std::bitset<32> LHS, std::bitset<32> RHS);
    void getPartition(std::bitset<32> node);

public:
    std::vector<std::vector<std::string>> data;
    std::string filename;
    int col_length;

    Solver();
    explicit Solver(std::string& filename);
    ~Solver() = default;

    bool load_data(std::string filename);
    unsigned long get_size();
    bool print_result(std::string filename);

    std::set<std::bitset<32>, compare> result;
    void solve();

};

// tests
extern void load_test();
#endif //FD_SOLVER_H
