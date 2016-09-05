/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: GPL 2.0
    Started: 2016-9-5
    Notes:
		Manages connections to the client part of the backend.
*/

#include "Clienthandler.h"

// Connected clients as displayed in the console.
std::atomic<uint64_t> Clienthandler::Clientcount;

// Map of the connections and associated clients.
std::pair<std::mutex, std::vector<Ayriaclient *>> Clienthandler::Lingeringconnections;
std::vector<std::pair<std::mutex, std::unordered_map<size_t, Ayriaclient *>>*> Clienthandler::Clientmap;

// Each of the wrappers call this with a different index.
void Clienthandler::onInternalevent(mg_connection *Connection, size_t EventID, void *Eventdata, size_t Index)
{
	// Incoming messages get prioritized.
	if (EventID == MG_EV_WEBSOCKET_FRAME)
	{
		Clientmap[Index]->first.lock();
		{
			websocket_message *Message = (websocket_message *)Eventdata;

			// Copy the message into a string and send it to the client in a new thread.
			auto Lambda = [](std::string Message, Ayriaclient *Client) { Client->onFrame(Message); };
			std::thread(Lambda, std::string((const char *)Message->data, Message->size), Clientmap[Index]->second[size_t(Connection)]).detach();
		}
		Clientmap[Index]->first.unlock();
		return;
	}

	// New connections next.
	if (EventID == MG_EV_WEBSOCKET_HANDSHAKE_DONE)
	{
		Clientmap[Index]->first.lock();
		{
			Clientmap[Index]->second[size_t(Connection)] = new Ayriaclient();
			Clientmap[Index]->second[size_t(Connection)]->onConnect(Connection);

			Clientcount++;
		}
		Clientmap[Index]->first.unlock();
		return;
	}

	// Lastly we check dropped connections.
	if (EventID == MG_EV_CLOSE)
	{
		// Verify that this event is for the websock part.
		if (1 == (Connection->flags & MG_F_IS_WEBSOCKET))
		{
			Clientmap[Index]->first.lock();
			{
				Lingeringconnections.first.lock();
				{
					Clientmap[Index]->second[size_t(Connection)]->onDisconnect();
					Lingeringconnections.second.push_back(Clientmap[Index]->second[size_t(Connection)]);
					Clientmap[Index]->second.erase(size_t(Connection));

					Clientcount--;
				}
				Lingeringconnections.first.unlock();
			}
			Clientmap[Index]->first.unlock();
		}
		return;
	}

	// We don't want other messages.
	mg_close_conn(Connection);
}

// Cleanup thread for lingering connections.
void Clienthandler::Connectioncleanup()
{
	uint64_t Timestamp;

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(30));
		Timestamp = GetTickCount64();

		Lingeringconnections.first.lock();
		{
		RESTART_LOOP:
			for (auto Iterator = Lingeringconnections.second.begin(); Iterator != Lingeringconnections.second.end(); ++Iterator)
			{
				if (*Iterator)
				{
					if (!(*Iterator)->isActive(Timestamp))
					{
						delete (*Iterator);
						Lingeringconnections.second.erase(Iterator);
						goto RESTART_LOOP;
					}
				}
			}
		}
		Lingeringconnections.first.unlock();
	}
}

// Initialize the clientmap.
Clienthandler::Clienthandler()
{
	Clientmap.push_back(new std::pair<std::mutex, std::unordered_map<size_t, Ayriaclient *>>());
	Clientmap.push_back(new std::pair<std::mutex, std::unordered_map<size_t, Ayriaclient *>>());
	Clientmap.push_back(new std::pair<std::mutex, std::unordered_map<size_t, Ayriaclient *>>());
	Clientmap.push_back(new std::pair<std::mutex, std::unordered_map<size_t, Ayriaclient *>>());
	Clientmap.push_back(new std::pair<std::mutex, std::unordered_map<size_t, Ayriaclient *>>());
}

// Wrappers for the eventhandler.
void Clienthandler::onEvent1(mg_connection *Connection, size_t EventID, void *Eventdata)
{
	onInternalevent(Connection, EventID, Eventdata, 0);
}
void Clienthandler::onEvent2(mg_connection *Connection, size_t EventID, void *Eventdata)
{
	onInternalevent(Connection, EventID, Eventdata, 1);
}
void Clienthandler::onEvent3(mg_connection *Connection, size_t EventID, void *Eventdata)
{
	onInternalevent(Connection, EventID, Eventdata, 2);
}
void Clienthandler::onEvent4(mg_connection *Connection, size_t EventID, void *Eventdata)
{
	onInternalevent(Connection, EventID, Eventdata, 3);
}
void Clienthandler::onEvent5(mg_connection *Connection, size_t EventID, void *Eventdata)
{
	onInternalevent(Connection, EventID, Eventdata, 4);
}
