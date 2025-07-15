#pragma once
#include <iostream>
#include <vector>


class Message 

{
    public:
        std::string prefix;
        std::string command;
        std::string raw;
        std::vector<std::string> params;

        Message(const std::string& raw_msg); // constructeur
};
