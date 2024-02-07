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

	iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	cout << "���� ����" << endl;
	if (!CreateWorkerThreads()) return;
	while (true)
	{
		PostAccept();
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
	while (true)
	{
		DWORD bytesTransferred;
		ULONG_PTR completionKey;
		Overlapped* overlapped;

		// IO �Ϸ� ��Ŷ�� ��ٸ��ϴ�.
		if (!GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &completionKey, (LPOVERLAPPED*)&overlapped, INFINITE))
		{
			// ���� ó�� (�߰����� ���� ó���� �߰��� �� ����)
			cout << "GetQueuedCompletionStatus failed with error: " << GetLastError() << endl;
			cout << "??";
			continue;
		}
		// IO �۾� ������ �����ϱ� ���� �Ϸ� Ű�� Ȯ��
		switch (overlapped->type)
		{
		case IO_RECV:
			// ���� �Ϸ� ó��
			HandleReceive(overlapped, bytesTransferred);
			break;
		case IO_SEND:
			// �۽� �Ϸ� ó��
			HandleSend(overlapped, bytesTransferred);
			break;
		case IO_ACCEPT:
			// Accept �Ϸ� ó��
			HandleAccept(overlapped, bytesTransferred);
			break;
			// �ʿ信 ���� �� ���� ��츦 �߰� (��: IO_CONNECT)
		default:
			cout << "Unknown IO type" << endl;
			break;
		}
	}
}


void lOCPServer::HandleReceive(Overlapped* overlapped, DWORD bytesTransferred)
{
	// ���ŵ� ������ ó��, ��Ŷ ó�� ���� ����
	// overlapped->recvBuffer���� ���ŵ� �����Ϳ� �׼����� �� �ֽ��ϴ�.

	// ����: ���ŵ� ������ ���
	cout << "Received data: " << overlapped->recvBuffer << endl;

	// ����ؼ� ���ο� ���� �۾��� ���
	PostRecv(overlapped);
}

void lOCPServer::HandleSend(Overlapped* overlapped, DWORD bytesTransferred)
{
	// �۽� �۾� �Ϸ� ó��, �ʿ信 ���� �߰� ���� ����
	// ...
	cout << "Send data: " << overlapped->recvBuffer << endl;
	PostSend(overlapped);
	// �ʿ��� ��� ���ҽ� ����, ���� �ݱ� ���� ����
}

void lOCPServer::HandleAccept(Overlapped* overlapped, DWORD bytesTransferred)
{
	// Accept �۾� �Ϸ� ó��, �ʿ信 ���� ���ο� ��Ŀ ������ ���� �Ǵ� ������ ���� ó�� ���� ����
	// ...
	//cout << "Send data: " << overlapped->recvBuffer << endl;
	// ���ο� Accept �۾��� ����Ͽ� ����ؼ� ������ ������ ����
	cout << "�����" << endl;
	PostAccept();
}


void lOCPServer::PostRecv(Overlapped* overlapped)
{
	// ���ο� ���� �۾��� �غ��ϰ� ���
	DWORD flags = 0;
	DWORD recvBytes = 0;

	int result = WSARecv(overlapped->socket, &(overlapped->wsabuf), 1, &recvBytes, &flags, &(overlapped->overlapped), NULL);

	if (result == SOCKET_ERROR && (WSAGetLastError() != WSA_IO_PENDING))
	{
		cout << "WSARecv failed with error: " << WSAGetLastError() << endl;
	}
}

void lOCPServer::PostSend(Overlapped* overlapped)
{
	// ���ο� �۽� �۾��� �غ��ϰ� ���
	DWORD sendBytes = 0;

	int result = WSASend(overlapped->socket, &(overlapped->wsabuf), 1, &sendBytes, 0, &(overlapped->overlapped), NULL);

	if (result == SOCKET_ERROR && (WSAGetLastError() != WSA_IO_PENDING))
	{
		cout << "WSASend failed with error: " << WSAGetLastError() << endl;
	}
}


void lOCPServer::PostAccept()
{
	int result;
	// Ŭ���̾�Ʈ ����
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	DWORD recvBytes;
	DWORD flags;
	SOCKET clientsocket = WSAAccept(listensocket, (struct sockaddr*)&clientAddr, &addrLen, NULL, NULL);
	if (clientsocket == INVALID_SOCKET) {
		cout << "accept ����" << endl;
		return;
	}
	Overlapped* overlap = new Overlapped();
	overlap->socket = clientsocket;
	overlap->recvBytes = 0;
	overlap->sendBytes = 0;
	overlap->wsabuf.len = 1024;
	overlap->wsabuf.buf = overlap->recvBuffer;
	flags = 0;
	::CreateIoCompletionPort((HANDLE)clientsocket, iocpHandle, (DWORD)overlap, 0);

	// Ŭ���̾�Ʈ���Լ� ù ��° ��Ŷ�� �ޱ� ���� Recv ȣ��
	PostRecv(overlap);
}