#include <iostream>
#include <tuple>
#include <sstream>
#include <utility>

using namespace std;

// 获取函数类型的参数信息
template <typename Ret, typename ...Args>
struct function_traits;

template <typename _Ret, typename ..._Args>
struct function_traits<_Ret (*)(_Args...)>
{
    using Ret = _Ret;
    constexpr static size_t ArgSize = sizeof...(_Args);
    using args_tuple = tuple<_Args...>;
};

// 辅助函数，用于展开 tuple 中的参数并调用普通函数
template <typename Func, typename Tuple, std::size_t... Indices>
constexpr decltype(auto) apply_helper(Func&& func, Tuple&& tuple, std::index_sequence<Indices...>) {
    return std::forward<Func>(func)(std::get<Indices>(std::forward<Tuple>(tuple))...);
}

// 主函数，用于展开 tuple 中的参数并调用普通函数
template <typename Func, typename Tuple>
constexpr decltype(auto) apply(Func&& func, Tuple&& tuple) {
    return apply_helper(std::forward<Func>(func), std::forward<Tuple>(tuple),
                        std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
}

// 辅助函数，用于替换 tuple 的前半部分
template <std::size_t N, typename TupleA, typename TupleB, std::size_t... IndicesA, std::size_t... IndicesB>
constexpr auto replace_prefix(const TupleA& a, const TupleB& b, std::index_sequence<IndicesA...>, std::index_sequence<IndicesB...>) {
    return std::make_tuple(std::get<IndicesA>(a)..., std::get<N + IndicesB>(b)...);
}

// 主函数，用于替换 tuple 的前半部分
template <typename TupleA, typename TupleB>
constexpr auto replace_prefix(const TupleA& a, const TupleB& b) {
    static_assert(std::tuple_size<TupleA>::value <= std::tuple_size<TupleB>::value, "TupleA must be shorter than or equal to TupleB");
    return replace_prefix<std::tuple_size<TupleA>::value>(a, b, std::make_index_sequence<std::tuple_size<TupleA>::value>{}, std::make_index_sequence<std::tuple_size<TupleB>::value - std::tuple_size<TupleA>::value>{});
}

// 自定义函数调用逻辑
template <typename F, typename ...Args>
void custom_call(F&& fun, Args&& ...args)
{
    constexpr size_t expectArgsSize = function_traits<F>::ArgSize;
    constexpr size_t passArgsSize = sizeof...(args);
    if constexpr(expectArgsSize == passArgsSize)
        std::forward<F>(fun)(std::forward<Args>(args)...);
    else
    {
        using params_type = typename function_traits<F>::args_tuple;
        params_type defaultParams;
        tuple<Args...> passedParams = make_tuple(forward<Args>(args)...);
        auto result = replace_prefix(passedParams, defaultParams);
        ::apply(fun, result);
    }
}

// 示例函数
void f(int a, char b = 'b', float c = 0.1) {
    ostringstream oss;
    oss << "a = " << a << ", b = " << b << ",c = " << c << "\n";
    cout << oss.str();
}

int main() {
    custom_call(&f);
    custom_call(&f, 1);
    custom_call(&f, 1, ' ');
    custom_call(&f, 1, ' ', 4.6);
    return 0;
}
