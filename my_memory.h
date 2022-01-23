
// 内存管理相关，如智能指针

// Copyright (c) By Song Yuhua.
// SPDX-License-Identifier: MIT

#include "mygp_type_traits.h"
#include <utility>
#include <xutility>

namespace mygp {

struct _Zero_then_variadic_args_t {
  explicit _Zero_then_variadic_args_t() = default;
};

struct _One_then_variadic_args_t {
  explicit _One_then_variadic_args_t() = default;
};

// store a pair of values, deriving from empty first
template <typename _Ty1, typename _Ty2,
          bool = is_empty_v<_Ty1> && !is_final_v<_Ty1>>
class _Compressed_pair final : private _Ty1 {
 public:
  _Ty2 _Myval2;

  using _Mybase = _Ty1;  // for visualization

  template <typename... _Other2>
  constexpr explicit _Compressed_pair(
      _Zero_then_variadic_args_t,
      _Other2&&... _Val2) noexcept(conjunction_v<is_nothrow_default_constructible<_Ty1>,
                                                 is_nothrow_constructible<
                                                     _Ty2, _Other2...>>)
      : _Ty1(), _Myval2(forward<_Other2>(_Val2)...) {}

  template <typename _Other1, typename... _Other2>
  constexpr _Compressed_pair(
      _One_then_variadic_args_t, _Other1&& _Val1,
      _Other2&&... _Val2) noexcept(conjunction_v<is_nothrow_constructible<_Ty1,
                                                                          _Other1>,
                                                 is_nothrow_constructible<
                                                     _Ty2, _Other2...>>)
      : _Ty1(forward<_Other1>(_Val1)),
        _Myval2(forward<_Other2>(_Val2)...) {}

  constexpr _Ty1& _Get_first() noexcept { return *this; }

  constexpr const _Ty1& _Get_first() const noexcept { return *this; }
};

// store a pair of values, not deriving from first
template <typename _Ty1, typename _Ty2>
class _Compressed_pair<_Ty1, _Ty2, false> final {
 public:
  _Ty1 _Myval1;
  _Ty2 _Myval2;

  template <typename... _Other2>
  constexpr explicit _Compressed_pair(
      _Zero_then_variadic_args_t,
      _Other2&&... _Val2) noexcept(conjunction_v<is_nothrow_default_constructible<_Ty1>,
                                                 is_nothrow_constructible<
                                                     _Ty2, _Other2...>>)
      : _Myval1(), _Myval2(forward<_Other2>(_Val2)...) {}

  template <typename _Other1, typename... _Other2>
  constexpr _Compressed_pair(
      _One_then_variadic_args_t, _Other1&& _Val1,
      _Other2&&... _Val2) noexcept(conjunction_v<is_nothrow_constructible<_Ty1,
                                                                          _Other1>,
                                                 is_nothrow_constructible<
                                                     _Ty2, _Other2...>>)
      : _Myval1(forward<_Other1>(_Val1)),
        _Myval2(forward<_Other2>(_Val2)...) {}

  constexpr _Ty1& _Get_first() noexcept { return _Myval1; }

  constexpr const _Ty1& _Get_first() const noexcept { return _Myval1; }
};

template <typename _Ty, typename _Dx_noref, typename = void>
struct _Get_deleter_pointer_type {
  using type = _Ty*;
};

template <typename _Ty, typename _Dx_noref>
struct _Get_deleter_pointer_type<_Ty, _Dx_noref,
                                 void_t<typename _Dx_noref::pointer>> {
  using type = typename _Dx_noref::pointer;
};

template <typename _Dx>
using _Unique_ptr_enable_default_t = enable_if_t<
    conjunction_v<negation<is_pointer<_Dx>>, is_default_constructible<_Dx>>,
    int>;

// unique_ptr
template <typename _Ty, typename _Dx>
class unique_ptr {
 public:
  using element_type = _Ty;
  using deleter_type = _Dx;
  using pointer = _Get_deleter_pointer_type<_Ty, remove_reference_t<_Dx>>::type;

  template <typename _Dx2 = _Dx, _Unique_ptr_enable_default_t<_Dx2> = 0>
  constexpr unique_ptr() noexcept : _Mypair(_Zero_then_variadic_args_t{}) {}

