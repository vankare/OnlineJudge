#pragma once

#ifndef __HEADER__HPP
#define __HEADER__HPP

#include "header.hpp"

#endif
#include <iostream>

void Test(const vector<int>& nums, int ans)
{
    int ret = Solution().MaxNum(nums);
    if(ret == ans)
    {
        cout << "测试用例通过" << endl;
    }
    else
    {
        cout << "测试用例不通过" << endl;
    }
}


int main()
{
    Test({1, 9, 8, 3, 2}, 9);
    Test({95, 2, 896, 2, 56}, 896);
    return 0;
}
