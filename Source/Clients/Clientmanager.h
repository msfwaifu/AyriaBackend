/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: GPL 2.0
    Started: 2016-8-11
    Notes:
        Associates a connection with clientdata.
*/

#pragma once
#include <Configuration\All.h>
#include <mongoose.h>

// Base struct for client info.
struct Ayriaclient
{
    mg_connection *Socket;

    virtual void onDisconnect();    
    virtual void onMessage(std::string Message);
    virtual void onConnect(mg_connection *Socket);
    virtual void Sendmessage(std::string &Message);
};

namespace Clientconnection
{
    // Callback for mongoose.
    void Eventhandler(struct mg_connection *Connection, int EventID, void *Eventdata);
}
