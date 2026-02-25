#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <stdarg.h>
/* 1. 学习不定参宏函数的使用 */

// #define LOG(formatq, ...) printf("[%s:%d]" format, __FILE__, __LINE__, ##__VA_ARGS__)
// int main()
// {
//     // LOG("%s-%d\n", "hello", 1);
//     LOG("hello");

//     return 0;
// }

/* 2. c语言不定参函数的使用和访问 */

// void PrintNum(int count, ...)
// {
//     va_list ap;
//     va_start(ap, count); // 获取指定参数的起始地址,即count之后不定参的起始地址
//     for (int i = 0; i < count; i++)
//     {
//         int num = va_arg(ap, int);
//         std::cout << num << " ";
//     }
//     std::cout << std::endl;
//     va_end(ap); // 将ap指针置空
// }

// // 模拟实现printf函数
// void myprintf(const char *fmt, ...)
// {
//     va_list ap;
//     va_start(ap, fmt);
//     char *res;
//     int ret = vasprintf(&res, fmt, ap);
//     if (ret != -1)
//     {
//         /* 成功 */
//         printf("%s", res);
//         free(res);
//     }
//     va_end(ap);
// }

// int main()
// {
//     PrintNum(5, 10, 20, 30, 40, 50);
//     myprintf("hello");
//     myprintf("hello-%d", 1);

//     return 0;
// }

/* c++风格不定参函数的使用 */
void xprintf()
{
    std::cout << std::endl;
}
template <class T, class... Args>
void xprintf(const T &v, Args &&...args)
{
    std::cout << v;
    if ((sizeof... (args)) > 0) // 获取参数个数
    {
        xprintf(std::forward<Args>(args)...);
    }
    else
    {
        xprintf();
    }
}

int main()
{
    xprintf("hello");
    xprintf("hello", "world");
    return 0;
}