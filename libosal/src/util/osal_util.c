#include "util_topic.h"
#include "osal_util.h"

int osal_sub_topic_check(const char *topic)
{
	int ret = mosquitto_sub_topic_check(topic);

	if( ret != MOSQ_ERR_SUCCESS ) return -1;
	else                          return 0;
}

int osal_pub_topic_check(const char *topic)
{
	int ret = mosquitto_pub_topic_check(topic);
	
	if(ret != MOSQ_ERR_SUCCESS) return -1;
	else                        return 0;
}

int osal_topic_matches_sub(const char *sub, const char *topic)
{
	bool result;
	int ret = mosquitto_topic_matches_sub(sub, topic, &result);

	if(ret != MOSQ_ERR_SUCCESS) return -1;

	if(result) return 0;
	else       return 1;
}