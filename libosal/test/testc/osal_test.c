#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

void test_1(void *t)
{
    printf("Task => 1\n");
}

void test_2(void *t)
{
    printf("Task => 2\n");
}

void test_3(void *t)
{
    static int cnt = 0;

    cnt++;
	printf("Task =[%d:%s]=> 3\n", cnt, (char *)t);

    usleep(1000 * 100);
}

void test_4(void *t)
{
    usleep(1000 * 1000);
    printf("Task => 4\n");
}

void test_5(void *t)
{
    usleep(1000 * 1000 * 10);
    printf("Task => 5\n");
}

void main()
{
}
