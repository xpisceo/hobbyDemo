//
//  ConstDefine.h
//  ufun
//
//  Created by  on 12-2-9.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef ufun_ConstDefine_h
#define ufun_ConstDefine_h

#define LOGIN_WEBSERVICE     @"http://www.ufun.cc/?controller=userinterface&action=userlogin&username=$username&passwd=$passwd&sign=$sign"

#define REGIST_WEBSERVICE   @"http://www.ufun.cc/?controller=userinterface&action=userreg&username=$username&passwd=$passwd&registip=$registip&ptype=$ptype&email=$email&phone=$phone&identity=$identity&invited=$invited&sign=$sign"

#define UPDATE_PROFILE_WEBSERVICE @"http://www.ufun.cc/?controller=userinterface&action=modify&uid=$uid&nichen=$nichen&sex=$sex&city=$city&birthday=$birthday&interest=$interest&occupation=$occupation &school=$ school &sign=$sign"     //POST

#define RETRIVE_PROFILE_WEBSERVICE @"http://www.ufun.cc/?controller=userinterface&action=personal&uid=$uid"


typedef enum _AddRequestResult
{
    E_SUCCESS,
    E_FILLED,
    E_ERROR
}AddRequestResult;


//system config
#define NETWORK_REQ_BUFFER_COUNT      10
#define NETWORK_RES_BUFFER_COUNT      10


//condition lock
#define LOCK_CONDITION_NODATA          0
#define LOCK_CONDITION_HAVEDATA        1

#define SAFE_RELEASE(X)   if (X) { [X release] ; X = nil; }
#define SAFE_CFRELEASE(X) if (X) { CFRelease(X); X = nil; }

#endif
