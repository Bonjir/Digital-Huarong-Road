
#include "network.h"

WORD CLIENT::wVersionRequested;
WSAData CLIENT::wsaData;

int SERVER::InitServer()
{
	//加载套接字库
	int err;
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		mvprintw(1, 3, "WSAStartup() called failed!");
		refresh();
		clear();
		return 0;
	}

	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		//若不是所请求的版本号2.2，则终止WinSock库的使用
		WSACleanup();
		return 0;
	}

	//创建用于监听的套接字
	sockServer = socket(AF_INET, SOCK_STREAM, 0);
	if (sockServer == INVALID_SOCKET)
	{
		mvprintw(1, 3, "socket() called failed! ,error code is: %d", WSAGetLastError());
		refresh();
		clear();
		return 0;
	}


	//填充服务器端套接字结构
	addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//将主机字节顺序转换成TCP/IP网络字节顺序
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(SERVERPORT);

	//将套接字绑定到一个本地地址和端口上
	err = bind(sockServer, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
	if (err == SOCKET_ERROR)
	{
		mvprintw(1, 3, "bind() called failed! The error code is: %d\n", WSAGetLastError());
		refresh();
		clear();
		return 0;
	}

	//将套接字设置为监听模式，准备接收客户请求
	err = listen(sockServer, 5);
	if (err == SOCKET_ERROR)
	{
		mvprintw(1, 3, "listen() called failed! The error code is: %d\n", WSAGetLastError());
		refresh();
		clear();
		return 0;
	}

	return 1;
}

int SERVER::WaitForClient()
{
	static SOCKET sockConn;
	sockConn = accept(sockServer, (SOCKADDR*)&addrClient, &dSzSOCKADDR);
	if (sockConn == INVALID_SOCKET)
	{
		mvprintw(1, 3, "accept() called falied! The error code is: %d\n", WSAGetLastError());
		refresh();
		clear();
		return 0;
	}
	else
	{
		this->idClnt = &sockConn;
		return 1;
	}
}

int SERVER::SendMsg(const char* sendBuf)
{
	return send(*idClnt, sendBuf, strlen(sendBuf) + 1, 0);
}

int SERVER::ReceiveMsg(char *recvBuff)
{
	memset(recvBuff, 0, 100 * sizeof(char));
	return recv(*idClnt, recvBuff, 100, 0);
}

int SERVER::FarewellClient()
{
	if(idClnt != NULL)
		return closesocket(*idClnt);
}

int CLIENT::InitClient()
{
	//加载套接字库
	int err;

	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		mvprintw(1, 3, "WSAStartup() called failed!");
		refresh();
		clear();
		return 0;
	}

	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		WSACleanup();
		return 0;
	}


	/* The WinSock DLL is acceptable. Proceed. */

	//创建套接字
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	if (sockClient == INVALID_SOCKET)
	{
		mvprintw(1, 3, "socket() called failed! ,error code is: %d", WSAGetLastError());
		refresh();
		clear();
		return 0;
	}
	return 1;
}

int CLIENT::Connect(const char *szIPAddr)
{
	//需要连接的服务端套接字结构信息
	addrServer.sin_addr.S_un.S_addr = inet_addr(szIPAddr);//设定服务器的IP地址 172.20.10.4 (.1)
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(SERVERPORT);//设定服务器的端口号(使用网络字节序)‘’

	//向服务器发出连接请求
	int err = connect(sockClient, (SOCKADDR*)&addrServer, sizeof(SOCKADDR)); // 6000 
	if (err == SOCKET_ERROR)
	{
		mvprintw(1, 3, "connect() called failed! The error code is: %d", WSAGetLastError());
		refresh();
		clear();
		return 0;
	}
	else
	{
		return 1;
	}
}

int CLIENT::ReceiveMsg(char *recvBuf)
{
	memset(recvBuf, 0, 100 * sizeof(char));
	return recv(sockClient, recvBuf, 100, 0);
}

int CLIENT::SendMsg(const char *sendBuf)
{
	return send(sockClient, sendBuf, strlen(sendBuf) + 1, 0);
}

int CLIENT::CloseClient()
{
	closesocket(sockClient);
	return WSACleanup();
	//终止套接字库的使用
}

bool GetLocalIPAddress(char *szIPAddress)
{
	strcpy(szIPAddress, GetLocalIPAddress().c_str());
	return strlen(szIPAddress);
}

