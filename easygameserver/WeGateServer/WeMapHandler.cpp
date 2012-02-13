#include "WeMapHandler.h"
#include <stdio.h>
#include "WeGateServer.h"

namespace We
{
	MapHandler::MapHandler()
	{
		m_Id = 0;
	}
	MapHandler::~MapHandler()
	{
	}
	void MapHandler::OnProcessPacket( PacketHeader* packet )
	{
	}
	void MapHandler::OnDisconnect()
	{
	}
	void MapHandler::Update()
	{
	}
}