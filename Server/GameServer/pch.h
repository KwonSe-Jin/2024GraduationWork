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

extern array <ClientInfo, 100> clients;
void Login_Back(int _s_id);
void Send_Player(int _s_id, int enm);
void process_packet(int _s_id, unsigned char* p);
int get_id();
