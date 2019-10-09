#include "osal_wrapper.hpp"
#include <string.h>

#include <iostream>
#include <unistd.h>

using namespace std;

void test_callback(osal_msg_t *msg, void *arg)
{
    printf("Topic : %s\n", msg->topic);
    printf("Data  : %s\n", msg->data);
    printf("Length: %d\n", msg->data_len);
}

int main()
{
    OSAL_INST()->do_register(1, test_callback, NULL);

    OSAL_INST()->subscribe(1, "/test/+");

    while(1)
    {
        OSAL_INST()->publish("/test/1", "111", 3, osal_wrapper::PRIORITY_LOWEST);
        OSAL_INST()->publish("/test/2", "222", 3, osal_wrapper::PRIORITY_LOWER);
        OSAL_INST()->publish("/test/3", "333", 3, osal_wrapper::PRIORITY_HIGHER);
        OSAL_INST()->publish("/test/4", "444", 3, osal_wrapper::PRIORITY_HIGHEST);
        OSAL_INST()->publish("/test/5", "555", 3);
        OSAL_INST()->publish("/test/6", "666", 3);
        OSAL_INST()->publish("/test/7", "777", 3);

        break;
    }

    usleep(1000 * 1000 * 1);
    OSAL_TERMINATE();
}
