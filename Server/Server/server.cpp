#include "pch.h"


HANDLE g_h_iocp;
HANDLE g_timer;

int main()
{
	wcout.imbue(locale("korean"));
	IOCPServer server;

	server.Initialize();
	server.Run();
	
}


