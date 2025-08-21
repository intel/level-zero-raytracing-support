// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include <sycl/sycl.hpp>

template <typename T, sycl::usm::alloc AllocKind, size_t Alignment = 0>
class usm_allocator {
public:
  using value_type = T;
  using propagate_on_container_copy_assignment = std::true_type;
  using propagate_on_container_move_assignment = std::true_type;
  using propagate_on_container_swap = std::true_type;

public:
  template <typename U> struct rebind {
    typedef usm_allocator<U, AllocKind, Alignment> other;
  };

  usm_allocator() = delete;
  usm_allocator(const sycl::context& context, const sycl::device& device, const sycl::property_list& PropList = {})
    : context(context), device(device), props(PropList) {}
  
  usm_allocator(const usm_allocator& ) = default;
  usm_allocator(usm_allocator&& ) noexcept = default;
  
  usm_allocator& operator=(const usm_allocator& other) {
    context = other.context;
    device = other.device;
    props = other.props;
    return *this;
  }
  
  usm_allocator& operator=(usm_allocator&& other) {
    context = std::move(other.context);
    device = std::move(other.device);
    props = std::move(other.props);
    return *this;
  }

  template <class U>
  usm_allocator(const usm_allocator<U, AllocKind, Alignment>& other) noexcept
      : context(other.context), device(other.device), props(other.props) {}

  T* allocate(size_t n)
  {

    if (n == 0)
      return nullptr;

    auto ptr = reinterpret_cast<T*>(aligned_alloc(getAlignment(), n*sizeof(value_type), device, context, AllocKind, props));
    
    if (!ptr)
      throw std::runtime_error("USM allocation failed");

    return ptr;
  }

  void deallocate(T* ptr, size_t) {
    if (ptr) sycl::free(ptr, context);
  }

  template <class U, sycl::usm::alloc AllocKindU, size_t AlignmentU> friend bool operator==(const usm_allocator<T, AllocKind, Alignment>& first,
                                                                                            const usm_allocator<U, AllocKindU, AlignmentU>& second)
  {
    return ((AllocKind == AllocKindU) && (first.context == second.context) && (first.device == second.device));
  }

  template <class U, sycl::usm::alloc AllocKindU, size_t AlignmentU>
  friend bool operator!=(const usm_allocator<T, AllocKind, Alignment>& first, const usm_allocator<U, AllocKindU, AlignmentU>& second) {
    return !(first == second);
  }

private:
  constexpr size_t getAlignment() const { return std::max(alignof(T), Alignment); }

  sycl::context context;
  sycl::device device;
  sycl::property_list props;
};

