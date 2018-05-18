# include "solver.h"
# include <iostream>
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
            int i = 0;
            while(!infile.eof()) {
                char buffer[256];
                infile.getline(buffer, 250);
                if (buffer[0] == '\0' || buffer[0] == '\r' || buffer[0] == '\n')
                    break;
                i++;
                line l(buffer);
                data.push_back(l);
                if (i % 1000 == 0) {
                    cout << "i = " << i << endl;
                }
            }
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
    filename = "/Users/yangbifei/CLionProjects/FD/data/test_data.txt";
    bool succ = load_data(filename);
    if (succ)
        cout << "Solver() succeed" << endl;
}

Solver::Solver(std::string file) {
    filename = file;
    bool succ = load_data(filename);
    if(succ)
        cout << "Solver() succeed" << endl;
}

int Solver::get_size() {
    return (int)data.size();
}

line Solver::get_data(int i) {
    return data[i];
}