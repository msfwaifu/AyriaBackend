/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: GPL 2.0
    Started: 2016-7-18
    Notes:
        This is basically a glorified callback routing the requests.
*/

#include "Servicehandler.h"
#include <unordered_map>
#include <json.hpp>

// Internal map over the services.
std::unordered_map<std::string, IService *> *Servicemap;

// Send the request to a service based on URL or servicename.
std::string Service::Handlemessage(std::string &URI, std::string &Params, std::string &Postdata)
{
    nlohmann::json Formated;
    Formated["service"] = URI;
    Formated["httpparam"] = Params;
    Formated["postdata"] = Postdata;
    return Handlemessage(Formated.dump());
}
std::string Service::Handlemessage(std::string &JSONString)
{
    nlohmann::json Request;
    Request.parse(JSONString);

    // Find the service in our list and call it.
    auto Service = Servicemap->find(Request["service"]);
    if (Service != Servicemap->end())
    {
        return Service->second->Handlerequest(JSONString);
    }

    // Else return an error.
    nlohmann::json Response;
    Response["result"] = 500;
    Response["error"] = "No handler could handle the request";
    return Response.dump(4);
}

void Service::Register(IService *Service)
{
    if (!Servicemap)
        Servicemap = new std::unordered_map<std::string, IService *>();

    Servicemap->emplace(Service->Servicename(), Service);
}
