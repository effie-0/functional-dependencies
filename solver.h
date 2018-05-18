#ifndef FD_SOLVER_H
#define FD_SOLVER_H

# include <vector>
# include "base.h"
class Solver {
    std::vector<line> data;
    std::string filename;

public:
    Solver();
    explicit Solver(std::string filename);
    ~Solver() = default;

    bool load_data(std::string filename);
    int get_size();
    line get_data(int i);
};

extern void solver_test();
#endif //FD_SOLVER_H
