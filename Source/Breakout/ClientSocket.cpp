// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientSocket.h"
#include <sstream>
#include <process.h>

#include "Game/BOGameMode.h"
#include "Character/Character1.h"
#include "Character/CharacterBase.h"
#include "Player/CharacterController.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformAffinity.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"
#pragma region Main Thread Code
ClientSocket::ClientSocket(UBOGameInstance* inst) :StopTaskCounter(0)
{
	gameinst = inst;
	
	Thread = FRunnableThread::Create(this, TEXT("Network Thread"));
	
}

ClientSocket::~ClientSocket() {
	if (Thread)
	{
		// ������ ����
		Thread->WaitForCompletion();
		Thread->Kill();
		delete Thread;
	}
}

bool ClientSocket::InitSocket()
{

	return true;
}

bool ClientSocket::Connect()
{

	WSADATA wsaData;
	// ���� ������ 2.2�� �ʱ�ȭ
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		UE_LOG(LogTemp, Error, TEXT("INITAILAIZING ERROR"));
		return false;
	}

	// TCP ���� ����
	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ServerSocket == INVALID_SOCKET) {
		return false;
	}

	// ������ ���� ������ ������ ����ü
	SOCKADDR_IN stServerAddr;
	::memset(&stServerAddr, 0, sizeof(stServerAddr));
	// ������ ���� ��Ʈ �� IP
	stServerAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, SERVER_IP, &stServerAddr.sin_addr);
	stServerAddr.sin_port = htons(SERVER_PORT);
	//stServerAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	int nResult = connect(ServerSocket, (sockaddr*)&stServerAddr, sizeof(stServerAddr));
	if (nResult == SOCKET_ERROR) {
		return false;
	}
	return true;
}


void ClientSocket::CloseSocket()
{
	closesocket(ServerSocket);
	WSACleanup();
}

bool ClientSocket::PacketProcess(char* ptr)
{
	//UE_LOG(LogClass, Warning, TEXT("init?"));
	//static bool first_time = true;
	switch (ptr[1])
	{
	case SC_LOGIN_OK: {
		SC_LOGIN_BACK* packet = reinterpret_cast<SC_LOGIN_BACK*>(ptr);
		//to_do

		//UE_LOG(LogClass, Warning, TEXT("RECV ROGIN?"));
		//login_cond = true;
		//CPlayer player;
		//player.Id = packet->clientid;
		///*player.X = packet->x;
		//player.Y = packet->y;
		//player.Z = packet->z;
		//player.p_type = packet->p_type;*/
		//PlayerInfo.players[player.Id] = player;
		//MyCharacterController->SetPlayerID(player.Id);
		//MyCharacterController->SetPlayerInfo(&PlayerInfo);
		//MyCharacterController->SetInitPlayerInfo(player);
		//UE_LOG(LogClass, Warning, TEXT("recv - id: %d, x: %d"), player.Id, player.X);
		break;
	}
	case SC_OTHER_PLAYER:
	{
		//UE_LOG(LogClass, Warning, TEXT("other ROGIN?"));
		SC_PLAYER_SYNC* packet = reinterpret_cast<SC_PLAYER_SYNC*>(ptr);
		auto info = make_shared<CPlayer>();
		info->Id = packet->id;
		info->X = packet->x;
		info->Y = packet->y;
		info->Z = packet->z;
		info->Yaw = packet->yaw;
		//info->p_type = packet->p_type;
		//float z = packet->z;
		UE_LOG(LogClass, Warning, TEXT("recv - info->id: %d,"), info->Id);
		MyCharacterController->SetNewCharacterInfo(info);
		break;
	}
	case SC_MOVE_PLAYER:
	{
		//UE_LOG(LogClass, Warning, TEXT("recv move?"));
		CS_MOVE_PACKET* packet = reinterpret_cast<CS_MOVE_PACKET*>(ptr);
		PlayerInfo.players[packet->id].X = packet->x;
		PlayerInfo.players[packet->id].Y = packet->y;
		PlayerInfo.players[packet->id].Z = packet->z;
		PlayerInfo.players[packet->id].Yaw = packet->yaw;
		PlayerInfo.players[packet->id].VeloX = packet->vx;
		PlayerInfo.players[packet->id].VeloY = packet->vy;
		PlayerInfo.players[packet->id].VeloZ = packet->vz;
		PlayerInfo.players[packet->id].Max_Speed = packet->Max_speed;
		UE_LOG(LogClass, Warning, TEXT("recv - move player id : %d,"), packet->id);
		break;
	}
	case SC_CHAR_BACK: {
		SC_SELECT_CHARACTER_BACK* packet = reinterpret_cast<SC_SELECT_CHARACTER_BACK*>(ptr);
		CPlayer player;
		player.Id = packet->clientid;
		//player.X = packet->x;
		//player.Y = packet->y;
		//player.Z = packet->z;
		player.p_type = packet->p_type;
		PlayerInfo.players[player.Id] = player;
		MyCharacterController->SetPlayerID(player.Id);
		MyCharacterController->SetPlayerInfo(&PlayerInfo);
		MyCharacterController->SetInitPlayerInfo(player);
		break;
	}
	case SC_OTHER_WEAPO: {
		SC_SYNC_WEAPO* packet = reinterpret_cast<SC_SYNC_WEAPO*>(ptr);
		PlayerInfo.players[packet->id].w_type = packet->weapon_type;
		//float z = packet->z;
		//UE_LOG(LogClass, Warning, TEXT("recv data"));

		break;
	}
	default:
		break;
	}
	return true;
}

