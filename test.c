#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    printf("%lu\n", tv.tv_usec / 1000);
    sleep(5);
    gettimeofday(&tv, NULL);
    printf("%lu\n", tv.tv_usec);

    return(0);
}
