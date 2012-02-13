#include <iostream>
#include <Signal.h>
#include <conio.h>
using namespace std;

#include "WeSocketMgr.h"
#include "TestClient.h"
using namespace We;

bool s_Running = false;
void _OnSignal(int s)
{
	switch (s)
	{
	case SIGINT:
	case SIGTERM:
	case SIGABRT:
	case SIGBREAK:
		{
			if( s_Running )
			{
				s_Running = false;
				SocketMgr::getSingleton().Shutdown();
				delete SocketMgr::getSingletonPtr();
			}
		}
		break;
	}

	signal(s, _OnSignal);
}

void main()
{
	char path[256] = "";
	::GetCurrentDirectory(sizeof(path),path);
	string m_CfgPath = path;
	m_CfgPath += "/TestClient.ini";

	char tmp[1024];
	::GetPrivateProfileString( "TestClient", "Ip", "127.0.0.1", tmp, sizeof(tmp), m_CfgPath.c_str() );
	string ip = tmp;
	::GetPrivateProfileString( "TestClient", "Port", "10000", tmp, sizeof(tmp), m_CfgPath.c_str() );
	int port = ::atoi( tmp );
	::GetPrivateProfileString( "TestClient", "TestNum", "100", tmp, sizeof(tmp), m_CfgPath.c_str() );
	int testClientNum = ::atoi( tmp );

	printf( "TestServer IP=%s port=%d\n", ip.c_str(), port );

	signal(SIGINT, _OnSignal);
	signal(SIGTERM, _OnSignal);
	signal(SIGABRT, _OnSignal);
	signal(SIGBREAK, _OnSignal);

	new SocketMgr();
	s_Running = true;
	TestClient* clients = new TestClient[testClientNum];
	for( int i=0; i<testClientNum; ++i )
	{
		clients[i].m_Id = i+1;
		SocketMgr::getSingleton().AddSocketConnector( i+1, &clients[i], ip.c_str(), port, 1024, 1024*10, 1024*10, 1024*50, true );
	}
	SocketMgr::getSingleton().StartIOCP( 0 );

	printf( "Running Loop, Press 'q' to Exit !!!\n" );
	char commnad[256];
	while( s_Running )
	{
		if (kbhit())
		{
			gets(commnad);
			if( stricmp(commnad,"q") == 0 || stricmp(commnad,"quit") == 0 )
				break;
			memset( commnad, 0, sizeof(commnad) );
		}
		for( int i=0; i<testClientNum; ++i )
		{
			if( i == testClientNum-1 )
				clients[i].Update();
			else
				clients[i].Update();
		}
		SocketMgr::getSingleton().Update();
		Sleep(1);
	}

	SocketMgr::getSingleton().Shutdown();
	delete SocketMgr::getSingletonPtr();
	delete []clients;
}