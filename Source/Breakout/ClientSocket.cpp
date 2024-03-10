// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientSocket.h"
#include <sstream>
#include <process.h>
#include "Game/BOGameMode.h"
#include "Character/CharacterBase.h"
#include "Player/CharacterController.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformAffinity.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"

ClientSocket::ClientSocket()
	:StopTaskCounter(0)
{}

ClientSocket::~ClientSocket() {
	delete Thread;
	Thread = nullptr;

	closesocket(ServerSocket);
	WSACleanup();
}

bool ClientSocket::InitSocket()
{
	WSADATA wsaData;
	// ���� ������ 2.2�� �ʱ�ȭ
	int nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (nResult != 0) {
		UE_LOG(LogTemp, Error, TEXT("INITAILAIZING ERROR"));
		return false;
	}

	// TCP ���� ����
	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ServerSocket == INVALID_SOCKET) {
		return false;
	}
	return true;
}

bool ClientSocket::Connect(const char* s_IP, int port)
{
	// ������ ���� ������ ������ ����ü
	SOCKADDR_IN stServerAddr;
	// ������ ���� ��Ʈ �� IP
	stServerAddr.sin_family = AF_INET;
	stServerAddr.sin_port = htons(port);
	stServerAddr.sin_addr.s_addr = inet_addr(s_IP);

	int nResult = connect(ServerSocket, (sockaddr*)&stServerAddr, sizeof(sockaddr));
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

void ClientSocket::PacketProcess(unsigned char* ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case SC_LOGIN_OK: {
		SC_LOGIN_BACK* packet = reinterpret_cast<SC_LOGIN_BACK*>(ptr);
		UE_LOG(LogClass, Warning, TEXT("recv data"));
		/*login_cond = true;*/
		CPlayer player;
		player.Id = packet->cl_id;
		player.X = packet->x;
		player.Y = packet->y;
		player.Z = packet->z;
		PlayerInfo.players[player.Id] = player;
		MyCharacterController->SetPlayerID(player.Id);
		MyCharacterController->SetPlayerInfo(&PlayerInfo);
		MyCharacterController->SetInitPlayerInfo(player);
		UE_LOG(LogClass, Warning, TEXT("recv - id: %d, x: %d"), player.Id, player.X);
		break;
	}
	case SC_OTHER_PLAYER:
	{
		SC_PLAYER_SYNC* packet = reinterpret_cast<SC_PLAYER_SYNC*>(ptr);
		auto info = make_shared<CPlayer>();
		info->Id = packet->id;
		info->X = packet->x;
		info->Y = packet->y;
		info->Z = packet->z;
		info->Yaw = packet->yaw;
		//float z = packet->z;
		UE_LOG(LogClass, Warning, TEXT("recv data"));
		MyCharacterController->SetNewCharacterInfo(info);
		break;
	}
	case SC_MOVE_PLAYER:
	{
		CS_MOVE_PACKET* packet = reinterpret_cast<CS_MOVE_PACKET*>(ptr);
		PlayerInfo.players[packet->id].X = packet->x;
		PlayerInfo.players[packet->id].Y = packet->y;
		PlayerInfo.players[packet->id].Z = packet->z;
		PlayerInfo.players[packet->id].Yaw = packet->yaw;
		PlayerInfo.players[packet->id].VeloX = packet->vx;
		PlayerInfo.players[packet->id].VeloY = packet->vy;
		PlayerInfo.players[packet->id].VeloZ = packet->vz;
		break;
	}
	default:
		break;
	}
}

void ClientSocket::Send_Login_Info(char* id, char* pw)
{
	//��Ŷ ����
	CS_LOGIN_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = CS_LOGIN;
	strcpy(packet.id, id);
	strcpy(packet.pw, pw);

	auto player= Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(MyCharacterController, 0));
	//cs_login_packet
	auto location = player->GetActorLocation();
	
	SendPacket(&packet);
	UE_LOG(LogClass, Warning, TEXT("Sending login info - id: %s, pw: %s"), ANSI_TO_TCHAR(id), ANSI_TO_TCHAR(pw));

}

