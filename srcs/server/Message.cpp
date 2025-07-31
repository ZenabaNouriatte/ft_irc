/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmogne <zmogne@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 14:30:42 by cschmid           #+#    #+#             */
/*   Updated: 2025/07/30 15:14:06 by zmogne           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"

/*========== METHODES ==========*/
Message::Message(std::string raw_msg): raw(raw_msg)
{
        msgParsing(raw);
        debugPrint();
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
                else
                        return;
        }

        size_t space = msg.find(' ', i); // extraction commande
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
        while (i <msg.size()) // extraction param et trailing
        {
                if (msg[i] == ':')
                {
                        trailing = msg.substr( i + 1);
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
                while (i < msg.size() && msg[i] == ' ')
                        ++i;
        }
}

void Message::debugPrint() const {
	std::cout << "┌──────────── Parsed Message ────────────" << std::endl;
	if (!prefix.empty())
		std::cout << "│ Prefix   : " << prefix << std::endl;
	std::cout << "│ Command  : " << command << std::endl;
	std::cout << "│ Params   : ";
	if (params.empty()) std::cout << "(none)";
	else for (size_t i = 0; i < params.size(); ++i)
		std::cout << "[" << i << "] = '" << params[i] << "'  ";
	std::cout << std::endl;
	if (!trailing.empty())
		std::cout << "│ Trailing : " << trailing << std::endl;
	std::cout << "└────────────────────────────────────────" << std::endl;
}
