#include "pch.h"
#include "lOCPServer.h"

lOCPServer::lOCPServer()
{

}

lOCPServer::~lOCPServer()
{
	WSACleanup();
	cout << "����" << endl;
}

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
	int result;
	// Ŭ���̾�Ʈ ����
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	DWORD recvBytes;
	DWORD flags;
	iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	cout << "���� ����" << endl;
	if (!CreateWorkerThreads()) return;

	while (true)
	{
		SOCKET clientsocket = WSAAccept(listensocket, (struct sockaddr*)&clientAddr, &addrLen, NULL, NULL);
		if (clientsocket == INVALID_SOCKET) {
			cout << "accept ����" << endl;
			return;
		}
		overlap = new Overlapped();
		overlap->socket = clientsocket;
		overlap->recvBytes = 0;
		overlap->sendBytes = 0;
		overlap->wsabuf.len = 1024;
		overlap->wsabuf.buf = overlap->recvBuffer;
		flags = 0;
		::CreateIoCompletionPort((HANDLE)clientsocket, iocpHandle, (DWORD)overlap, 0);
		RecvPacket(overlap);
	}

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
	// Overlapped I/O �۾����� ���۵� ������ ũ��
	DWORD   recvBytes;
	DWORD   sendBytes;
	// Completion Key�� ���� ������ ����
	Overlapped* pCompletionKey;
	// I/O �۾��� ���� ��û�� Overlapped ����ü�� ���� ������    
	Overlapped* overlap;
	DWORD   dwFlags = 0;

	while (true) {
		bool bResult = GetQueuedCompletionStatus(iocpHandle, &recvBytes, (PULONG_PTR)&pCompletionKey, (LPOVERLAPPED*)&overlap, INFINITE);

		if (recvBytes == 0)
			cout << "[INFO] ����(" << overlap->socket << ")�κ��� 0 ����Ʈ ����" << endl;

		if (recvBytes > 0) {
			cout << "[INFO] ����(" << overlap->socket << ")�κ��� ������ ����: " << overlap->wsabuf.buf << endl;

			//if (recvBytes >= sizeof(CS_LOGIN_PACKET)) {
			CS_LOGIN_PACKET* loginPacket = reinterpret_cast<CS_LOGIN_PACKET*>(overlap->wsabuf.buf);

			// loginPacket ó�� (���̵�, ��й�ȣ Ȯ�� ��)

			SC_LOGIN_BACK loginOkPacket;
			loginOkPacket.size = sizeof(SC_LOGIN_BACK);
			loginOkPacket.type = 1;  // SC_LOGIN_OK Ÿ��
			loginOkPacket.cl_id = 1; // ���� ���̵� (������ ��)
			loginOkPacket.x = 0.0f;   // ��ǥ (������ ��)
			loginOkPacket.y = 0.0f;   // ��ǥ (������ ��)
			loginOkPacket.z = 0.0f;   // ��ǥ (������ ��)
			strncpy_s(loginOkPacket.id, MAX_INFO_SIZE, loginPacket->id, MAX_INFO_SIZE);
			strncpy_s(loginOkPacket.pw, MAX_INFO_SIZE, loginPacket->pw, MAX_INFO_SIZE);
			cout << "login info : " << loginOkPacket.id << ", pw:" << loginOkPacket.pw << endl;

			// ��Ŷ�� Ŭ���̾�Ʈ���� ����
			/*DWORD sendBytes;
			if (WSASend(
				overlap->socket,
				&(overlap->wsabuf),
				1,
				&sendBytes,
				0,
				&(overlap->overlapped),
				NULL
			) == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
				cout << "[ERROR] WSASend ����: " << WSAGetLastError() << endl;
				closesocket(overlap->socket);
				delete overlap;
				continue;
			}*/

			// ���� ��Ŷ ���� �۾� ����
			RecvPacket(overlap);
			//}
		}
		else {
			// 0 ����Ʈ�� ���� - ������ ���� �� �����Ƿ� �̿� ���� ó���� ����
			cout << "[INFO] ����(" << overlap->socket << ")�κ��� 0 ����Ʈ ����" << endl;
			closesocket(overlap->socket);
			delete overlap;
		}
	}

}

void lOCPServer::RecvPacket(Overlapped* overlap)
{
	DWORD recv_flag = 0;
	ZeroMemory(&(overlap->overlapped), sizeof(overlap->overlapped));
	overlap->wsabuf.buf = reinterpret_cast<char*>(overlap->_net_buf + overlap->prev_size);
	overlap->wsabuf.len = sizeof(overlap->_net_buf) - overlap->prev_size;

	int ret = WSARecv(
		overlap->socket,
		&(overlap->wsabuf),
		1,
		0,
		&recv_flag,
		&(overlap->overlapped),
		NULL
	);

	if (SOCKET_ERROR == ret) {
		int error_num = WSAGetLastError();
		// ���� ó��
	}
}


