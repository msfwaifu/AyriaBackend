/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: GPL 2.0
    Started: 2016-9-5
    Notes:
		Manages connections to the client part of the backend.
*/

#pragma once
#include "Clientimplementations\Ayriaclient.h"
#include <Configuration\All.h>
#include <unordered_map>
#include <mongoose.h>
#include <atomic>
#include <mutex>

class Clienthandler
{
	static std::pair<std::mutex, std::vector<Ayriaclient *>> Lingeringconnections;
	static std::vector<std::pair<std::mutex, std::unordered_map<size_t, Ayriaclient *>>*> Clientmap;
	static void onInternalevent(mg_connection *Connection, size_t EventID, void *Eventdata, size_t Index);
	
public:
	static std::atomic<uint64_t> Clientcount;
	static void Connectioncleanup();

	static void onEvent1(mg_connection *Connection, size_t EventID, void *Eventdata);
	static void onEvent2(mg_connection *Connection, size_t EventID, void *Eventdata);
	static void onEvent3(mg_connection *Connection, size_t EventID, void *Eventdata);
	static void onEvent4(mg_connection *Connection, size_t EventID, void *Eventdata);
	static void onEvent5(mg_connection *Connection, size_t EventID, void *Eventdata);

	Clienthandler();
};

static Clienthandler Clientinitialization;
