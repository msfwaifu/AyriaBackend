/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: GPL 2.0
    Started: 2016-9-5
    Notes:
		Clients are better stored in objects so they can be copied.
*/

#include "Ayriaclient.h"
#include <json.hpp>

void Ayriaclient::onConnect(mg_connection *Connection)
{
	this->Connection = Connection;
	this->Timestamp = GetTickCount64();
}
void Ayriaclient::onFrame(std::string &JSONMessage)
{
	std::string Service;
	std::string Task;

	try
	{
		nlohmann::json Request = nlohmann::json::parse(JSONMessage);
		Service = Request["Service"].get<std::string>();
		Task = Request["Task"].get<std::string>();

		/*
			TODO(Convery):
			Send the data to a service and put the result in Response.
		*/
		std::string Response;

		// Send the response if needed.
		if (Response.size())
			onPush(Response);
		return;
	}
	catch (...) {}

	// With a corrupted JSON message, drop.
	mg_close_conn(Connection);
}
void Ayriaclient::onPush(std::string &JSONMessage)
{
	mg_send_websocket_frame(this->Connection, WEBSOCKET_OP_TEXT, JSONMessage.c_str(), JSONMessage.size());
}
bool Ayriaclient::isActive(uint64_t Time)
{
	// Clients are allowed a 30 sec reconnect, which in practice is up to 35 sec.
	return (Time - this->Timestamp < 30000);
}
void Ayriaclient::onDisconnect()
{
}
