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
#include "osal_app.h"

#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>

enum
{
    OSAL_FD_READ  = 0,
    OSAL_FD_WRITE = 1,
};

static int  osal_fd[2]   = {-1, -1};
static bool osal_fd_idle = true;
static pthread_mutex_t osal_fd_mutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_t osal_app_tid_g;
static void * osal_app_running(void *arg);

/*
 *	Initial OSAL application.
 */
int osal_app_init(void)
{
    static bool osal_app_is_init = false;

    if( osal_app_is_init )  return 0;

    if( pipe(osal_fd) < 0 ) return -1;

    osal_init();

    osal_app_is_init = true;
    return 0;
}

/*
 *	Start OSAL application.
 */
int osal_app_start(void)
{
    if( pthread_create(&osal_app_tid_g, NULL, osal_app_running, NULL) == -1 )
        return -1;

    return 0;
}

/*
 *	Stop OSAL application.
 */
void osal_app_stop(void)
{
	char e  = 'e';
    int ret = write(osal_fd[OSAL_FD_WRITE], &e, 1);

    pthread_join(osal_app_tid_g, NULL);
}

/*
 *	Publish a topic.
 */
int osal_app_publish(const char *topic, const char *data, unsigned int data_len, osal_priority_t priority)
{
    osal_msg_t msg = {
        .topic    = (char *)topic,
        .data     = (char *)data,
        .data_len = data_len,
    };

    if( osal_publish(&msg, priority) != 0 )
        return -1;

    pthread_mutex_lock(&osal_fd_mutex);

    if( osal_fd_idle ) {
        char w  = 'w';
        int ret = write(osal_fd[OSAL_FD_WRITE], &w, 1);

        osal_fd_idle = false;
    }

    pthread_mutex_unlock(&osal_fd_mutex);

    return 0;
}

/*
 *	Work thread.
 */
static void * osal_app_running(void *arg)
{   
    while(1)
    {
        char r;
        if ( read(osal_fd[OSAL_FD_READ], &r, 1) <= 0 )
			continue;

        if ( r == 'e' ) break;

        pthread_mutex_lock(&osal_fd_mutex);
        osal_fd_idle = true;
        pthread_mutex_unlock(&osal_fd_mutex);

        osal_schedule();
    }

    pthread_exit(0);
    return NULL;
}
