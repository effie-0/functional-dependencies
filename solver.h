#ifndef FD_SOLVER_H
#define FD_SOLVER_H

# include <vector>
# include "base.h"
class Solver {
    std::vector<line> data;

public:
    Solver();
    ~Solver() = default;

    bool load_data(std::string filename);
};

#endif //FD_SOLVER_H
