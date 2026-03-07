#pragma once
#include "header.cpp"

// 测试用例
void test1()
{
    // 通过定义临时对象,来完成方法的调用
    bool ret = Solution().isPalindrome(121);
    if (ret)
    {
        std::cout << "通过测试用例1, 测试121通过" << std::endl;
    }
    else
    {
        std::cout << "未通过测试用例1, 测试值: 121" << std::endl;
    }
}

void test2()
{
    // 通过定义临时对象,来完成方法的调用
    bool ret = Solution().isPalindrome(-10);
    if (!ret)
    {
        std::cout << "通过测试用例2, 测试-10通过" << std::endl;
    }
    else
    {
        std::cout << "未通过测试用例2, 测试值: -10" << std::endl;
    }
}

void test2() {}

int main()
{
    test1();
    test2(); // 跑通所有test函数, 表明测试通过
}