// Functional Dependencies problem
// author: Zhao Shangming, Yang Bifei
// date: 2018.5.15

# include <iostream>
# include "base.h"
# include "solver.h"
# include "utils.h"

int main() {
//    std::cout << "Hello, World!" << std::endl;
//    base_test();
//    solver_test();
    std::string fname = "../data/data.txt";
    Solver solver(fname);
    // debug
//    int i = 0, j = 0, rSize = solver.data.size();
//    for (i = 0;i < rSize;i++) {
//        line l = solver.data[i];
//        for (j = 0;j < 15;j++) {
//            if (l[j].isStr) {
//                std::cout << l[j].str;
//            } else {
//                std::cout << l[j].num;
//            }
//            std::cout << " ";
//        }
//    }
    std::set<uint32_t> result = mainLoop(solver.data);
    std::set<uint32_t>::iterator it, itend = result.end();
    for (it = result.begin(); it != itend; it++) {
        std::cout << *it << "\n";
    }
    return 0;
}
