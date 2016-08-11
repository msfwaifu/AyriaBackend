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
    virtual std::string Handlerequest(std::string &JSONString) = 0;
};

namespace Service
{
    // Send the request to a service based on URL or servicename.
    std::string Handlemessage(std::string &URI, std::string &Params, std::string &Postdata);
    std::string Handlemessage(std::string &JSONString);

    // Register a service to be used in the handler.
    void Register(IService *Service);
}
