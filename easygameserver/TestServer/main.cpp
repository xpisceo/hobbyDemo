#include <iostream>
#include <conio.h>
using namespace std;

#include "WeSocketMgr.h"
#include "WeSocketListener.h"
#include "TestServer.h"
using namespace We;

void main()
{
	char path[256] = "";
	::GetCurrentDirectory(sizeof(path),path);
	string m_CfgPath = path;
	m_CfgPath += "/TestServer.ini";

	char tmp[1024];
	::GetPrivateProfileString( "TestServer", "Ip", "127.0.0.1", tmp, sizeof(tmp), m_CfgPath.c_str() );
	string ip = tmp;
	::GetPrivateProfileString( "TestServer", "Port", "10000", tmp, sizeof(tmp), m_CfgPath.c_str() );
	int port = ::atoi( tmp );
	::GetPrivateProfileString( "TestServer", "MaxConnection", "2000", tmp, sizeof(tmp), m_CfgPath.c_str() );
	int maxConnection = ::atoi( tmp );

	SocketMgr* sockMgr = new SocketMgr();
	TestServer testServer;
	sockMgr->AddSocketListener( 1, &testServer, maxConnection, 1024*50, 1024*10, 1024*100, 1024*10, true );
	sockMgr->StartIOCP( 0 );
	sockMgr->StartListen( 1, ip.c_str(), port );

	char commnad[256];
	while( true )
	{
		if (kbhit())
		{
			gets(commnad);
			if( stricmp(commnad,"q") == 0 || stricmp(commnad,"quit") == 0 )
				break;
			memset( commnad, 0, sizeof(commnad) );
		}
		sockMgr->Update();
		testServer.Update();
		Sleep(1);
	}

	SocketMgr::getSingleton().Shutdown();
	delete SocketMgr::getSingletonPtr();
}