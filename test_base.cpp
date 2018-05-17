# include "base.h"
# include <iostream>
using namespace std;

void test() {
    string id_card("443074522");
    string given_name("Butcher");
    string surname("Quindo");
    string street("Kilian");
    string code("5h3");
    string city("Kosrae");
    string state("FM");
    string college("Community College of the Air Force");
    string address("130 W Maxwell Blvd");
    string phone("334-953-6436");
    line a(1,48071,id_card,given_name,'V',surname,286,street,code,city,state,96944, college,address,phone);
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
}