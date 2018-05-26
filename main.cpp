// Functional Dependencies problem
// author: Zhao Shangming, Yang Bifei
// date: 2018.5.15

# include <iostream>
# include "solver.h"
# include <ctime>

int main() {
    std::string ifile("../data/data.txt");
    std::string ofile("../data/result.txt");

    std::time_t start, end;
    start = clock();
    Solver solver1(ifile);
    solver1.solve();
    solver1.print_result(ofile);
    end = clock();

    double interval = end - start;
    std::cout << interval/CLOCKS_PER_SEC << "s" << std::endl;

    return 0;
}
