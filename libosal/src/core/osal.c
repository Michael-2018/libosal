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
#include "osal_util.h"
#include "osal.h"
#include "list.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <bits/pthreadtypes.h>

typedef struct osal_topic_t {
    char *                topic;
    struct osal_topic_t * next;
}osal_topic_t;

typedef struct osal_node_t {
    int              id;

    void *           arg;
    osal_callback_t  cb;

    osal_topic_t     topic_list;

    struct list_head list;
} osal_node_t;

typedef struct osal_msg_node_t {
    osal_msg_t       msg;
    osal_priority_t  priority;

    struct list_head list;
} osal_msg_node_t;

#define OSAL_CALLOC(len) 		calloc(len, sizeof(char))
#define OSAL_FREE(p) 			do { if((p) != NULL) free(p); } while(0)

#define OSAL_NEW_NODE()			(osal_node_t *)OSAL_CALLOC(sizeof(osal_node_t))
#define OSAL_NEW_MSG()			(osal_msg_node_t *)OSAL_CALLOC(sizeof(osal_msg_node_t))
#define OSAL_NEW_STRING(len)	(char *)OSAL_CALLOC(len + 1)
#define OSAL_NEW_TOPIC()        (osal_topic_t *)OSAL_CALLOC(sizeof(osal_topic_t))

/*
 *  OSAL-QUEUE
 */
static osal_node_t      osal_queue_g;
static pthread_rwlock_t osal_queue_rwlock = PTHREAD_RWLOCK_INITIALIZER;

/*
 *  OSAL-MESSAGE-LIST
 */
static osal_msg_node_t  osal_msg_list_g;
static pthread_rwlock_t osal_msg_list_rwlock = PTHREAD_RWLOCK_INITIALIZER;

static void osal_handler(osal_msg_node_t *p_msg);
static int  osal_un_subscribe_common(int id, const char *topic, bool is_subscribe);

static bool osal_topic_exist(osal_topic_t *p_topic_list, const char *topic);
static bool osal_topic_match(osal_topic_t *p_topic_list, const char *topic);
static int  osal_topic_add(osal_topic_t *p_topic_list, const char *topic);
static int  osal_topic_delete(osal_topic_t *p_topic_list, const char *topic);
static int  osal_topic_delete_recursive(osal_topic_t *p_topic_list);

/*
 *  Initial OSAL.
 */
void osal_init(void)
{
	/* Initial OSAL-Queue */
    osal_queue_g.id = -1;
	osal_queue_g.cb = NULL;
    osal_queue_g.arg = NULL;
    osal_queue_g.topic_list.next = NULL;
    osal_queue_g.topic_list.topic = NULL;
	INIT_LIST_HEAD( &osal_queue_g.list );

    /* Initial OSAL-Message-List */
    osal_msg_list_g.msg.data = NULL;
    osal_msg_list_g.msg.topic = NULL;
    INIT_LIST_HEAD( &osal_msg_list_g.list );
}

/*
 *  Schedule
 */
void osal_schedule(void)
{
	osal_msg_node_t  * p_msg_node;

	struct list_head * pos, 
                     * next,
                     * head = &(osal_msg_list_g.list);

	struct list_head * continue_pos = NULL;

    do{
        p_msg_node = NULL;

        pthread_rwlock_wrlock( &osal_msg_list_rwlock );

        for ( pos = (continue_pos)?continue_pos:head->next, next = pos->next; 
              pos != head; 
              pos = next, next = pos->next ) 
        {
            p_msg_node = list_entry( pos, osal_msg_node_t, list );

            if ( --p_msg_node->priority > 0 )
            	continue;

            /* 
             * Release from the message list.
             * It's very important for this message node to escape from list, 
             * which we don't ever care about lock.
             */
            continue_pos = (next == head)?NULL : next; 
            list_del_init( pos ); 

            break;
        } 

        pthread_rwlock_unlock( &osal_msg_list_rwlock );
 
        /* Last node in list */
        if ( pos == head && p_msg_node != NULL )
            continue;

        /* List is empty */
        if ( p_msg_node == NULL )
            break;

        osal_handler( p_msg_node );

        OSAL_FREE( p_msg_node->msg.topic );
        OSAL_FREE( p_msg_node->msg.data );
        OSAL_FREE( p_msg_node );
    }while(1);
}

/*
 *  Register
 */
