#include "WoSocketHandler.h"
#include "WeSocket.h"

namespace We
{
	bool SocketHandler::SendPacket( PacketHeader* packet )
	{
		if( m_Socket == 0 )
			return false;
		return m_Socket->Send( packet );
	}
}