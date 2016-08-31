/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: GPL 2.0
    Started: 2016-8-11
    Notes:
        Manages the creation of clients and routes messages.
*/

#include <Services\Servicehandler.h>
#include "Clientmanager.h"
#include <json.hpp>

void Ayriaclient::onDisconnect() {};
std::string Ayriaclient::onMessage(std::string &Message) 
{
    nlohmann::json JSONRequest;

    try
    {
        JSONRequest.parse(Message);
        std::string Name = JSONRequest["Request"];

        /*
            TODO(Convery):
            Check if this request should be handled internally.
        */
        if (0 == std::strcmp(Name.c_str(), "debugtest"))
            return "{ Status: \"Debug\" }";
    }
    catch (...) {}

    // Else we pass it along to the handler.
    return Service::Handlemessage(Message);
};
void Ayriaclient::onConnect(mg_connection *Socket) {};
void Ayriaclient::Sendmessage(std::string &Message) {};
