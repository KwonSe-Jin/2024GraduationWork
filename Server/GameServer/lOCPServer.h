#pragma once
#include "Overlapped.h"
#include "protocol.h"
class lOCPServer
{
public:
	lOCPServer();
	virtual ~lOCPServer();
	bool Init();
	void Start();
	bool CreateWorkerThreads();
	void WorkerThread();
	void RecvPacket(Overlapped* overlap); 
	void SendPacket(Overlapped* overlap, void* packet);
	virtual bool OnRecv(int s_id, Overlapped* overlap, DWORD num_bytes) { return true; };
protected:
	SOCKET listensocket;
	HANDLE iocpHandle;
	Overlapped* overlap;
	bool	bAccept;			// ��û ���� �÷���
	bool	bWorkerThread;	// �۾� ������ ���� �÷���
public:
	vector <thread> workerthread;
};

