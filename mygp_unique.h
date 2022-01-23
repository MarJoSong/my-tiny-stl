// Simple Unique_ptr

// Copyright (c) By Song Yuhua.
// SPDX-License-Identifier: MIT


#include "mygp_type_traits.h"

namespace mygp {






template <typename T>
class Unique_ptr {
 public:
  Unique_ptr(T* ptr = nullptr) : mPtr(ptr){};

  ~Unique_ptr() {
    if (mPtr) delete mPtr;
  }

  Unique_ptr(Unique_ptr && p) noexcept;
  Unique_ptr& operator=(Unique_ptr && p) noexcept;

  Unique_ptr(const Unique_ptr & p) = delete;
  Unique_ptr& operator=(Unique_ptr & p) = delete;

  T& operator*() const { return *mPtr; }
  T* operator->() const { return mPtr; }
  explicit operator bool() const noexcept { return mPtr; }

  void reset(T* q = nullptr) noexcept {
    if (q != mPtr) {
      if (mPtr)
        delete = mPtr;
      mPtr = q;
    }
  }

  T* release() noexcept {
    T* res = mPtr;
    mPtr = nullptr;
    return res;
  }

  T* get() const noexcept { return mPtr; }

  void swap(Unique_ptr & p) noexcept {
    using std::swap;
    swap(mPtr, p.mPtr);
  }

 private:
  T* mPtr;
};

template <typename T>
Unique_ptr<T>& Unique_ptr<T>::operator=(Unique_ptr && p) {
#if 0
  if (*this != p) {
    if (mPtr)
      delete ptr;
    mPtr = p.mPtr;
    p.mPtr = nullptr;
  }
#endif
  swap(*this, p);

  return *this;
}

template <typename T>
Unique_ptr<T>::Unique_ptr(Unique_ptr && p) : mPtr(p.mPtr) {
  p.mPtr = nullptr;
}

}  // namespace mygp
