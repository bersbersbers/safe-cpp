// Copyright 2024 Sean Baxter
// Copyright 2024 Christian Mazakas
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#feature on safety

#include <std2/iterator.h>
#include <std2/panic.h>
#include <std2/slice.h>

#include <cstddef>
#include <cstring>

#include <cstdio>

namespace std2
{

template<class T+>
class vector
{
public:
  using value_type = T;
  using size_type = std::size_t;

  vector() safe
    : p_(nullptr)
    , capacity_{0}
    , size_{0}
  {
  }

  ~vector() safe {
    unsafe ::operator delete(p_);
  }

  slice_iterator<const value_type> iter(const self^) noexcept safe {
    return slice_iterator<const value_type>(self.slice());
  }

  slice_iterator<value_type> iter(self^) noexcept safe {
    return slice_iterator<value_type>(self.slice());
  }

  value_type* data(self^) noexcept safe {
    return self->p_;
  }

  const value_type* data(const self^) noexcept safe {
    return self->p_;
  }

  size_type size(const self^) noexcept safe {
    return self->size_;
  }

  size_type capacity(const self^) noexcept safe {
    return self->capacity_;
  }

  void push_back(self^, T t) safe {
    if (self.capacity() == self.size()) { self.grow(); }

    unsafe __rel_write(self->p_ + self->size_, rel t);
   ++self->size_;
  }

  [value_type; dyn]^ slice(self^) noexcept safe {
    unsafe return slice_from_raw_parts(self.data(), self.size());
  }

  const [value_type; dyn]^ slice(const self^) noexcept safe {
    unsafe return slice_from_raw_parts(self.data(), self.size());
  }

  value_type^ operator[](self^, size_type i) noexcept safe {
    if (i >= self.size()) panic_bounds("vector subscript is out-of-bounds");
    unsafe return ^self.data()[i];
  }

  bool empty(const self^) noexcept safe {
    return self.size() == 0;
  }

private:

  static
  void relocate_array(value_type* dst, value_type const* src, size_type n) {
    // TODO: we should add a relocation check here
    // this code likely isn't sound for types with non-trivial/non-defaulted
    // relocation operators
    std::memcpy(dst, src, n * sizeof(value_type));
  }

  void grow(self^) safe {
    size_type cap = self.capacity();
    size_type ncap = cap ? 2 * cap : 1;

    value_type* p;
    unsafe {
      p = static_cast<value_type*>(::operator new(ncap * sizeof(value_type)));
      relocate_array(p, self.data(), self.capacity());
      ::operator delete(self->p_);
    }

    self->p_ = (value_type*)p;
    self->capacity_ = ncap;
  }

  value_type* p_;
  size_type capacity_;
  size_type size_;
  // value_type __phantom_data;
};

} // namespace std2
