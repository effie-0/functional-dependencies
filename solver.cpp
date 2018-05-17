# include "solver.h"
# include <iostream>
# include <fstream>
# include <exception>
using namespace std;

bool Solver::load_data(string filename) {
    ifstream infile;
    try {
        infile.open(filename);
        if (!infile.is_open()) {
            cerr << "failure while load data" << endl;
            exit(1);
        }
        else {
            while(!infile.eof()) {
                char buffer[200];
                infile.getline(buffer, 198);
                line l(buffer);
            }
        }
    }
    catch (exception& e) {
        cerr << e.what() << endl;
        cerr << "failure while load data" << endl;
    }

}