string GetLocalIPAddress()
{
	std::string sAddr;
	char szAddrBuff[20];

	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret == 0)
	{
		char hostname[257];
		ret = gethostname(hostname, sizeof(hostname));
		if (ret != SOCKET_ERROR)
		{
			HOSTENT* host = gethostbyname(hostname);
			if (host != NULL)
				strcpy(szAddrBuff, inet_ntoa(*(in_addr*)*host->h_addr_list)), sAddr = szAddrBuff;
		}
		return sAddr;
	}
	else return "GetLocalIPAddress() error";
}

static string EncryptPart(int decPart)
{
	return string("") + char((decPart / 16) + 'a') + char(decPart % 16 + 'a');
}

string EncryptIP(string sIPAddr)
{
	string sRet;
	sIPAddr += '.';
	int len = sIPAddr.length(), decPart;

	for (int i = 0; i < len - 1;)
	{
		int iDotPos = sIPAddr.find('.', i);

		decPart = StringToInt(sIPAddr.substr(i, iDotPos - i));
		sRet += EncryptPart(decPart);
		i = iDotPos + 1;
	}

	return sRet;
}

static int _Char_EncryptToDec(char h)
{
	if (h >= 'a' && h <= 'p')
		return h - 'a';
	if (h >= 'A' && h <= 'P')
		return h - 'A';
	return (int)-1e9;
}

static int _TwoDigitEncryptToDec(string sHex)
{
	return _Char_EncryptToDec(sHex[0]) * 16 + _Char_EncryptToDec(sHex[1]);
}

bool DecodeIP(string *psEncryptedIP)
{
	if (psEncryptedIP->length() != 8)return 0;
	string sRet;
	int dPart[4];
	for (int i = 0; i < 4; i++)
	{
		dPart[i] = _TwoDigitEncryptToDec(psEncryptedIP->substr(i * 2, 2));
		if (dPart[i] < 0)
			return 0;
		sRet += IntToString(dPart[i]) + (i < 3 ? "." : "");
	}

	*psEncryptedIP = sRet;
	return 1;
}


/*----- class:  NETWORK -----*/

int NETWORK::InitNetWork(NETWORKTYPE type)
{
	bConnected = 0;

	switch (type)
	{
	case NETWORKTYPE_SERVER:
	{
		this->type = type;
		return this->InitServer();
	}
	case NETWORKTYPE_CLIENT:
	{
		this->type = type;
		return this->InitClient();
	}
	}
}

int NETWORK::SendMsg(const char *sendBuf)
{
	if (!bConnected)
		return 0;

	switch (this->type)
	{
	case NETWORKTYPE_SERVER:
	{
		return this->SERVER::SendMsg(sendBuf);
	}
	case NETWORKTYPE_CLIENT:
	{
		return this->CLIENT::SendMsg(sendBuf);
	}
	default:
		return 0;
	}
}

int NETWORK::ReceiveMsg(char *recvBuff)
{
	if (!bConnected)
		return 0;

	switch (this->type)
	{
	case NETWORKTYPE_SERVER:
	{
		this->SERVER::ReceiveMsg(recvBuff);
		break;
	}
	case NETWORKTYPE_CLIENT:
	{
		this->CLIENT::ReceiveMsg(recvBuff);
		break;
	}
	default:
		return 0;
	}
	return 1;
}

void NETWORK::EndUpConnect()
{
	bConnected = 0;

	switch (type)
	{
	case NETWORKTYPE_SERVER:
	{
		this->FarewellClient();
		return;
	}
	case NETWORKTYPE_CLIENT:
	{
		this->CloseClient();
		return;
	}
	}
}

int NETWORK::WaitForClient()
{
	if (this->type == NETWORKTYPE_CLIENT)
		return 0;
	return bConnected = SERVER::WaitForClient();
}

int NETWORK::Connect(const char* szIPAddr)
{
	if (this->type == NETWORKTYPE_SERVER)
		return 0;
	return bConnected = CLIENT::Connect(szIPAddr);
}

/* -- 心跳包-不阻塞 客户端 --*/

WORD HBCLIENT::wVersionRequested;
WSAData HBCLIENT::wsaData;

int HBCLIENT::InitClient()
{
	int err;
	err = WSAStartup(WINSOCK_VERSION, &wsaData);
	if (err != 0)
	{
		LogOutput("WSAStartup() called failed!");
		return 0;
	}

	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	if (sockClient == INVALID_SOCKET)
	{
		LogOutput("socket() called failed! ,error code is: %d", WSAGetLastError());
		return 0;
	}
	return 1;
}

HBCLIENT::HBCLIENT()
{
	this->bConnectAlive = 0;
}

