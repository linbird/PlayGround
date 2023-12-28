//支持通过裸指针、shared_ptr、weak_ptr安全的调用成员函数

#include <functional>
#include <memory>
#include <iostream>
#include <string>

class Test
{
	private:
		int data = 100;
	public:
		Test() = default;

		int testint(int x)
		{
			return x + data;
		}

		void testvoid()
		{
			std::cout << "void \t";
		}

		void testoverload1(const std::string& str)
		{
			std::cout << "testoverload1(const string&) \t";
		}

		void testoverload1(std::string& str)
		{
			std::cout << "testoverload1(string&) \t";
		}
    
        bool testbool()
        {
            std::cout << "bool testbool \t";
            return true;
        }
};

	template <typename Caller, typename Callable, typename ...Args>
auto SafeCall(Caller* caller, Callable&& callable, Args&&...args) -> decltype(auto)
{
	using RetType = decltype((std::declval<Caller>().*std::forward<Callable>(callable))(std::forward<Args>(args)...));

	if(nullptr == caller)   return RetType();

	std::cout << __FUNCTION__ << "(raw):" << __LINE__ << ' ';
	return (const_cast<Caller*>(caller)->*std::forward<Callable>(callable))(std::forward<Args>(args)...);
}

	template <typename Caller, typename Callable, typename ...Args>
auto SafeCall(std::weak_ptr<Caller> caller, Callable&& callable, Args&&...args) -> decltype(auto)
{
	using RetType = decltype((std::declval<Caller>().*std::forward<Callable>(callable))(std::forward<Args>(args)...));

	const auto &shareCaller = caller.lock();
	if(!shareCaller)    return RetType();

	std::cout << __FUNCTION__ << "(weak):" << __LINE__ << ' ';
	return (shareCaller.get()->*std::forward<Callable>(callable))(std::forward<Args>(args)...);
}

	template <typename Caller, typename Callable, typename ...Args>
auto SafeCall(std::shared_ptr<Caller> caller, Callable&& callable, Args&&...args) -> decltype(auto)
{
	using RetType = decltype((std::declval<Caller>().*std::forward<Callable>(callable))(std::forward<Args>(args)...));

	if(!caller)    return RetType();

	std::cout << __FUNCTION__ << "(share):" << __LINE__ << ' ';
	return (caller.get()->*std::forward<Callable>(callable))(std::forward<Args>(args)...);
}

int main()
{
	auto sharedObj {std::make_shared<Test>()};
	std::weak_ptr<Test> weakObj {sharedObj};

    std::string str{"hello"};
    const std::string& cstr = str;

    SafeCall(sharedObj, (void (Test::*)(const std::string&))&Test::testoverload1, str);
    SafeCall(sharedObj, (void (Test::*)(const std::string&))&Test::testoverload1, cstr);
    SafeCall(sharedObj, (void (Test::*)(std::string&))&Test::testoverload1, str);
//    SafeCall(sharedObj, (void (Test::*)(std::string&))&Test::testoverload1, cstr);
    
	for(int i = 0; i < 12; ++i)
	{
		std::cout << i << '\n';
		std::cout << SafeCall(sharedObj, &Test::testint, i) << '\n';   
		std::cout << SafeCall(weakObj, &Test::testint, i+1) << '\n';   
		std::cout << SafeCall(sharedObj.get(), &Test::testint, i+2) << '\n';
        SafeCall(sharedObj, &Test::testvoid);   std::cout << '\n';
        SafeCall(weakObj, &Test::testvoid);   std::cout << '\n';
        SafeCall(sharedObj.get(), &Test::testvoid);   std::cout << '\n';
        std::cout << std::boolalpha << SafeCall(sharedObj, &Test::testbool) << '\n';
        std::cout << std::boolalpha << SafeCall(weakObj, &Test::testbool) << '\n';
        std::cout << std::boolalpha << SafeCall(sharedObj.get(), &Test::testbool) << '\n';
		std::cout << '\n';

		if(i == 7)
			sharedObj.reset();
	}   


	return 0;
}
