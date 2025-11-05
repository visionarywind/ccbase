/**
 *
给定一个整数 n，计算所有小于等于 n 的非负整数中数字 1 出现的个数。

 

示例 1：

输入：n = 13
输出：6
示例 2：

输入：n = 0
输出：0
 

提示：

0 <= n <= 109
 *
*/
#include <iostream>
using namespace std;

class Solution {
public:
    int countDigitOne(int n) {
        
    }
};

public int countDigitOne(int n) {
    // 将数字n转换为字符串形式，便于按位处理
    String s = String.valueOf(n);
    
    // 获取数字的位数
    int m = s.length();
    
    // 特殊情况：如果只有一位数，只有当n>0时才有一个数字1
    if (m == 1) return n > 0 ? 1 : 0;
    
    // 计算第 i 位前缀代表的数值，和后缀代表的数值
    // 例如 abcde 则有 ps[2] = ab; ss[2] = de
    // ps数组存储前缀，ss数组存储后缀
    int[] ps = new int[m], ss = new int[m];
    
    // 初始化第一个位置的后缀值（从第二位到末尾组成的数字）
    ss[0] = Integer.parseInt(s.substring(1));
    
    // 循环计算中间位置的前缀和后缀值
    for (int i = 1; i < m - 1; i++) {
        // 前缀：从第0位到第i-1位组成的数字
        ps[i] = Integer.parseInt(s.substring(0, i));
        // 后缀：从第i+1位到末尾组成的数字
        ss[i] = Integer.parseInt(s.substring(i + 1));
    }
    
    // 最后一个位置的前缀值（从开头到倒数第二位组成的数字）
    ps[m - 1] = Integer.parseInt(s.substring(0, m - 1));
    
    // 分情况讨论每一位上数字1出现的次数
    int ans = 0;
    
    // 遍历每一位数字
    for (int i = 0; i < m; i++) {
        // x 为当前位数值，len 为当前位后面长度为多少
        int x = s.charAt(i) - '0', len = m - i - 1;
        // 获取当前位的前缀和后缀值
        int prefix = ps[i], suffix = ss[i];
        // 当前位上1出现的总次数
        int tot = 0;
        
        // 计算高位变化时当前位为1的次数
        // prefix * 10^len 表示高位每变化一次，当前位会经历10^len次循环
        tot += prefix * Math.pow(10, len);
        
        // 根据当前位的值分情况讨论
        if (x == 0) {
            // 当前位为0：不需要额外增加
        } else if (x == 1) {
            // 当前位为1：需要加上后缀部分的贡献(suffix + 1表示从0到suffix共suffix+1个数)
            tot += suffix + 1;
        } else {
            // 当前位大于1：当前位为1的情况会出现完整的10^len次
            tot += Math.pow(10, len);
        }
        
        // 累加到总结果中
        ans += tot;
    }
    
    // 返回最终结果
    return ans;
}