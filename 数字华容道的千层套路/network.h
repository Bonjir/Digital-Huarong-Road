#pragma once
#ifndef _NETWORK_H_
#define _NETWORK_H_

#define SERVERPORT 6000	//服务端口号

//#define LOCAL
#ifdef LOCAL 
#define SERVER_ADDR "127.0.0.1"
#else
#define SERVER_ADDR "39.106.13.42"
#endif

#define WCOL_SVRPORT 8668


#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#define HAVE_STRUCT_TIMESPEC

#pragma comment(lib, "Ws2_32.lib")
//#pragma comment(lib, "pthreadVC2.lib")

#include <WinSock2.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <thread>
//#include <pthread.h>
#include <mutex>
#include "debug.h"
#include "log.h"

using std::string;
using std::thread;
using std::mutex;

bool GetLocalIPAddress(char *szIPAddress);
string GetLocalIPAddress();
string EncryptIP(string sIPAddr);
bool DecodeIP(string *psEncryptedIP);

typedef SOCKET(*CLIENTID);

class SERVER
{
protected:
	virtual int WaitForClient();
	virtual int SendMsg(const char* sendBuf);
	virtual int ReceiveMsg(char *recvBuff);
	virtual int FarewellClient();
	int InitServer();
	

	WORD wVersionRequested;//用于保存WinSock库的版本号
	WSADATA wsaData;
	SOCKET sockServer;
	SOCKADDR_IN addrServer;
	SOCKADDR_IN addrClient;	//保存发送请求连接的客户端的套接字信息
	int dSzSOCKADDR = sizeof(SOCKADDR);
	CLIENTID idClnt;
};

class CLIENT
{
protected:
	virtual int Connect(const char *szIPAddr);
	virtual int ReceiveMsg(char *recvBuf);
	virtual int SendMsg(const char *sendBuf);
	int InitClient();
	int CloseClient();

	static WORD wVersionRequested;
	static WSAData wsaData;

	SOCKET sockClient;
	SOCKADDR_IN addrServer;
};

enum NETWORKTYPE {NETWORKTYPE_SERVER, NETWORKTYPE_CLIENT};

class NETWORK : SERVER, CLIENT
{
public:
	int InitNetWork(NETWORKTYPE type);
	int SendMsg(const char *sendBuf);
	int ReceiveMsg(char *recvBuff);
	void EndUpConnect();
	int WaitForClient();
	int Connect(const char *szIPAddr);

protected:
	NETWORKTYPE type;
	int bConnected = 0;
};

class HBCLIENT
{
public:
	HBCLIENT();
	int Connect(const char *szIPAddr);
	int ReceiveMsg(char *recvBuf);
	int SendMsg(const char *sendBuf);
	int InitClient();
	int CloseClient();
	bool IsConnectAlive();

	bool HaveCommandBuff();
	string PeekCommand();
	string PeekAllCommand();
	string GetCommand();

protected: 
	static WORD wVersionRequested;
	static WSAData wsaData;

	SOCKET sockClient;
	SOCKADDR_IN addrServer;
	static void SendHeartPack(HBCLIENT* self);
	thread heartbeat_sender;
	bool bConnectAlive;
#define HEARTBEAT_DELAY 10

	mutex mtx;
	thread thdRecv;
	string sCommBuf;
	static void RecvMsgProc(HBCLIENT *self);

};


#endif