  // 没看懂
  template <typename _Uty, typename _Is_nullptr = is_same<_Uty, nullptr_t>>
  using _Enable_ctor_reset =
      enable_if_t<is_same_v<_Uty, pointer>               //
                  || _Is_nullptr::value                  //
                  || (is_same_v<pointer, element_type*>  //
                      && is_pointer_v<_Uty>              //
                      && is_convertible_v<remove_pointer_t<_Uty> (*)[],
                                          element_type (*)[]>)>;  // TRANSITION,
                                                                  // GH-248
  template <typename _Uty, typename _Dx2 = _Dx,
            _Unique_ptr_enable_default_t<_Dx2> = 0,
            typename = _Enable_ctor_reset<_Uty>>
  explicit unique_ptr(_Uty _Ptr) noexcept
      : _Mypair(_Zero_then_variadic_args_t{}, _Ptr) {}

  template <typename _Uty, typename _Dx2 = _Dx,
            enable_if_t<is_constructible_v<_Dx2, const _Dx2&>, int> = 0,
            typename = _Enable_ctor_reset<_Uty>>
  unique_ptr(_Uty _Ptr, const _Dx& _Dt) noexcept
      : _Mypair(_One_then_variadic_args_t{}, _Dt, _Ptr) {}

  template <typename _Uty, typename _Dx2 = _Dx,
            enable_if_t<conjunction_v<negation<is_reference<_Dx2>>,
                                      is_constructible<_Dx2, _Dx2>>,
                        int> = 0,
            typename = _Enable_ctor_reset<_Uty>>
  unique_ptr(_Uty _Ptr, _Dx&& _Dt) noexcept
      : _Mypair(_One_then_variadic_args_t{}, move(_Dt), _Ptr) {}

  template <typename _Uty, typename _Dx2 = _Dx,
            enable_if_t<
                conjunction_v<is_reference<_Dx2>,
                              is_constructible<_Dx2, remove_reference_t<_Dx2>>>,
                int> = 0>
  unique_ptr(_Uty, remove_reference_t<_Dx>&&) = delete;

  template <class _Dx2 = _Dx, enable_if_t<is_move_constructible_v<_Dx2>, int> = 0>
    unique_ptr(unique_ptr&& _Right) noexcept
        : _Mypair(_One_then_variadic_args_t{}, forward<_Dx>(_Right.get_deleter()), _Right.release()) {}

    template <class _Dx2 = _Dx, enable_if_t<is_move_assignable_v<_Dx2>, int> = 0>
    unique_ptr& operator=(unique_ptr&& _Right) noexcept {
        if (this != addressof(_Right)) {
            reset(_Right.release());
            _Mypair._Get_first() = move(_Right._Mypair._Get_first());
        }

        return *this;
    }

    void swap(unique_ptr& _Right) noexcept {
        std::_Swap_adl(_Mypair._Myval2, _Right._Mypair._Myval2);
        std::_Swap_adl(_Mypair._Get_first(), _Right._Mypair._Get_first());
    }

    ~unique_ptr() noexcept {
        if (_Mypair._Myval2) {
            _Mypair._Get_first()(_Mypair._Myval2);
        }
    }

    _NODISCARD _Dx& get_deleter() noexcept {
        return _Mypair._Get_first();
    }

    _NODISCARD const _Dx& get_deleter() const noexcept {
        return _Mypair._Get_first();
    }

    _NODISCARD add_lvalue_reference_t<_Ty> operator*() const noexcept {
        return *_Mypair._Myval2;
    }

    _NODISCARD pointer operator->() const noexcept {
        return _Mypair._Myval2;
    }

    _NODISCARD pointer get() const noexcept {
        return _Mypair._Myval2;
    }

    explicit operator bool() const noexcept {
        return static_cast<bool>(_Mypair._Myval2);
    }

    pointer release() noexcept {
        return std::exchange(_Mypair._Myval2, nullptr);
    }

    void reset(pointer _Ptr = nullptr) noexcept {
        pointer _Old = std::exchange(_Mypair._Myval2, _Ptr);
        if (_Old) {
            _Mypair._Get_first()(_Old);
        }
    }

    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;


 private:
  template <typename, typename>
  friend class unique_ptr;

  _Compressed_pair<_Dx, pointer> _Mypair;
};

}  // namespace mygp
