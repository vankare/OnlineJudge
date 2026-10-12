#pragma once

#include <iostream>

void Test(int x, bool ans)
{
    bool ret = Solution().IsPalindrome(x);
    if(ret == ans)
    {
        cout << "测试用例通过, 样例：" + to_string(x) << endl;
    }
    else
    {
        cout << "测试用例不通过, 样例：" + to_string(x) << endl;
    }
}


int main()
{
    Test(121, true);
    Test(10, false);
    return 0;
}
