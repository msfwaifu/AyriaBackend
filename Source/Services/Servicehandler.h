/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: GPL 2.0
    Started: 2016-7-18
    Notes:
        This is basically a glorified callback routing the requests.
*/

#pragma once
#include <Configuration\All.h>
#include <mongoose.h>

struct IService
{
    virtual std::string Servicename() = 0;

    // Handle the request as needed, returns true if the request was handled.
    virtual bool Handlerequest(mg_connection *Connection, http_message *Request) = 0;
};

namespace Service
{
    // Parse the request and send it to a handler, or register a new service.
    void Eventhandler(struct mg_connection *Connection, int EventID, void *Eventdata);
    void Register(IService *Service);
}

// Forward declaration of a service.
#define Declareservice(Name)                                                        \
struct Service_ ##Name : public IService                                            \
{                                                                                   \
    virtual std::string Servicename() { return #Name; };                            \
    virtual bool Handlerequest(mg_connection *Connection, http_message *Request);   \
}                                                                                   \

// List of services we support in this version.
/* TODO */