void ClientSocket::Send_Move_Packet(int sessionID, FVector Location, FRotator Rotation, FVector Velocity)
{
	/*if (login_cond == true) {*/
		CS_MOVE_PACKET packet;
		packet.size = sizeof(packet);
		packet.type = CS_MOVE;
		packet.id = sessionID;
		packet.x = Location.X;
		packet.y = Location.Y;
		packet.z = Location.Z;
		packet.yaw = Rotation.Yaw;
		packet.vx = Velocity.X;
		packet.vy = Velocity.Y;
		packet.vz = Velocity.Z;
		SendPacket(&packet);
		//UE_LOG(LogClass, Warning, TEXT("send move"));
	//}
}

bool ClientSocket::Init()
{
	return true;
}
uint32 ClientSocket::Run()
{
	FPlatformProcess::Sleep(0.03);
	Iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(socket), Iocp, NULL, 0);
	RecvPacket();
	//SleepEx(0, true);
//	StopTaskCounter.GetValue() == 0 && MyCharacterController != nullptr
	FPlatformProcess::Sleep(0.03);
	while (StopTaskCounter.GetValue() == 0 && MyCharacterController != nullptr)
	{
		DWORD num_byte;
		LONG64 iocp_key;
		WSAOVERLAPPED* p_over;
		BOOL ret = GetQueuedCompletionStatus(Iocp, &num_byte, (PULONG_PTR)&iocp_key, &p_over, INFINITE);

		Overlapped* exp_over = reinterpret_cast<Overlapped*>(p_over);

		if (ret == false) {
			int err_no = WSAGetLastError();
			if (exp_over->type == IO_SEND)
				delete exp_over;
			continue;
		}

		switch (exp_over->type) {
		case IO_RECV: {
			if (num_byte == 0) {
				continue;
			}
			int remain_data = num_byte + _prev_size;
			unsigned char* packet_start = exp_over->recvBuffer;
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
				memcpy(&exp_over->recvBuffer, packet_start, remain_data);
			}

			RecvPacket();
			break;
		}
		case IO_SEND: {
			if (num_byte != exp_over->wsabuf.len) {
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
}

bool ClientSocket::StartListen()
{
	// ������ ����
	if (Thread != nullptr) return false;
	Thread = FRunnableThread::Create(this, TEXT("ClientSocket"), 0, TPri_BelowNormal);
	return (Thread != nullptr);
}

void ClientSocket::StopListen()
{
	// ������ ����
	Stop();
	Thread->WaitForCompletion();
	Thread->Kill();
	delete Thread;
	Thread = nullptr;
	StopTaskCounter.Reset();
}

void ClientSocket::RecvPacket()
{
	UE_LOG(LogClass, Warning, TEXT("recv data"));
	DWORD recv_flag = 0;
	ZeroMemory(&_recv_over.overlapped, sizeof(_recv_over.overlapped));
	_recv_over.wsabuf.buf = reinterpret_cast<char*>(_recv_over.recvBuffer + _prev_size);
	_recv_over.wsabuf.len = sizeof(_recv_over.recvBuffer) - _prev_size;
	int ret = WSARecv(ServerSocket, &_recv_over.wsabuf, 1, 0, &recv_flag, &_recv_over.overlapped, NULL);
	if (SOCKET_ERROR == ret) {
		int error_num = WSAGetLastError();
	}
	/*if (ret > 0) {
		UE_LOG(LogClass, Warning, TEXT("recv �� "));
	}*/
}

void ClientSocket::SendPacket(void* packet)
{
	UE_LOG(LogClass, Warning, TEXT("send data"));
	int psize = reinterpret_cast<unsigned char*>(packet)[0];
	Overlapped* ex_over = new Overlapped(IO_SEND, psize, packet);
	int ret = WSASend(ServerSocket, &ex_over->wsabuf, 1, 0, 0, &ex_over->overlapped, NULL);
	if (SOCKET_ERROR == ret) {
		int error_num = WSAGetLastError();
		if (ERROR_IO_PENDING != error_num)
			WSAGetLastError();
	}
}

void ClientSocket::SetPlayerController(ACharacterController* CharacterController)
{
	if (CharacterController)
	{
		MyCharacterController = CharacterController;
	}
}
