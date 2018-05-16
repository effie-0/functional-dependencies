#ifndef FD_BASE_H
#define FD_BASE_H

# include <string>

struct value {
    int num;
    std::string str;
    bool isStr;
};

class line {
    line();
    ~line() = default;
    line(int id, int dis_num, std::string& id_card_num, std::string& given_name, char type, std::string& surname,
         int number, std::string& street, std::string& code, std::string& city, std::string& state, int zip,
         std::string& college, std::string& addr, std::string& phone);

protected:
    int id;
    int dis_num;
    std::string id_card_num;
    std::string given_name;
    char type;
    std::string surname;
    int number;
    std::string street;
    std::string code;
    std::string city;
    std::string state;
    int zip;
    std::string college;
    std::string addr;
    std::string phone;

public:
    value& operator[](int i);
};

#endif //FD_BASE_H