void ClientSocket::Send_Login_Info(char* id, char* pw)
{
	//��Ŷ ����
	CS_LOGIN_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = CS_LOGIN;
	strcpy_s(packet.id, id);
	strcpy_s(packet.pw, pw);
	//cs_login_packet
	//auto location = player->GetActorLocation();
	//packet.x = location.X;
	//packet.y = location.Y;
	//packet.z = location.Z;
	//packet.p_type = character_type;
	SendPacket(&packet);
	//Send(packet.size, &packet);
	UE_LOG(LogClass, Warning, TEXT("Sending login info - id: %s, pw: %s"), ANSI_TO_TCHAR(id), ANSI_TO_TCHAR(pw));
	
}

void ClientSocket::Send_Move_Packet(int sessionID, FVector Location, FRotator Rotation, FVector Velocity, float Max_speed)
{
	//if (login_cond == true) {
		CS_MOVE_PACKET packet;
		packet.size = sizeof(packet);
		packet.type = CS_MOVE_Packet;
		packet.id = sessionID;
		packet.x = Location.X;
		packet.y = Location.Y;
		packet.z = Location.Z;
		packet.yaw = Rotation.Yaw;
		packet.vx = Velocity.X;
		packet.vy = Velocity.Y;
		packet.vz = Velocity.Z;
		packet.Max_speed = Max_speed;
		//Send(packet.size, &packet);
		SendPacket(&packet);
		//UE_LOG(LogClass, Warning, TEXT("send move"));
	//}
}

void ClientSocket::Send_Character_Type(PlayerType type)
{
	auto player = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(MyCharacterController, 0));
	CS_SELECT_CHARACTER packet;
	packet.size = sizeof(packet);
	packet.type = CS_SELECT_CHAR;
	//Send(packet.size, &packet);
	packet.p_type = type;
	SendPacket(&packet);
}

