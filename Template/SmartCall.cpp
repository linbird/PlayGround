//支持通过裸指针、shared_ptr、weak_ptr安全的调用成员函数

class Test
{
    private:
    int data = 100;
    public:
    Test() = default;

    int test(int x)
    {
        return x + data;
    }
};

template <typename Caller, typename Callable, typename ...Args>
auto SafeCall(const Caller* caller, Callable callable, Args ...args) -> decltype(auto)
{
    using RetType = decltype((std::declval<Caller>().*callable)(std::forward<Args>(args)...));

    if(nullptr == caller)   return RetType{};

    std::cout << __FUNCTION__ << "(raw):" << __LINE__ << ' ';
    return (const_cast<Caller*>(caller)->*callable)(std::forward<Args>(args)...);
}

template <typename Caller, typename Callable, typename ...Args>
auto SafeCall(std::weak_ptr<Caller> caller, Callable callable, Args ...args) -> decltype(auto)
{
    using RetType = decltype((std::declval<Caller>().*callable)(std::forward<Args>(args)...));

    const auto &shareCaller = caller.lock();
    if(!shareCaller)    return RetType{};

    std::cout << __FUNCTION__ << "(weak):" << __LINE__ << ' ';
    return (shareCaller.get()->*callable)(std::forward<Args>(args)...);
}

template <typename Caller, typename Callable, typename ...Args>
auto SafeCall(std::shared_ptr<Caller> caller, Callable callable, Args ...args) -> decltype(auto)
{
    using RetType = decltype((std::declval<Caller>().*callable)(std::forward<Args>(args)...));

    if(!caller)    return RetType{};

    std::cout << __FUNCTION__ << "(share):" << __LINE__ << ' ';
    return (caller.get()->*callable)(std::forward<Args>(args)...);
}


int main()
{
    auto sharedObj {std::make_shared<Test>()};
    std::weak_ptr<Test> weakObj {sharedObj};
    
    for(int i = 0; i < 12; ++i)
    {
        std::cout << i << '\n';
    std::cout << SafeCall(sharedObj, &Test::test, i) << '\n';   
    std::cout << SafeCall(weakObj, &Test::test, i+1) << '\n';   
    std::cout << SafeCall(sharedObj.get(), &Test::test, i+2) << '\n';
            std::cout << '\n';

    if(i == 7)
        sharedObj.reset();
    }   

    return 0;
}
