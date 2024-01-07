#include <iostream>

using namespace std;

template <typename T>
void print(T n) {
    cout << "The value is: " << n << endl;
}

int main() {
    void (*funcPtr)(int) = &print<int>;
    funcPtr(10);
    return 0;
}

