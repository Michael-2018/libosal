/*
 * Copyright (c) 2019-2020 Michael(172145980@qq.com).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __OSAL_WRAPPER__
#define __OSAL_WRAPPER__

#include "osal_app.h"
#include <stdio.h>

#define OSAL_INST()        osal_wrapper::inst()
#define OSAL_TERMINATE()   do{ delete OSAL_INST(); }while(0)

class osal_wrapper
{
public:
    enum
    {
        PRIORITY_HIGHEST = OSAL_PRIORITY_HIGHEST,
        PRIORITY_HIGHER  = OSAL_PRIORITY_HIGHER,
        PRIORITY_NORMAL  = OSAL_PRIORITY_NORMAL,
        PRIORITY_LOWER   = OSAL_PRIORITY_LOWER,
        PRIORITY_LOWEST  = OSAL_PRIORITY_LOWEST,
    };

public:
    static osal_wrapper * inst() 
    { 
        static osal_wrapper *inst = NULL; 
        return ( inst == NULL )?(inst = new osal_wrapper()):inst;
    }

public:
    osal_wrapper()
    { 
        (void)osal_app_init();
        (void)osal_app_start();
    }

    ~osal_wrapper()
    { 
        osal_app_stop();
    }

public:
    int do_register(int id, osal_callback_t cb, void * arg) 
	{ 
		return osal_register(id, cb, arg); 
	}
	
    int cancel(int id) 
	{ 
		return osal_cancel(id); 
	}
	
    int subscribe(int id, const char *topic) 
	{ 
		return osal_subscribe(id, topic); 
	}
	
    void unsubscribe(int id, const char *topic) 
	{ 
		(void)osal_unsubscribe(id, topic); 
	}

    int publish(const char *topic, 
                const char *data, 
                unsigned int data_len, 
                int priority = PRIORITY_NORMAL)
    {
        return osal_app_publish(topic, data, data_len, (osal_priority_t)priority);
    }
};

#endif /* __TPOOL_WRAPPER__ */

