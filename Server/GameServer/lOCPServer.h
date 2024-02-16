#pragma once
#include "pch.h"
//#include "PacketHandler.h"
class PacketHandler;

class lOCPServer
{
public:
	lOCPServer();
	virtual ~lOCPServer();
	bool Init();
	void Start();
	bool CreateWorkerThreads();
	void WorkerThread();
	bool HandleReceive(int cl_id, Overlapped* overlapped, DWORD bytesTransferred);
	void HandleSend(Overlapped* overlapped, DWORD bytesTransferred);
	bool HandleAccept(Overlapped* overlapped);
	//void PostRecv(Overlapped* overlapped);
	//void PostSend(Overlapped* overlapped);
	//void PostAccept();
	//virtual bool OnRecv(int s_id, Overlapped* overlap, DWORD num_bytes) { return true; };
protected:
	SOCKET listensocket;
	HANDLE iocpHandle;

	std::unique_ptr<PacketHandler> p_handler;
	bool	bAccept;			// ��û ���� �÷���
	bool	bWorkerThread;	// �۾� ������ ���� �÷���
public:
	vector <thread> workerthread;
	int cl_id;
};


