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
std::unordered_map<std::string, IService *> Servicemap;

void Service::Eventhandler(mg_connection *Connection, int EventID, void *Eventdata)
{
    switch (EventID)
    {
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
        {
            /*
                TODO(Convery):
                Create a new user_data structure for this connection.
            */
            break;
        }
        case MG_EV_CLOSE:
        {
            if (Connection->flags & MG_F_IS_WEBSOCKET)
            {
                /*
                    TODO(Convery):
                    Delete the user_data structure for this connection.
                */
            }
            break;
        }

        case MG_EV_WEBSOCKET_FRAME:
        {
            // Convert the incoming data to a manageable string.
            websocket_message *Message = (websocket_message *)Eventdata;
            std::string Request((const char *)Message->data, Message->size);


            mg_send_websocket_frame(Connection, WEBSOCKET_OP_CLOSE, "", 0);
            break;
        }
        case MG_EV_HTTP_REQUEST:
        {
            http_message *Message = (http_message *)Eventdata;
            std::string URI(Message->uri.p, Message->uri.len);

            // Find the service in our list and call it.
            auto Service = Servicemap.find(URI);
            if (Service == Servicemap.end() || !Service->second->Handlerequest(Connection, Message))
            {
                nlohmann::json Response;
                Response["result"] = 500;
                Response["error"] = "No handler could handle the request";

                mg_printf(Connection, "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
                mg_printf_http_chunk(Connection, Response.dump(4).c_str());
                mg_send_http_chunk(Connection, "", 0);
            }

            break;
        }
    }
}
void Service::Register(IService *Service)
{
    Servicemap.emplace(Service->Servicename(), Service);
}
