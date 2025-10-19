
#include <iostream>
using namespace std;

// 无限递归函数
void recursive_overflow(int counter) {
    int local_array[1000]; // 每次调用消耗约4KB栈空间
    cout << "Depth: " << counter << endl;
    recursive_overflow(counter + 1); // 无限递归调用
}

int main() {
    cout << "Starting stack overflow demo..." << endl;
    recursive_overflow(1); // 触发栈溢出
    return 0;
}

