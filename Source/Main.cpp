/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: GPL 2.0
    Started: 2016-7-18
    Notes:
        Server entrypoint and initialization.
*/

#include <Configuration\All.h>
#include <Clients\Clientmanager.h>
#include <mongoose.h>

// Shutdown signal is sent.
static sig_atomic_t Signalreceived = 0;

int main(int argc, char **argv)
{
    // Clean the logfile so we only save this session.
    DeleteLogfile();

    // Set the signal handler.
    {
        auto Signalhandler = [](int SignalID) -> void
        {
            Signalreceived = SignalID;
        };

        signal(SIGTERM, Signalhandler);
        signal(SIGINT, Signalhandler);
    }

    // Start the server.
    {
        mg_mgr Manager;
        mg_connection *Connection;

        // Initialize the manager without any userdata.
        mg_mgr_init(&Manager, NULL);

        // Open 10 ports for the client handler.
        for (uint32_t i = 28000; i < 28010; ++i)
        {
            Connection = mg_bind(&Manager, va_small("%i", i), Clientconnection::Eventhandler);
            if(Connection) mg_set_protocol_http_websocket(Connection);
        }

        // Open 10 ports for the server handler.
        for (uint32_t i = 28000; i < 28010; ++i)
        {
            // Connection = mg_bind(&Manager, va_small("%i", i), Serverconnection::Eventhandler);
            // if(Connection) mg_set_protocol_http_websocket(Connection);
        }

        // Loop until we should quit.
        while (Signalreceived == 0)
        {
            mg_mgr_poll(&Manager, 200);
        }
        mg_mgr_free(&Manager);
    }

    return 0;
}
