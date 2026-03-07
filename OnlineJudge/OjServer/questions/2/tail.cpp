#ifndef COMPILE_ONLINE
#include "header.cpp"

#endif

void test1()
{
    std::vector<int> v = {1, 2, 3, 4, 5, 6};
    int max = Solution().Max(v);
    if (max == 6)
    {
        std::cout << "test1通过" << std::endl;
    }
    else
    {
        std::cout << "test1未通过" << std::endl;
    }
}

void test2()
{
    std::vector<int> v = {-1, -2, -3};
    int max = Solution().Max(v);
    if (max == -1)
    {
        std::cout << "test2通过" << std::endl;
    }
    else
    {
        std::cout << "test2未通过" << std::endl;
    }
}
int main()
{
    test1();
    test2();
}