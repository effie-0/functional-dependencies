# include "solver.h"
# include <iostream>
using namespace std;

void load_test() {
    Solver solver1;
    cout << "solver1.data.size = " << solver1.get_size() << endl;
    auto a = solver1.data[2];
    for (auto& it : a) {
        cout  << it << endl;
    }

    string str("../data/data.txt");
    Solver solver2(str);
    cout << "solver2.data.size = " << solver2.get_size() << endl;
    auto b = solver2.data[0];
    int i;
    for (i = 0; i < solver2.col_length; i++) {
        cout << i << ": " << b[i] << endl;
    }
}