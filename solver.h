#ifndef FD_SOLVER_H
#define FD_SOLVER_H

# include <vector>
# include <set>
# include <string>
# include <bitset>
# include <map>
class Solver {
private:
    std::vector<bool> uniqueAttr;
    std::map<std::bitset<32>, bool> visited;
    std::map<std::bitset<32>, bool> dependency;
    std::map<std::bitset<32>, bool> minimal_dependency;
    std::map<std::bitset<32>, bool> candidate_minimal_dependency;
    std::map<std::bitset<32>, bool> non_dependency;
    std::map<std::bitset<32>, bool> maximal_non_dependency;
    std::map<std::bitset<32>, bool> candidate_maximal_non_dependency;

    bool isUnique(size_t attr);

    std::set<std::bitset<32>> findLHSs(int RHS);
    std::set<std::bitset<32>> generateNextSeeds();

    bool isVisited(std::bitset<32> LHS);
    bool isCandidate(std::bitset<32> LHS);
    bool isDependency(std::bitset<32> LHS);
    bool isNonDependency(std::bitset<32> LHS);
    bool isMinimal(std::bitset<32> LHS);
    bool isMaximal(std::bitset<32> LHS);

    void inferCategory(std::bitset<32> LHS);
    void computePartitions(std::bitset<32> LHS, int RHS);

public:
    std::vector<std::vector<std::string>> data;
    std::string filename;
    int col_length;

    Solver();
    explicit Solver(std::string& filename);
    ~Solver() = default;

    bool load_data(std::string filename);
    int get_size();
    bool print_result(std::string filename);

    std::set<std::bitset<32>> result;

    void solve();

};

// tests
extern void load_test();
#endif //FD_SOLVER_H
