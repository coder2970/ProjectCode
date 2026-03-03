#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>

int main()
{
    // struct rlimit r;
    // r.rlim_cur = 1;
    // r.rlim_max = RLIM_INFINITY;
    // setrlimit(RLIMIT_CPU, &r);

    struct rlimit r;
    r.rlim_cur = 1024 * 1024 * 20;
    r.rlim_max = RLIM_INFINITY;
    setrlimit(RLIMIT_AS, &r);
    int count = 1;
    while (1)
    {
        int *p = new int[1024 * 1024];
        std::cout << count++ << std::endl;
    }
    return 0;
}
