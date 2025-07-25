#pragma once
#include <iostream>
#include <vector>
#include <string>



class Message 

{
    public:
        std::string prefix;
        std::string command;
        std::string raw;
        std::vector<std::string> params;
        std::string trailing;

        Message(std::string raw_msg); // constructeur
        void msgParsing(std::string raw_msg);
};
