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

    id_card_num = "";
    given_name = "";
    surname = "";
    street = "";
    code = "";
    city = "";
    state = "";
    college = "";
    addr = "";
    phone = "";
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
    int num;
    while(p != nullptr) {
        if (i > 15)
            break;
        if (i == 1 || i == 2 || i == 7 || i == 12) {
            char* end;
            num = (int)strtol(p, &end, 10);
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
                default:
                    zip = num;
                    break;
            }
        }
        else if (i == 5) {
            if (p[0])
                type = p[0];
        }
        else {
            string s(p);
            switch(i) {
                case 3:
                    id_card_num = s;
                    break;
                case 4:
                    given_name = s;
                    break;
                case 6:
                    surname = s;
                    break;
                case 8:
                    street = s;
                    break;
                case 9:
                    code = s;
                    break;
                case 10:
                    city = s;
                    break;
                case 11:
                    state = s;
                    break;
                case 13:
                    college = s;
                    break;
                case 14:
                    addr = s;
                    break;
                case 15:
                    phone = s;
                    break;
                default:
                    cerr << "invalid index at changeString" << endl;
            }
        }

        i++;
        p = strtok(nullptr, sep);
    }
}

line::line(const line &l) {
    id = l.id;
    dis_num = l.dis_num;
    id_card_num = l.id_card_num;
    given_name = l.given_name;
    type = l.type;
    surname = l.surname;
    number = l.number;
    street = l.street;
    code = l.code;
    city = l.city;
    state = l.state;
    zip = l.zip;
    college = l.college;
    addr = l.addr;
    phone = l.phone;
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

bool line::operator < (const line &line) const {
    if (id < line.id) return true;
    else if (id > line.id) return false;
    else if (dis_num < line.dis_num) return true;
    else if (dis_num > line.dis_num) return false;
    else if (id_card_num < line.id_card_num) return true;
    else if (id_card_num > line.id_card_num) return false;
    else if (given_name > line.given_name) return true;
    else if (given_name < line.given_name) return false;
    else if (type < line.type) return true;
    else if (type > line.type) return false;
    else if (surname < line.surname) return true;
    else if (surname > line.surname) return false;
    else if (number < line.number) return true;
    else if (number > line.number) return false;
    else if (street < line.street) return true;
    else if (street > line.street) return false;
    else if (code < line.code) return true;
    else if (code > line.code) return false;
    else if (city < line.city) return true;
    else if (city > line.city) return false;
    else if (state < line.state) return true;
    else if (state > line.state) return false;
    else if (zip < line.zip) return true;
    else if (zip > line.zip) return false;
    else if (college < line.college) return true;
    else if (college > line.college) return false;
    else if (addr < line.addr) return true;
    else if (addr > line.addr) return false;
    else if (phone < line.phone) return true;
    else if (phone > line.phone) return false;
    else return false;
}

bool line::operator == (const line &l) const {
    if (l.id == id && l.dis_num == dis_num && l.id_card_num == id_card_num && l.given_name == given_name && l.type == type && l.surname == surname && l.number == number && l.street == street && l.code == code && l.city == city && l.state == state && l.zip == zip && l.college == college && l.addr == addr && l.phone == phone) return true;
    return false;
}

void line::set(int i, value v) {
    switch(i) {
        case 1:
            id = v.num;
            break;
        case 2:
            dis_num = v.num;
            break;
        case 3:
            id_card_num = v.str;
            break;
        case 4:
            given_name = v.str;
            break;
        case 5:
            type = v.str[0];
            break;
        case 6:
            surname = v.str;
            break;
        case 7:
            number = v.num;
            break;
        case 8:
            street = v.str;
            break;
        case 9:
            code = v.str;
            break;
        case 10:
            city = v.str;
            break;
        case 11:
            state = v.str;
            break;
        case 12:
            zip = v.num;
            break;
        case 13:
            college = v.str;
            break;
        case 14:
            addr = v.str;
            break;
        case 15:
            phone = v.str;
            break;
        default:
            cerr << "index out of boundary" << endl;
    }
}
