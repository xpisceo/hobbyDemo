//
//  SocketHandler.h
//  ufun
//
//  Created by  on 12-2-13.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef ufun_SocketHandler_h
#define ufun_SocketHandler_h

#include "PackageHeader.h"

class Socket;

class SocketHandler
{
public:
    SocketHandler() { m_Socket = NULL; }
    
    /// ¥¶¿ÌÕ¯¬Á ˝æ›∞¸
    virtual void OnProcessPacket( PacketHeader* packet ) = 0;
    
    /// ¡¨Ω”∂œø™
    virtual void OnDisconnect() = 0;
    
    void SetSocket( Socket* socket ) { m_Socket = socket; }
    Socket* GetSocket() { return m_Socket; }
    
    bool SendPacket( PacketHeader* packet );
    
protected:
    Socket* m_Socket;
};

#endif
