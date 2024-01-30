// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientSocket.h"
#include <sstream>
#include <process.h>
#include "Game/BOGameMode.h"
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

void ClientSocket::PacketProcess(const char* ptr)
{
}

void ClientSocket::Send_Login_Info(char* id, char* pw)
{
	//��Ŷ ����
	//CS_LOGIN_PACKET packet;
	CS_LOGIN_PACKET packet;
	packet.size = sizeof(packet);
	strcpy(packet.id, id);
	strcpy(packet.pw, pw);

	//cs_login_packet
	SendPacket(&packet);
	UE_LOG(LogClass, Warning, TEXT("Sending login info - id: %s, pw: %s"), ANSI_TO_TCHAR(id), ANSI_TO_TCHAR(pw));
}

bool ClientSocket::Init()
{
	return true;
}
uint32 ClientSocket::Run()
{
	FPlatformProcess::Sleep(0.03);
	// ���Ӹ�带 ������

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
	Overlap* ex_over = new Overlap(OP_SEND, psize, packet);
	int ret = WSASend(ServerSocket, &ex_over->_wsa_buf, 1, 0, 0, &ex_over->_wsa_over, NULL);
	if (SOCKET_ERROR == ret) {
		int error_num = WSAGetLastError();
		if (ERROR_IO_PENDING != error_num)
			WSAGetLastError();
	}
}