int osal_register(int id, osal_callback_t cb, void *arg)
{
	struct list_head * pos;
	osal_node_t      * p_osal_node = NULL;
	osal_node_t      * p_new_osal_node;

    pthread_rwlock_rdlock( &osal_queue_rwlock );

    list_for_each( pos, &(osal_queue_g.list) ) {
        p_osal_node = list_entry( pos, osal_node_t, list );

        if( p_osal_node->id == id ) {
            pthread_rwlock_unlock( &osal_queue_rwlock );
            return 0;
        }
    }

	pthread_rwlock_unlock( &osal_queue_rwlock );

    if( (p_new_osal_node = OSAL_NEW_NODE()) == NULL )
    	return -1;

    p_new_osal_node->topic_list.next  = NULL;
    p_new_osal_node->topic_list.topic = NULL;
    INIT_LIST_HEAD( &p_new_osal_node->list );

    p_new_osal_node->id  = id;
    p_new_osal_node->cb  = cb;
    p_new_osal_node->arg = arg;

    pthread_rwlock_wrlock( &osal_queue_rwlock );
    list_add_tail( &(p_new_osal_node->list), &(osal_queue_g.list) );
    pthread_rwlock_unlock( &osal_queue_rwlock );

    return 0;
}

/*
 *  Cancel
 */
int osal_cancel(int id)
{
    struct list_head * pos, * next;
    osal_node_t      * p_osal_node = NULL;

    pthread_rwlock_wrlock( &osal_queue_rwlock );

    list_for_each_safe( pos, next, &(osal_queue_g.list) ) {
        p_osal_node = list_entry(pos, osal_node_t, list);

        if(p_osal_node->id == id){
            list_del_init(pos);
            break;
        }
    } 

    pthread_rwlock_unlock( &osal_queue_rwlock );

    if( p_osal_node == NULL || p_osal_node->id != id )
        return -1;

    osal_topic_delete_recursive( &p_osal_node->topic_list );
    OSAL_FREE(p_osal_node);

    return 0;
}

/*
 *  Subscribe
 */
int osal_subscribe(int id, const char *topic)
{   
    if ( osal_sub_topic_check(topic) != 0 )
        return -1;

    return osal_un_subscribe_common( id, topic, true );
}

/*
 *  Unsubscribe
 */
int osal_unsubscribe(int id, const char *topic)
{
    return osal_un_subscribe_common( id, topic, false );
}

/*
 *  Publish
 */
int osal_publish(osal_msg_t *msg, osal_priority_t priority)
{
	struct list_head * pos;
	osal_msg_node_t  * new_msg;

    if( msg == NULL || msg->topic == NULL || 
    	osal_pub_topic_check(msg->topic) != 0 ||
    	(new_msg = OSAL_NEW_MSG()) == NULL )
    {
    	return -1;
    }

    if( msg->data_len > 0 ) {
        if( (new_msg->msg.data = OSAL_NEW_STRING(msg->data_len)) == NULL ) {
            OSAL_FREE(new_msg);
            return -1;
        }
    }
    else
        new_msg->msg.data = NULL;

    if( (new_msg->msg.topic = OSAL_NEW_STRING(strlen(msg->topic))) == NULL ) {
        OSAL_FREE( new_msg->msg.data );
        OSAL_FREE( new_msg );
        return -1;
    }
    strcpy( new_msg->msg.topic, msg->topic );

    new_msg->priority = priority;

    new_msg->msg.data_len = msg->data_len;
    if( msg->data_len > 0 ) memcpy(new_msg->msg.data, msg->data, msg->data_len);

    pthread_rwlock_wrlock( &osal_msg_list_rwlock );
    list_add_tail( &(new_msg->list), &(osal_msg_list_g.list) );
    pthread_rwlock_unlock( &osal_msg_list_rwlock );

    return 0;
}

/*
 *  Handler
 */
static void osal_handler(osal_msg_node_t *p_msg)
{
	struct list_head * head = &osal_queue_g.list,
                     * pos  = head->next;

    void             * bk_arg      = NULL;
    osal_callback_t    bk_callback = NULL;

 	osal_node_t      * p_osal_node = NULL;   
    char             * topic = p_msg->msg.topic;

    do{
        pthread_rwlock_rdlock(&osal_queue_rwlock);

        bk_arg      = NULL;
        bk_callback = NULL;
        p_osal_node = NULL;

        if( pos->next != NULL && pos != head ) {
            p_osal_node = list_entry( pos, osal_node_t, list );
            pos = pos->next;
        }
        else{
        	/* Last node */
            pthread_rwlock_unlock(&osal_queue_rwlock);
            break;
        }

        if( osal_topic_match(&p_osal_node->topic_list, topic) ) {
            bk_callback = p_osal_node->cb;
            bk_arg      = p_osal_node->arg;
        }

        pthread_rwlock_unlock(&osal_queue_rwlock);

        /*
         * Callback to user.
         * WARNING: 
         *    If bk_callback is NULL which doesn't to mean lost the callback 
         *    function, it's maybe means the topic was not matched, because 
         *    we can't callback when the topic not matched!
         */
        if( bk_callback != NULL ) bk_callback( &p_msg->msg, bk_arg );
    }while(1);
}

