#pragma once

#define WIN32_LEAN_AND_MEAN // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#ifdef _DEBUG
#pragma comment(lib, "Debug\\ServerCore.lib")
#else
#pragma comment(lib, "Release\\ServerCore.lib")
#endif

#include "CorePch.h"
#include "ClientInfo.h"
#include "Overlapped.h"
#include "PacketHandler.h"

#include "protocol.h"

extern array <ClientInfo*, 100> clients;
void Login_Back(int _s_id);