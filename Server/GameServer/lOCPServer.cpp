#include "pch.h"
#include "lOCPServer.h"

lOCPServer::lOCPServer()
{
	//cl_id = 0;
	for (int i = 0; i < 100; ++i) clients[i].cl_id = i;
}

//lOCPServer::~lOCPServer()
//{
//	WSACleanup();
//	cout << "����" << endl;
//}

bool lOCPServer::Init()
{
	WSADATA wsaData;

	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;
	listensocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	if (listensocket == INVALID_SOCKET)
		return 0;
	
	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(12345);
	if (::bind(listensocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		return 0;
	if (::listen(listensocket, SOMAXCONN) == SOCKET_ERROR)
		return 0;
	return true;
}

void lOCPServer::Start()
{
	DWORD dwBytes;
	iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	cout << "���� ����" << endl;
	if (!CreateWorkerThreads()) return;
	//while (true)
	//{
	//	//PostAccept();
	//}
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(listensocket), iocpHandle, 0, 0);
	
	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	char   accept_buf[sizeof(SOCKADDR_IN) * 2 + 32 + 100];
	Overlapped  accept_ex;
	*(reinterpret_cast<SOCKET*>(&accept_ex.recvBuffer)) = c_socket;
	ZeroMemory(&accept_ex.overlapped, sizeof(accept_ex.overlapped));
	accept_ex.type = IO_ACCEPT;
	AcceptEx(listensocket, c_socket, accept_buf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, &accept_ex.overlapped);

}

bool lOCPServer::CreateWorkerThreads()
{
	unsigned int numCores = thread::hardware_concurrency();

	// �ּ� 1���� ��Ŀ �����带 ����
	int WorkerCount = numCores;
	// ��Ŀ ������ ����
	for (int i = 0; i < WorkerCount; ++i) {
		workerthread.emplace_back([this]() {WorkerThread(); });
	}

	std::cout << "Created " << WorkerCount << " worker threads." << std::endl;
	return true;
}

void lOCPServer::WorkerThread()
{
	while (true)
	{
		//cout << "����";
		DWORD bytesTransferred;
		LONG64 completionKey;
		WSAOVERLAPPED* p_over;

		// IO �Ϸ� ��Ŷ�� ��ٸ��ϴ�.
		bool ret = GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, 
			(PULONG_PTR)&completionKey, (LPOVERLAPPED*)&p_over, INFINITE);

		int cl_id = static_cast<int>(completionKey);
		Overlapped* overlap = reinterpret_cast<Overlapped*>(p_over);
		if (FALSE == ret) {
			int err_no = WSAGetLastError();
			cout << "GQCS Error : ";
			//error_display(err_no);
			cout << endl;
			continue;
		}
		// IO �۾� ������ �����ϱ� ���� �Ϸ� Ű�� Ȯ��
		switch (overlap->type)
		{
		case IO_RECV: {
			// ���� �Ϸ� ó��
			/*if (false == HandleReceive(cl_id, overlap, bytesTransferred))
				continue;*/
			cout << "Received data: " << overlap->recvBuffer << endl;
			ClientInfo& cl = clients[cl_id];
			int remain_data = bytesTransferred + cl.prev;
			unsigned char* packet_start = overlap->recvBuffer;
			int packet_size = packet_start[0];

			while (packet_size <= remain_data) {
				process_packet(cl_id, packet_start);
				remain_data -= packet_size;
				packet_start += packet_size;
				if (remain_data > 0) packet_size = packet_start[0];
				else break;
			}

			if (0 < remain_data) {
				cl.prev = remain_data;
				memcpy(&overlap->recvBuffer, packet_start, remain_data);
			}
			cl.c_recv();
			break;
		}
		case IO_SEND:
			// �۽� �Ϸ� ó��
			//HandleSend(overlap, bytesTransferred);
			break;
		case IO_ACCEPT: {
			//// Accept �Ϸ� ó��
			cout << "Accept Completed.\n";
			SOCKET c_socket = *(reinterpret_cast<SOCKET*>(overlap->recvBuffer));
			int a_id = get_id();
			if (-1 == a_id)
				cout << "over" << endl;
			else
			{
				ClientInfo& cl = clients[a_id];
				cl.cl_id = a_id;
				cl.cl_state = ST_ACCEPT;
				cl.prev = 0;
				cl.c_overlapped.type = IO_RECV;
				cl.c_overlapped.wsabuf.buf = reinterpret_cast<char*>(cl.c_overlapped.recvBuffer);
				cl.c_overlapped.wsabuf.len = sizeof(cl.c_overlapped.recvBuffer);
				ZeroMemory(&cl.c_overlapped.overlapped, sizeof(cl.c_overlapped.overlapped));
				cl.c_socket = c_socket;
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), iocpHandle, a_id, 0);
				cl.c_recv();

			}
			ZeroMemory(&overlap->overlapped, sizeof(overlap->overlapped));
			c_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
			*(reinterpret_cast<SOCKET*>(overlap->recvBuffer)) = c_socket;
			AcceptEx(listensocket, c_socket, overlap->recvBuffer + 8, 0, sizeof(SOCKADDR_IN) + 16,
				sizeof(SOCKADDR_IN) + 16, NULL, &overlap->overlapped);
			//HandleAccept(overlap);
			break;
			// �ʿ信 ���� �� ���� ��츦 �߰� (��: IO_CONNECT)
		}
	/*	default:
			cout << "Unknown IO type" << endl;
			break;*/
		}
	}
}

