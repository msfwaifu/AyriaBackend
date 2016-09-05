/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: GPL 2.0
    Started: 2016-7-18
    Notes:
        Server entrypoint and initialization.
*/

#include <Configuration\All.h>
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

        // Loop until we should quit.
        while (Signalreceived == 0)
        {
            mg_mgr_poll(&Manager, 200);
        }
        mg_mgr_free(&Manager);
    }

    return 0;
}
