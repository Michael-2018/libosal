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
#ifndef __OSAL_H__
#define __OSAL_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	char *topic;
	char *data;
	unsigned int data_len;
} osal_msg_t;

typedef enum {
	OSAL_PRIORITY_HIGHEST = 1,
	OSAL_PRIORITY_HIGHER  = 2,
	OSAL_PRIORITY_NORMAL  = 3,
	OSAL_PRIORITY_LOWER   = 4,
	OSAL_PRIORITY_LOWEST  = 5,
} osal_priority_t;

typedef void (*osal_callback_t)(osal_msg_t *msg, void *arg);

/**
 *	Initial OSAL.
 */
void osal_init(void);

/**
 *	Schedule OSAL.
 *
 *	@note This function must be called periodly in main().
 *	@note This is a thread safety function.	
 */
void osal_schedule(void);

/**
 *	Register a callback function.
 *
 *	@param id   unique identify.
 *	@param cb   callback function.
 *	@param arg  user param.
 *
 *	@return \c  0: Successfully
 *	@return \c -1: Failed
 *
 *	@warning  Variable cb as NULL is not permited.
  *	@note     This is a thread safety function.	
 */
int osal_register(int id, osal_callback_t cb, void *arg);

/**
 *	Cancel a member.
 *
 *	@param id  unique identify.
 *
 *	@return \c  0: Successfully
 *	@return \c -1: Id is not exist
 */
int osal_cancel(int id);

/**
 *	Subscribe topic.
 *
 *	@param id     unique identify.
 *	@param topic  topic string.
 *
 *	@return    \c  0: Successfully
 *	@return    \c -1: Failed
 *	@return	   \c  1: Topic is Existed
 */
int osal_subscribe(int id, const char *topic);

/**
 *	Unsubscribe topic.
 *
 *	@param id     unique identify.
 *	@param topic  topic string.
 *
 *	@return \c  0: Successfully
 *  @return \c -1: Topic is Not Exist
 */
int osal_unsubscribe(int id, const char *topic);

/**
 *	Publish a event-message by indicate a destination id.
 *	This is a thread safety function.
 *
 *	@param msg       OSAL message.
  *	@param priority  Priority.
 * 
 *	@return \c  0: Successfully
 *	@return \c -1: Failed
 */
int osal_publish(osal_msg_t *msg, osal_priority_t priority);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __OSAL_H__ */