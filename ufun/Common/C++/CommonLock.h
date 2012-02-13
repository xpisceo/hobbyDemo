//
//  WinLock.h
//  ufun
//
//  Created by  on 12-2-13.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef ufun_WinLock_h
#define ufun_WinLock_h

#include<pthread.h>

// 临界区
class CommonLock
{
public:
    CommonLock() { pthread_mutex_init(&lock, NULL); }
    virtual ~CommonLock() { pthread_mutex_destroy(&lock); }
    
    inline void Lock()
    {
        pthread_mutex_lock(&lock);
    }
    
    inline void Unlock()
    {
        pthread_mutex_unlock(&lock);
    }
    
protected:
    pthread_mutex_t lock;
};

// 局部自动锁
class AutoLock
{
protected:
    CommonLock* m_Lock;
public:
    AutoLock( CommonLock* lock )
    {
        m_Lock = lock;
        if( m_Lock )
            m_Lock->Lock();
    }
    ~AutoLock()
    {
        if( m_Lock )
            m_Lock->Unlock();
    }
};

#endif
