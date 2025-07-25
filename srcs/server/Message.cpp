/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmogne <zmogne@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 14:30:42 by cschmid           #+#    #+#             */
/*   Updated: 2025/07/25 14:44:07 by zmogne           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"

/*========== METHODES ==========*/
Message::Message(std::string raw_msg): raw(raw_msg)
{
        msgParsing(raw);
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
        }
}