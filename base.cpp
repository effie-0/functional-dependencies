# include "base.h"
# include <iostream>
# include <cstring>
using namespace std;

line::line() {
    id = -1;
    dis_num = -1;
    type = ' ';
    number = -1;
    zip = -1;
}

line::line(int m_id, int m_dis_num, std::string& m_id_card_num, std::string& m_given_name, char m_type,
           std::string& m_surname, int m_number, std::string& m_street, std::string& m_code, std::string& m_city,
           std::string& m_state, int m_zip, std::string& m_college, std::string& m_addr, std::string& m_phone) {
    id = m_id;
    dis_num = m_dis_num;
    id_card_num = m_id_card_num;
    given_name = m_given_name;
    type = m_type;
    surname = m_surname;
    number = m_number;
    street = m_street;
    code = m_code;
    city = m_city;
    state = m_state;
    zip = m_zip;
    college = m_college;
    addr = m_addr;
    phone = m_phone;
}

line::line(char *str) {
    char* p = nullptr;
    const char* sep = ",";
    p = strtok(str, sep);
    int i = 1;
    while(p != nullptr) {
        p = strtok(nullptr, sep);
    }
}

value line::operator[](int i) {
    struct value result;
    result.isStr = true;
    result.num = -1;
    switch(i) {
        case 1:
            result.isStr = false;
            result.num = id;
            break;
        case 2:
            result.isStr = false;
            result.num = dis_num;
            break;
        case 3:
            result.str = id_card_num;
            break;
        case 4:
            result.str = given_name;
            break;
        case 5:
            char c_type[2];
            c_type[0] = type;
            c_type[1] = '\0';
            result.str = string(c_type);
            break;
        case 6:
            result.str = surname;
            break;
        case 7:
            result.isStr = false;
            result.num = number;
            break;
        case 8:
            result.str = street;
            break;
        case 9:
            result.str = code;
            break;
        case 10:
            result.str = city;
            break;
        case 11:
            result.str = state;
            break;
        case 12:
            result.isStr = false;
            result.num = zip;
            break;
        case 13:
            result.str = college;
            break;
        case 14:
            result.str = addr;
            break;
        case 15:
            result.str = phone;
            break;
        default:
            cerr << "index out of boundary" << endl;
    }
    return result;
}

bool line::changeNum(int i, int num) {
    switch(i) {
        case 1:
            id = num;
            break;
        case 2:
            dis_num = num;
            break;
        case 7:
            number = num;
            break;
        case 12:
            zip = num;
            break;
        default:
            cerr << "invalid index at changeNum" << endl;
            return false;
    }

    return true;
}

bool line::changeString(int i, std::string& str) {
    switch(i) {
        case 3:
            id_card_num = str;
            break;
        case 4:
            given_name = str;
            break;
        case 6:
            surname = str;
            break;
        case 8:
            street = str;
            break;
        case 9:
            code = str;
            break;
        case 10:
            city = str;
            break;
        case 11:
            state = str;
            break;
        case 13:
            college = str;
            break;
        case 14:
            addr = str;
            break;
        case 15:
            phone = str;
            break;
        default:
            cerr << "invalid index at changeString" << endl;
            return false;
    }
    return true;
}

bool line::changeType(char ch) {
    type = ch;
    return true;
}