/*
 *  subscribe/unsubscribe
 */
static int osal_un_subscribe_common(int id, const char *topic, bool is_subscribe)
{
	int ret = -1;

	struct list_head * pos;
	osal_node_t      * p_osal_node = NULL;

	pthread_rwlock_wrlock( &osal_queue_rwlock );

	list_for_each( pos, &(osal_queue_g.list) ) {
	    p_osal_node = list_entry(pos, osal_node_t, list);
	    if( p_osal_node->id == id ) break;
	}

	if( !(p_osal_node == NULL || p_osal_node->id != id) ) {
	    if(is_subscribe) ret = osal_topic_add( &p_osal_node->topic_list, topic );
	    else             ret = osal_topic_delete( &p_osal_node->topic_list, topic );
	}

	pthread_rwlock_unlock(&osal_queue_rwlock);

	return ret;
}

/*
 *  Is topic existed?
 */
static bool osal_topic_exist(osal_topic_t *p_topic_list, const char *topic) 
{
    if( p_topic_list->topic != NULL &&
        strcmp(p_topic_list->topic, topic) == 0 )
        return true;

    if( p_topic_list->next == NULL )
        return false;

    return osal_topic_exist( p_topic_list->next, topic );
}

/*
 *  Regular expressions matches(*, +).
 */
static bool osal_topic_match(osal_topic_t *p_topic_list, const char *topic)
{
    if( p_topic_list->topic != NULL &&
        osal_topic_matches_sub(p_topic_list->topic, topic) == 0 )
        return true;

    if( p_topic_list->next == NULL )
        return false;

    return osal_topic_match( p_topic_list->next, topic );
}

/*
 *   Add a topic.
 *
 *   0: Successfully
 *  -1: Failed
 *   1: Topic is Existed
 */
static int osal_topic_add(osal_topic_t *p_topic_list, const char *topic) 
{
    if( p_topic_list->topic != NULL &&
        strcmp(p_topic_list->topic, topic) == 0 )
        return 1;

    if( p_topic_list->next != NULL )
        return osal_topic_add( p_topic_list->next, topic );

    osal_topic_t *new_topic = OSAL_NEW_TOPIC();
    if( new_topic == NULL ) return -1;

    new_topic->topic = NULL;
    new_topic->next  = NULL;

    if( (new_topic->topic = OSAL_NEW_STRING(strlen(topic))) == NULL ) {
        OSAL_FREE( new_topic );
        return -1;
    }
    strcpy( new_topic->topic, topic );

    p_topic_list->next = new_topic;

    return 0;
}

/*
 *   Delete a topic.
 *
 *   0: Success
 *  -1: Topic is Not Exist
 */
static int osal_topic_delete(osal_topic_t *p_topic_list, const char *topic)
{
	#define OSAL_TOPIC_DELETE_FINDIT    1
	#define OSAL_TOPIC_DELETE_DELETED   0
	#define OSAL_TOPIC_DELETE_ERROR    -1

    if( p_topic_list->topic != NULL &&
        strcmp(p_topic_list->topic, topic) == 0 )
        return OSAL_TOPIC_DELETE_FINDIT;

    if( p_topic_list->next == NULL )
        return OSAL_TOPIC_DELETE_ERROR;

    if( osal_topic_delete(p_topic_list->next, topic) == OSAL_TOPIC_DELETE_FINDIT ) {
        osal_topic_t *del_topic = p_topic_list->next;
        p_topic_list->next = del_topic->next;

        OSAL_FREE( del_topic->topic );
        OSAL_FREE( del_topic );

        return OSAL_TOPIC_DELETE_DELETED;
    }
}

/*
 *  Recursive to delete topic.
 */
static int osal_topic_delete_recursive(osal_topic_t *p_topic_list)
{
	#define OSAL_TOPIC_DELETE_RECURSIVE_DELETE     0
	#define OSAL_TOPIC_DELETE_RECURSIVE_PREPARE   -1

    int ret;

    if( p_topic_list->next == NULL )
        return OSAL_TOPIC_DELETE_RECURSIVE_DELETE;
    else
        ret = osal_topic_delete_recursive( p_topic_list->next );

    if( ret == OSAL_TOPIC_DELETE_RECURSIVE_DELETE ) {
        OSAL_FREE(p_topic_list->next->topic);
        OSAL_FREE(p_topic_list->next);
    }

    return ret;
}
