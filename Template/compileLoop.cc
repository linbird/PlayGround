#include <iostream>

// 递归情况，处理当前索引并递归调用下一次循环
template <std::size_t N>
void compileTimeLoopHelper() {
    // 在这里执行循环体内的操作，这里仅输出当前索引
    std::cout << N << std::endl;

    // 递归调用下一次循环
    compileTimeLoopHelper<N - 1>();
}

// 辅助函数，终止递归
template <>
void compileTimeLoopHelper<0>() {
    // Do nothing for the base case
}

int main() {
    // 调用编译期循环，从 5 到 1
    compileTimeLoopHelper<5>();

    return 0;
}

