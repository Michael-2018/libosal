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
#ifndef __OSAL_APP_H__
#define __OSAL_APP_H__

#include "osal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *	Initial OSAL application.
 *
 *	@return \c  0: Successfully
 *	@return \c -1: Failed
 */
int osal_app_init(void);

/**
 *	Start OSAL application.
 *
 *	@return \c  0: Successfully
 *	@return \c -1: Failed
 */
int osal_app_start(void);

/**
 *	Stop OSAL application.
 */
void osal_app_stop(void);

/**
 *	Publish a topic.
 *
 *	@param topic     topic string
 *	@param data      data
 *  @param data_len  data length
 *  @param priority  priority
 *
 *	@return \c  0: Successfully
 *	@return \c -1: Failed
 */
int osal_app_publish(const char *topic, const char *data, unsigned int data_len, osal_priority_t priority);

/*
extern int osal_register(int id, osal_callback_t cb, void *arg);
extern int osal_cancel(int id);
extern int osal_subscribe(int id, const char *topic);
extern int osal_unsubscribe(int id, const char *topic);
*/

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __OSAL_APP_H__ */
