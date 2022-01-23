// 泛型编程需要用到的基础模板类

// Copyright (c) By Song Yuhua.
// SPDX-License-Identifier: MIT

namespace mygp {

// 基础函数
template <typename T, T v>
struct integral_constant {
  static constexpr T value = v;
  using value_type = T;
  using type = integral_constant;
  constexpr operator value_type() const noexcept { return value; }
  constexpr value_type operator()() const noexcept { return value; }
};

template <bool B>
using bool_constant = integral_constant<bool, B>;
using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

template <typename _Trait>
struct negation : bool_constant<!static_cast<bool>(_Trait::value)> {};

template <typename _Trait>
constexpr bool negation_v = negation<_Trait>::value;

// 使用模板偏特化技术，将 const 和 volatile 属性分离
// 使用 ::type 可以得到去除 const, volatile 的类型
template <typename _Ty>
struct remove_cv {
  using type = _Ty;
};

template <typename _Ty>
struct remove_cv<const _Ty> {
  using type = _Ty;
};

template <typename _Ty>
struct remove_cv<volatile _Ty> {
  using type = _Ty;
};

template <typename _Ty>
struct remove_cv<const volatile _Ty> {
  using type = _Ty;
};

template <typename _Ty>
using remove_cv_t = typename remove_cv<_Ty>::type;

// 若模板参数属于同一类型, 编译期求值 true, 否则为 false
// 实现一
#if 1
template <typename, typename>
constexpr bool is_same_v = false;

// 方法是偏特化，传入同一类型参数会优先匹配下面的模板，否则匹配上面的
template <typename _Ty>
constexpr bool is_same_v<_Ty, _Ty> = true;

template <typename _Ty1, typename _Ty2>
struct is_same : bool_constant<is_same_v<_Ty1, _Ty2>> {};
#else
// 实现二
template <typename _Ty1, typename _Ty2>
struct is_same : public false_type {};

template <typename _Ty>
struct is_same<_Ty, _Ty> : public true_type {};

template <typename _Ty1, typename _Ty2>
using is_same_v = typename is_same<_Ty1, _Ty2>::value;

#endif

// 若模板参数为 void 类型,编译期求值 true, 否则为 false
// 两种实现同 is_same
template <typename _Ty>
constexpr bool is_void_v = is_same_v<remove_cv_t<_Ty>, void>;

template <typename _Ty>
struct is_void : bool_constant<is_void_v<_Ty>> {};

template <typename... _Types>
using void_t = void;

// 若模板参数为 array 类型,编译期求值 true, 否则为 false
// 原理是 array 匹配模板时会多一个参数表示 array 中元素的数量
template <typename _Ty>
constexpr bool is_array_v = false;

template <class _Ty>
constexpr bool is_array_v<_Ty[]> = true;

template <typename _Ty, size_t _N>
constexpr bool is_array_v<_Ty[_N]> = true;

template <class _Ty>
struct is_array : bool_constant<is_array_v<_Ty>> {};

// 判断模板参数是否使用 const 修饰
template <typename>
constexpr bool is_const_v = false;

template <typename _Ty>
constexpr bool is_const_v<const _Ty> = true;

template <typename _Ty>
struct is_const : bool_constant<is_const_v<_Ty>> {};

// 判断模板参数是否使用 volatile 修饰
template <typename>
constexpr bool is_volatile_v = false;

template <typename _Ty>
constexpr bool is_volatile_v<volatile _Ty> = true;

template <typename _Ty>
struct is_volatile : bool_constant<is_volatile_v<_Ty>> {};

// 判断模板参数类型是否是引用
template <typename>
constexpr bool is_reference_v = false;

template <typename _Ty>
constexpr bool is_reference_v<_Ty&> = true;

template <typename _Ty>
constexpr bool is_reference_v<_Ty&&> = true;

template <typename _Ty>
struct is_reference : bool_constant<is_reference_v<_Ty>> {};

// determine whether _Ty is a pointer
template <typename>
constexpr bool is_pointer_v = false;

template <typename _Ty>
constexpr bool is_pointer_v<_Ty*> = true;

template <typename _Ty>
constexpr bool is_pointer_v<_Ty* const> = true;

template <typename _Ty>
constexpr bool is_pointer_v<_Ty* volatile> = true;

template <typename _Ty>
constexpr bool is_pointer_v<_Ty* const volatile> = true;

template <typename _Ty>
struct is_pointer : bool_constant<is_pointer_v<_Ty>> {};

// 判断模板参数类型是否是函数类型
// 原理是只用函数和引用类型不能被 const (再次)修饰
template <typename _Ty>
constexpr bool is_function_v = !is_const_v<const _Ty> && !is_reference_v<_Ty>;

template <class _Ty>
struct is_function : bool_constant<is_function_v<_Ty>> {};

// enable_if 实现, 当模板参数一为 true 时会优先匹配第二个实现
template <bool B, typename _Ty = void>
struct enable_if {};

template <typename _Ty>
struct enable_if<true, _Ty> {
  using type = _Ty;
};

template <bool B, typename _Ty = void>
using enable_if_t = typename enable_if<B, _Ty>::type;

// 萃取移除引用后的类型
template <typename _Ty>
struct remove_reference {
  using type = _Ty;
  using _Const_thru_ref_type = const _Ty;
};

template <typename _Ty>
struct remove_reference<_Ty&> {
  using type = _Ty;
  using _Const_thru_ref_type = const _Ty&;
};

template <typename _Ty>
struct remove_reference<_Ty&&> {
  using type = _Ty;
  using _Const_thru_ref_type = const _Ty&&;
};

template <typename _Ty>
using remove_reference_t = typename remove_reference<_Ty>::type;

// add reference (non-referenceable type)
template <typename _Ty, typename = void>
struct _Add_reference {
  using _Lvalue = _Ty;
  using _Rvalue = _Ty;
};

// add reference (referenceable type)
template <typename _Ty>
struct _Add_reference<_Ty, void_t<_Ty&>> {
  using _Lvalue = _Ty&;
  using _Rvalue = _Ty&&;
};

template <typename _Ty>
struct add_lvalue_reference {
  using type = typename _Add_reference<_Ty>::_Lvalue;
};

template <typename _Ty>
using add_lvalue_reference_t = typename _Add_reference<_Ty>::_Lvalue;

template <typename _Ty>
struct add_rvalue_reference {
  using type = typename _Add_reference<_Ty>::_Rvalue;
};

template <typename _Ty>
using add_rvalue_reference_t = typename _Add_reference<_Ty>::_Rvalue;

template <typename _Ty>
add_rvalue_reference_t<_Ty> declval() noexcept;

// 判断模板参数 From 能否隐式转换到 To
// SFINAE 技法
template <typename T>
void test_convertion(T);

template <typename From, typename To,
          typename = decltype(test_convertion<To>(declval<From>()))>
true_type test(int);

template <typename From, typename To>
false_type test(...);

template <typename From, typename To, bool = is_void_v<From>&& is_void_v<To>,
          bool = is_array_v<To> || is_function_v<To>>
struct is_convertible : false_type {};

template <typename From, typename To>
struct is_convertible<From, To, true, false> : true_type {};

template <typename From, typename To>
constexpr bool is_convertible_v = is_convertible<From, To>::value;

template <typename From, typename To>
struct is_convertible<From, To, false, false> : decltype(test<From, To>(1)) {};

template <typename T>
void test_noexcept(T) noexcept;

template <typename From, typename To, bool = is_void_v<From>>
struct is_nothrow_convertible_helper : true_type {};

template <typename From, typename To>
struct is_nothrow_convertible_helper<From, To, false>
    : integral_constant<bool, noexcept(test_noexcept<To>(declval<From>()))> {};

template <typename From, typename To, bool = is_convertible<From, To>::value>
struct is_nothrow_convertible : false_type {};

template <typename From, typename To>
struct is_nothrow_convertible<From, To, true>
    : is_nothrow_convertible_helper<From, To> {};

// 检查 _Ty 是否可以使用参数 _Args... 构造
template <typename _Ty, typename... _Args>
struct is_constructible : bool_constant<__is_constructible(_Ty, _Args...)> {};

template <typename _Ty, typename... _Args>
constexpr bool is_constructible_v = __is_constructible(_Ty, _Args...);

// 检查 _Ty 是否可以拷贝构造
template <typename _Ty>
struct is_copy_constructible : bool_constant<__is_constructible(
                                   _Ty, add_lvalue_reference_t<const _Ty>)> {};

template <typename _Ty>
constexpr bool is_copy_constructible_v =
    __is_constructible(_Ty, add_lvalue_reference_t<const _Ty>);

// 检查 _Ty 是否可以默认构造
template <typename _Ty>
struct is_default_constructible : bool_constant<__is_constructible(_Ty)> {};

template <typename _Ty>
constexpr bool is_default_constructible_v = __is_constructible(_Ty);

// 若参数 P1, P2,..., PN 中有 bool(Pi::value) == false ，则为首个 Pi
// ，否则若无这种类型，则为 PN
template <bool _First_value, typename _First, typename... _Rest>
struct _Conjunction {
  using type = _First;
};

template <typename _True, typename _Next, typename... _Rest>
struct _Conjunction<true, _True, _Next, _Rest...> {
  using type = typename _Conjunction<_Next::value, _Next, _Rest...>::type;
};

template <typename... _Traits>
struct conjunction : true_type {};

template <typename _First, typename... _Rest>
struct conjunction<_First, _Rest...>
    : _Conjunction<_First::value, _First, _Rest...>::type {};

template <typename... _Traits>
constexpr bool conjunction_v = conjunction<_Traits...>::value;

// 判断 _Ty 是否是个空类
template <typename _Ty>
struct is_empty : bool_constant<__is_empty(_Ty)> {};

template <typename _Ty>
constexpr bool is_empty_v = __is_empty(_Ty);

// 判断 _Ty 是否是个 final 类
template <typename _Ty>
struct is_final : bool_constant<__is_final(_Ty)> {};

template <typename _Ty>
constexpr bool is_final_v = __is_final(_Ty);

// determine whether direct-initialization of _Ty from _Args... is both valid
// and not potentially-throwing
template <typename _Ty, typename... _Args>
struct is_nothrow_constructible
    : bool_constant<__is_nothrow_constructible(_Ty, _Args...)> {};

template <typename _Ty, typename... _Args>
constexpr bool is_nothrow_constructible_v =
    __is_nothrow_constructible(_Ty, _Args...);


// determine whether value-initialization of _Ty is both valid and not
// potentially-throwing
template <typename _Ty>
struct is_nothrow_default_constructible
    : bool_constant<__is_nothrow_constructible(_Ty)> {};

template <typename _Ty>
constexpr bool is_nothrow_default_constructible_v =
    __is_nothrow_constructible(_Ty);


// determine whether type argument is an lvalue reference
template <class>
constexpr bool is_lvalue_reference_v = false; 

template <class _Ty>
constexpr bool is_lvalue_reference_v<_Ty&> = true;

template <class _Ty>
struct is_lvalue_reference : bool_constant<is_lvalue_reference_v<_Ty>> {};

// determine whether _Ty can be direct-initialized from an rvalue _Ty
template <class _Ty>
struct is_move_constructible : bool_constant<__is_constructible(_Ty, _Ty)> {
};

template <class _Ty>
constexpr bool is_move_constructible_v = __is_constructible(_Ty, _Ty);


// 完美转发 forward an lvalue as either an lvalue or an rvalue
template <typename _Ty>
[[nodiscard]] constexpr _Ty&& forward(
    remove_reference_t<_Ty>& _Arg) noexcept { 
    return static_cast<_Ty&&>(_Arg);
}

// forward an rvalue as an rvalue
template <typename _Ty>
[[nodiscard]] constexpr _Ty&& forward(remove_reference_t<_Ty>&& _Arg) noexcept { 
    static_assert(!is_lvalue_reference_v<_Ty>, "bad forward call");
    return static_cast<_Ty&&>(_Arg);
}

// forward _Arg as movable
template <class _Ty>
constexpr remove_reference_t<_Ty>&& move(_Ty&& _Arg) noexcept {
    return static_cast<remove_reference_t<_Ty>&&>(_Arg);
}



template <class _Ty>
[[nodiscard]] constexpr _Ty* addressof(_Ty& _Val) noexcept {
    return __builtin_addressof(_Val);
}

template <class _Ty>
const _Ty* addressof(const _Ty&&) = delete;


}  // namespace mygp
