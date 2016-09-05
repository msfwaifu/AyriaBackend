/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: GPL 2.0
    Started: 2016-9-5
    Notes:
		Clients are better stored in objects so they can be copied.
*/

#pragma once
#include <Configuration\All.h>
#include <mongoose.h>

class Ayriaclient
{
	mg_connection *Connection;
	uint64_t ReconnectID{};
	uint64_t Timestamp{};	

public:
	void onConnect(mg_connection *Connection);
	void onFrame(std::string &JSONMessage);
	void onPush(std::string &JSONMessage);
	bool isActive(uint64_t Time);
	void onDisconnect();
};