bool lOCPServer::HandleAccept(Overlapped* overlapped)
{
	//// Accept �۾� �Ϸ� ó��, �ʿ信 ���� ���ο� ��Ŀ ������ ���� �Ǵ� ������ ���� ó�� ���� ����
	//// ...
	//// ���ο� Accept �۾��� ����Ͽ� ����ؼ� ������ ������ ����
	////PostAccept();
	DWORD dwBytes;
	cout << "Accept Completed.\n";
	SOCKET c_socket = *(reinterpret_cast<SOCKET*>(overlapped->recvBuffer));
	int a_id = get_id();
	
	ClientInfo& cl = clients[a_id];
	//cl.state_lock.lock();
	cl.cl_id = a_id;
	//cl.cl_state = ST_ACCEPT;
	//.cl.state_lock.unlock();
	cl.prev = 0;
	cl.c_overlapped.type = IO_RECV;
	cl.c_overlapped.wsabuf.buf = reinterpret_cast<char*>(cl.c_overlapped.recvBuffer);
	cl.c_overlapped.wsabuf.len = sizeof(cl.c_overlapped.recvBuffer);
	ZeroMemory(&cl.c_overlapped.overlapped, sizeof(cl.c_overlapped.overlapped));
	cl.c_socket = c_socket;
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), iocpHandle, a_id, 0);
	cl.c_recv();
	

	ZeroMemory(&overlapped->overlapped, sizeof(overlapped->overlapped));
	c_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	*(reinterpret_cast<SOCKET*>(overlapped->recvBuffer)) = c_socket;
	AcceptEx(listensocket, c_socket, overlapped->recvBuffer + 8, 0, sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16, &dwBytes, &overlapped->overlapped);
	return true;
	
}
bool lOCPServer::HandleReceive(int cl_id, Overlapped* overlapped, DWORD bytesTransferred)
{
	// ���ŵ� ������ ó��, ��Ŷ ó�� ���� ����
	// overlapped->recvBuffer���� ���ŵ� �����Ϳ� �׼����� �� �ֽ��ϴ�.

	// ����: ���ŵ� ������ ���
	cout << "Received data: " << overlapped->recvBuffer << endl;
	ClientInfo& cl = clients[cl_id];
	int remain_data = bytesTransferred + cl.prev;
	unsigned char* packet_start = overlapped->recvBuffer;
	int packet_size = packet_start[0];

	while (packet_size <= remain_data) {
		process_packet(cl_id, packet_start);
		remain_data -= packet_size;
		packet_start += packet_size;
		if (remain_data > 0) packet_size = packet_start[0];
		else break;
	}

	if (0 < remain_data) {
		cl.prev = remain_data;
		memcpy(&overlapped->recvBuffer, packet_start, remain_data);
	}
	cl.c_recv();
	return true;
}


//void lOCPServer::HandleSend(Overlapped* overlapped, DWORD bytesTransferred)
//{
//	// �۽� �۾� �Ϸ� ó��, �ʿ信 ���� �߰� ���� ����
//	// ...
//	cout << "Send data: " << overlapped->recvBuffer << endl;
//	//PostSend(overlapped);
//	// �ʿ��� ��� ���ҽ� ����, ���� �ݱ� ���� ����
//}


//void lOCPServer::PostRecv(Overlapped* overlapped)
//{
//	// ���ο� ���� �۾��� �غ��ϰ� ���
//	DWORD flags = 0;
//	DWORD recvBytes = 0;
//
//	int result = WSARecv(overlapped->socket, &(overlapped->wsabuf), 1, &recvBytes, &flags, &(overlapped->overlapped), NULL);
//
//	if (result == SOCKET_ERROR && (WSAGetLastError() != WSA_IO_PENDING))
//	{
//		cout << "WSARecv failed with error: " << WSAGetLastError() << endl;
//	}
//}
//
//void lOCPServer::PostSend(Overlapped* overlapped)
//{
//	// ���ο� �۽� �۾��� �غ��ϰ� ���
//	DWORD sendBytes = 0;
//
//	int result = WSASend(overlapped->socket, &(overlapped->wsabuf), 1, &sendBytes, 0, &(overlapped->overlapped), NULL);
//
//	if (result == SOCKET_ERROR && (WSAGetLastError() != WSA_IO_PENDING))
//	{
//		cout << "WSASend failed with error: " << WSAGetLastError() << endl;
//	}
//}


//void lOCPServer::PostAccept()
//{
//	int result;
//	// Ŭ���̾�Ʈ ����
//	SOCKADDR_IN clientAddr;
//	int addrLen = sizeof(SOCKADDR_IN);
//	DWORD recvBytes;
//	DWORD flags;
//	SOCKET clientsocket = WSAAccept(listensocket, (struct sockaddr*)&clientAddr, &addrLen, NULL, NULL);
//	if (clientsocket == INVALID_SOCKET) {
//		cout << "accept ����" << endl;
//		return;
//	}
//	Overlapped* overlap = new Overlapped();
//	overlap->socket = clientsocket;
//	overlap->recvBytes = 0;
//	overlap->sendBytes = 0;
//	overlap->wsabuf.len = 1024;
//	overlap->wsabuf.buf = overlap->recvBuffer;
//	flags = 0;
//	::CreateIoCompletionPort((HANDLE)clientsocket, iocpHandle, (DWORD)overlap, 0);
//
//	// Ŭ���̾�Ʈ���Լ� ù ��° ��Ŷ�� �ޱ� ���� Recv ȣ��
//	PostRecv(overlap);
//}