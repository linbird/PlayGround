#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

template <typename T>
class CustomVec : public vector<T>
{
    private:
    using vector<T>::push_back;

    public:
        void Add()
        {
            for(int i = 0; i < 12; ++i)
                push_back(i);
        }
};

int main()
{
    CustomVec<int> x;
    x.Add();
    // x.push_back(2);

//    auto cc = [](const auto& y) -> void { cout << y << endl; };
    auto cc1 = [](int sa) { cout << sa << endl; };
//    for_each(x.begin(), x.end(), cc);
    /*
    for_each(x.begin(), x.end(), [](const auto& y) -> void{
        cout << y << endl;
    });
    */
    return 0;
}
