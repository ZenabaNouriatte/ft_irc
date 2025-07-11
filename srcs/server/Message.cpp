#include "Message.hpp"

Message::Message(const std::string& raw_msg): raw(raw_msg)
{
        std::cout << "Raw msg: " << raw << std::endl;
}