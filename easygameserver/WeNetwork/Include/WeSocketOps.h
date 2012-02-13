/********************************************************************
	created:	2009-6-10
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	Socket的一些设置
*********************************************************************/
#pragma once

#include "WeConfig.h"
#include <winsock2.h>

namespace We
{
	class SocketOps
	{
	public:
		// Create file descriptor for socket i/o operations.
		static SOCKET CreateTCPFileDescriptor();

		// Disable blocking send/recv calls.
		static bool Nonblocking(SOCKET fd);

		// Enable blocking send/recv calls.
		static bool Blocking(SOCKET fd);

		// Disable nagle buffering algorithm
		static bool DisableBuffering(SOCKET fd);

		// Enables nagle buffering algorithm
		static bool EnableBuffering(SOCKET fd);

		// Set internal buffer size to socket.
		static bool SetRecvBufferSize(SOCKET fd, uint32 size);

		// Set internal buffer size to socket.
		static bool SetSendBufferSize(SOCKET fd, uint32 size);

		// Set timeout, in seconds
		static bool SetTimeout(SOCKET fd, uint32 timeout);

		// Closes socket completely.
		static void CloseSocket(SOCKET fd);

		// Sets SO_REUSEADDR
		static void ReuseAddr(SOCKET fd);
	};
};
