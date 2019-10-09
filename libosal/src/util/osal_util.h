#ifndef __OSAL_UTIL_H__
#define __OSAL_UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 *	Check that a topic used for subscriptions is valid.
 *
 *	@param topic  the topic to check
 *
 *	@return \c  0 : Valid
 *	@return \c -1 : Invalid
 */
int osal_sub_topic_check(const char *topic);

/**
 *	Check that a topic used for publishing is valid.
 *
 *	@param topic  the topic to check
 *
 *	@return \c  0 : Valid
 *	@return \c -1 : Invalid
 */
int osal_pub_topic_check(const char *topic);

/**
 *	Check whether a topic matches a subscription.
 *
 *	@param sub    subscription string to check topic against.
 *	@param topic  topic to check.
 *
 *	@return \c  0 : Matche
 *	@return \c  1 : Not match
 *  @return \c -1 : Error
 */
int osal_topic_matches_sub(const char *sub, const char *topic);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __OSAL_UTIL_H__ */
