# include "solver.h"
# include <iostream>
using namespace std;

void solver_test() {
    Solver solver1;
    cout << "solver1.data.size = " << solver1.get_size() << endl;
    line a = solver1.get_data(0);
    int i;
    for (i = 1; i < 16; i++) {
        value result = a[i];
        if (result.isStr) {
            cout << i << ": " << result.str << endl;
        }
        else {
            cout << i << ": " << result.num << endl;
        }
    }

    Solver solver2("./data/data.txt");
    cout << "solver2.data.size = " << solver2.get_size() << endl;
    line b = solver2.get_data(101);
    for (i = 1; i < 16; i++) {
        value result = b[i];
        if (result.isStr) {
            cout << i << ": " << result.str << endl;
        }
        else {
            cout << i << ": " << result.num << endl;
        }
    }
}