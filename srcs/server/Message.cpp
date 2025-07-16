
#include "Message.hpp"

/*========== METHODES ==========*/
Message::Message(std::string raw_msg): raw(raw_msg)
{
        //std::cout << "Raw msg: " << raw << std::endl;
        msgParsing(raw);
        //std::cout << "DEBUG Prefix = [" << prefix << "]\n";
        //std::cout << "DEBUG Command= [" << command << "]\n";
        //for (size_t i = 0; i < params.size(); ++i)
        //        std::cout << "DEBUG Param[" << i << "] = [" << params[i] << "]\n";
}

void Message::msgParsing(std::string msg)
{
        while (!msg.empty() && (msg[msg.length() - 1] == '\r' || msg[msg.length() - 1] == '\n'))
                msg.erase(msg.length() - 1, 1);
        size_t i = 0;
        if (!msg.empty() && msg[0] == ':') // trouver prefix
        {
                size_t space = msg.find(' ');
                if (space != std::string::npos)
                {
                        prefix = msg.substr(1, space - 1);
                        i = space + 1;
                }
        }

        size_t space = msg.find(' ', i);
        if (space != std::string::npos)
        {
                command = msg.substr(i, space - i);
                i = space + 1;
        }
        else
        {
                command = msg.substr(i);
                return ;
        }
        while (i <msg.size())
        {
                if (msg[i] == ':')
                {
                        params.push_back(msg.substr( i + 1));
                        break;
                }

                size_t next_space = msg.find(' ', i);
                if ( next_space == std::string::npos)
                {
                        params.push_back(msg.substr(i));
                        break;
                }
                params.push_back(msg.substr(i, next_space - i));
                i = next_space + 1;
        }
}