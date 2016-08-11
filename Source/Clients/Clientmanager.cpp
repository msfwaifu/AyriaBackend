/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: GPL 2.0
    Started: 2016-8-11
    Notes:
        Associates a connection with clientdata.
*/

#include "Clientmanager.h"
#include <Services\Servicehandler.h>
#include <unordered_map>
#include <json.hpp>
#include <memory>
#include <thread>
#include <mutex>
#include <map>

// Client connections and objects.
std::mutex Mapguard;
std::unordered_map<mg_connection *, std::shared_ptr<Ayriaclient>> Clientmap;

// Callback for mongoose.
void Clientconnection::Eventhandler(struct mg_connection *Connection, int EventID, void *Eventdata)
{
    switch (EventID)
    {
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
        {
            Mapguard.lock();
            {
                Clientmap[Connection] = std::make_shared<Ayriaclient>();
                Clientmap[Connection]->onConnect(Connection);
            }
            Mapguard.unlock();

            break;
        }

        case MG_EV_CLOSE:
        {
            if ((Connection->flags & MG_F_IS_WEBSOCKET) == 1)
            {
                Mapguard.lock();
                {
                    Clientmap[Connection]->onDisconnect();
                    Clientmap.erase(Connection);
                }
                Mapguard.unlock();
            }            

            break;
        }

        case MG_EV_WEBSOCKET_FRAME:
        {
            Mapguard.lock();
            {
                websocket_message *wm = (websocket_message *)Eventdata;

                // Copy the message into a string and send to the client in a new thread.
                auto Lambda = [](std::string Message, std::shared_ptr<Ayriaclient> Client) { Client->onMessage(Message); };
                std::thread(Lambda, std::string((char *)wm->data, wm->size), Clientmap[Connection]).detach();
            }
            Mapguard.unlock();

            break;
        }

        case MG_EV_HTTP_REQUEST:
        {
            http_message *Message = (http_message *)Eventdata;

            // We only handle validation requests.
            if(!std::strstr(Message->uri.p, "validate"))
            {
                nlohmann::json Response;
                Response["result"] = 500;
                Response["error"] = "No handler could handle the request";

                mg_printf(Connection, "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
                mg_printf_http_chunk(Connection, Response.dump(4).c_str());
                mg_send_http_chunk(Connection, "", 0);
            }
            else
            {
                std::string URI(Message->uri.p, Message->uri.len);
                std::string Postdata(Message->body.p, Message->body.len);
                std::string Params(Message->query_string.p, Message->query_string.len);

                mg_printf(Connection, "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
                mg_printf_http_chunk(Connection, Service::Handlemessage(URI, Params, Postdata).c_str());
                mg_send_http_chunk(Connection, "", 0);
            }

            break;
        }
    }
}
