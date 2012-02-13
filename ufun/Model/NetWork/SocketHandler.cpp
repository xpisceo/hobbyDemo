//
//  SocketHandler.cpp
//  ufun
//
//  Created by  on 12-2-13.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "SocketHandler.h"
#include "Socket.h"

bool SocketHandler::SendPacket( PacketHeader* packet )
{
    if( m_Socket == 0 )
        return false;
    return m_Socket->Send( packet );
}