int HBCLIENT::Connect(const char *szIPAddr)
{
	addrServer.sin_addr.S_un.S_addr = inet_addr(szIPAddr);
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(WCOL_SVRPORT);

	int err = connect(sockClient, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
	if (err == SOCKET_ERROR)
	{
		LogOutput("connect() called failed! error code: %d", WSAGetLastError());
		return 0;
	}
	else
	{
		this->bConnectAlive = 1;
		while (this->heartbeat_sender.joinable() == 0)
			this->heartbeat_sender = thread(HBCLIENT::SendHeartPack, this);
		while (this->thdRecv.joinable() == 0)
			this->thdRecv = thread(RecvMsgProc, this);
		return 1;
	}
}

void HBCLIENT::SendHeartPack(HBCLIENT* self)
{
	while (1)
	{
		char szTime[20] = { 0 };
		sprintf(szTime, "[HB]%lld", time(0));
		self->SendMsg(szTime);
		if (self->bConnectAlive);
		else
		{
			return;
		}
		std::this_thread::sleep_for(std::chrono::seconds(HEARTBEAT_DELAY));
	}
}

int HBCLIENT::ReceiveMsg(char *recvBuf)
{
	int dRet = recv(sockClient, recvBuf, 256, 0);
	if (dRet <= 0)
	{
		if(this->bConnectAlive)
			LogOutput("服务器已关闭连接");
		this->bConnectAlive = 0;
	}
	return dRet;
}

int HBCLIENT::SendMsg(const char *sendBuf)
{
	int dRet = send(sockClient, sendBuf, strlen(sendBuf) + 1, 0);
	if (dRet <= 0)
	{
		this->bConnectAlive = 0;
		LogOutput("send failed, endup connect");
	}
	return dRet;
}

int HBCLIENT::CloseClient()
{
	if (this->heartbeat_sender.joinable())
	{
		TerminateThread(this->heartbeat_sender.native_handle(), 0);
		this->heartbeat_sender.join();
	}
	if (this->thdRecv.joinable())
	{
		TerminateThread(this->thdRecv.native_handle(), 0);
		this->thdRecv.join();
	}
	this->SendMsg("[QUIT]");
	this->bConnectAlive = 0;
	char szBuff[257];
	this->ReceiveMsg(szBuff);
	LogOutput("关闭了连接");
	return WSACleanup();
}

bool HBCLIENT::IsConnectAlive()
{
	return this->bConnectAlive;
}

void HBCLIENT::RecvMsgProc(HBCLIENT *clnt)
{
	char szMsg[257];
	while (true)
	{
		int nRecv = clnt->ReceiveMsg(szMsg);
		if (clnt->IsConnectAlive() == 0)
			return;
		if(nRecv > 0 && nRecv < 257)
			szMsg[nRecv] = '\0';
		else 
			LogOutput("nRecv 值异常 (%d), szMsg: %s", nRecv, szMsg);
		clnt->mtx.lock();
		clnt->sCommBuf += szMsg;
		clnt->mtx.unlock();
	}
}

bool HBCLIENT::HaveCommandBuff()
{
	this->mtx.lock();
	int ret = this->sCommBuf.length();
	this->mtx.unlock();
	return ret;
}

string HBCLIENT::PeekCommand()
{
	this->mtx.lock();
	string sCommCpy = this->sCommBuf;
	this->mtx.unlock();
	int cmdend = -1, len = sCommCpy.length();
	for (int i = 1; i < len; i++)
	{
		if (sCommCpy.c_str()[i] == '[')
		{
			cmdend = i;
			break;
		}
	}
	if (cmdend == -1)
	{
		return sCommCpy;
	}
	else
	{
		string sCmdRet = sCommCpy.substr(0, cmdend);
		return sCmdRet;
	}
}

string HBCLIENT::PeekAllCommand()
{
	this->mtx.lock();
	string sCommCpy = this->sCommBuf;
	this->mtx.unlock();
	return sCommCpy;
}

string HBCLIENT::GetCommand()
{
	this->mtx.lock();
	string sCommCpy = this->sCommBuf;
	this->mtx.unlock();
	int cmdend = -1, len = sCommCpy.length();
	for (int i = 1; i < len; i++)
	{
		if (sCommCpy.c_str()[i] == '[')
		{
			cmdend = i;
			break;
		}
	}
	if (cmdend == -1)
	{
		mtx.lock();
		this->sCommBuf = "";
		mtx.unlock();
		return sCommCpy;
	}
	else if (cmdend > 0 && cmdend < len)
	{
		string sCmdRet = sCommCpy.substr(0, cmdend);
		mtx.lock();
		this->sCommBuf.erase(0, cmdend);
		mtx.unlock();
		return sCmdRet;
	}
	else
		LogOutput("cmdend 值异常 (%d)", cmdend); 
}