void ClientSocket::Send_Weapon_Type(WeaponType type, int sessionID)
{
	CS_SELECT_WEAPO packet;
	packet.size = sizeof(packet);
	packet.type = CS_SELECT_WEP;
	packet.id = sessionID;
	packet.weapon_type = type;
	//Send(packet.size, &packet);
	SendPacket(&packet);
}
bool ClientSocket::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("Thread has been initialized"));
	return true;
}
uint32 ClientSocket::Run()
{
	// �𸮾� ���� �α� ���
	FPlatformProcess::Sleep(0.03);
	//	Concurrency::concurrent_queue<char> buffer;
		////Connect();
	Iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(ServerSocket), Iocp, 0, 0);

	RecvPacket();

	//Send_LoginPacket();

	SleepEx(0, true);
	//StopTaskCounter.GetValue() == 0
	// recv while loop ����
	// StopTaskCounter Ŭ���� ������ ����� Thread Safety�ϰ� ����
	while (true)
	{
		DWORD num_byte;
		LONG64 iocp_key;
		WSAOVERLAPPED* p_over;

		BOOL ret = GetQueuedCompletionStatus(Iocp, &num_byte, (PULONG_PTR)&iocp_key, &p_over, INFINITE);

		Overlap* exp_over = reinterpret_cast<Overlap*>(p_over);

		if (false == ret) {
			int err_no = WSAGetLastError();
			if (exp_over->_op == IO_SEND)
				delete exp_over;
			continue;
		}

		switch (exp_over->_op) {
		case IO_RECV: {
			if (num_byte == 0) {
				//Disconnect();
				continue;
			}
			int remain_data = num_byte + _prev_size;
			char* packet_start = exp_over->_net_buf;
			int packet_size = packet_start[0];
			while (packet_size <= remain_data) {
				PacketProcess(packet_start);
				remain_data -= packet_size;
				packet_start += packet_size;
				if (remain_data > 0) packet_size = packet_start[0];
				else break;
			}

			if (0 < remain_data) {
				_prev_size = remain_data;
				memcpy(&exp_over->_net_buf, packet_start, remain_data);
			}

			RecvPacket();
			SleepEx(0, true);
			break;
		}
		case IO_SEND: {
			if (num_byte != exp_over->_wsa_buf.len) {
				//Disconnect();
			}
			delete exp_over;
			break;
		}

		}

	}
	return 0;

}
void ClientSocket::Stop()
{
	// thread safety ������ ������ while loop �� ���� ���ϰ� ��
	StopTaskCounter.Increment();
}

void ClientSocket::Exit()
{
	if (ServerSocket)
	{
		closesocket(ServerSocket);
		WSACleanup();
	}
}

//bool ClientSocket::StartListen()
//{
//	// ������ ����
//	if (Thread != nullptr) return false;
//	Thread = FRunnableThread::Create(this, TEXT("ClientSocket"), 0, TPri_BelowNormal);
//	return (Thread != nullptr);
//}
//
//void ClientSocket::StopListen()
//{
//	// ������ ����
//	Stop();
//	Thread->WaitForCompletion();
//	Thread->Kill();
//	delete Thread;
//	Thread = nullptr;
//	StopTaskCounter.Reset();
//}

void ClientSocket::SetPlayerController(ACharacterController* CharacterController)
{
	if (CharacterController)
	{
		MyCharacterController = CharacterController;
	}
}

bool ClientSocket::Send(const int SendSize, void* SendData)
{
	char buff[BUFSIZE];
	memcpy(buff, SendData, SendSize);

	int nSendLen = send(ServerSocket, buff, buff[0], 0);
	UE_LOG(LogNet, Display, TEXT("Send Packet SIZE %d"), nSendLen);

	return true;
}
void ClientSocket::RecvPacket()
{
	//UE_LOG(LogClass, Warning, TEXT("recv data"));
	DWORD recv_flag = 0;
	ZeroMemory(&_recv_over._wsa_over, sizeof(_recv_over._wsa_over));
	_recv_over._wsa_buf.buf = reinterpret_cast<char*>(_recv_over._net_buf + _prev_size);
	_recv_over._wsa_buf.len = sizeof(_recv_over._net_buf) - _prev_size;
	int ret = WSARecv(ServerSocket, &_recv_over._wsa_buf, 1, 0, &recv_flag, &_recv_over._wsa_over, NULL);
	if (SOCKET_ERROR == ret) {
		int error_num = WSAGetLastError();
	}

}
void ClientSocket::SendPacket(void* packet)
{
	int psize = reinterpret_cast<unsigned char*>(packet)[0];
	Overlap* ex_over = new Overlap(IO_SEND, psize, packet);
	int ret = WSASend(ServerSocket, &ex_over->_wsa_buf, 1, 0, 0, &ex_over->_wsa_over, NULL);
	if (SOCKET_ERROR == ret) {
		int error_num = WSAGetLastError();
		if (ERROR_IO_PENDING != error_num)
			WSAGetLastError();
	}
}